// === quadtree_world_click_to_move.c ===
// C SDL2 demo: finite quadtree-backed world with hybrid camera/player movement,
// invisible walls, mouse-influenced character facing direction, and click-to-move

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <cstddef>
#include <random>
#include <thread>

// Screen and tile dimensions -------------------------------------------------------------------------------
#define SCREEN_WIDTH 800  // Viewport width in pixels
#define SCREEN_HEIGHT 600 // Viewport height in pixels
#define TILE_SIZE 64      // Size of each square tile in pixels

// World dimensions ------------------------------------------------------------------------------------------
#define WORLD_COLS 30                            // Horizontal tile count
#define WORLD_ROWS 20                            // Vertical tile count
#define WORLD_CAPACITY (WORLD_COLS * WORLD_ROWS) // Total tiles

// Player and camera settings --------------------------------------------------------------------------------
#define PLAYER_W 70                                        // Player sprite width
#define PLAYER_H 70                                        // Player sprite height
#define MOVE_SPEED 5                                       // Player movement speed
#define MARGIN_X ((SCREEN_WIDTH - PLAYER_W) / 2)           // Horizontal free-move margin
#define MARGIN_Y ((SCREEN_HEIGHT - PLAYER_H) / 2)          // Vertical free-move margin
#define MAX_CAM_X (WORLD_COLS * TILE_SIZE - SCREEN_WIDTH)  // Camera X bounds
#define MAX_CAM_Y (WORLD_ROWS * TILE_SIZE - SCREEN_HEIGHT) // Camera Y bounds

// Hysteresis dead-zone ------------------------------------------------------------------------------------------------
#define HYST 5 // pixels of dead-zone around the margin threshold

// Direction enumeration -------------------------------------------------------------------------------------
enum Facing
{
    DOWN = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3
};

// Tile data structure ----------------------------------------------------------------------------------------
typedef struct
{
    int x, y;
} Vec2; // Grid coordinate
typedef struct
{
    Vec2 key;
    bool hasTree;
} QuadtreeNode;                                 // Tile node
static QuadtreeNode worldNodes[WORLD_CAPACITY]; // All tiles

// Initialize static tile world ---------------------------------------------------------------------------------
void initWorld()
{
    int idx = 0;
    for (int y = 0; y < WORLD_ROWS; ++y)
    {
        for (int x = 0; x < WORLD_COLS; ++x)
        {
            worldNodes[idx].key.x = x;
            worldNodes[idx].key.y = y;
            worldNodes[idx].hasTree = ((x + y) % 3 == 0); // Example pattern
            idx++;
        }
    }
}

//  Smooth Camera Follow (Interpolation) ---------------------------------------------------------------------------------
static float camlerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

int main()
{
    // SDL initialization -----------------------------------------------------------------------------------------
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Create window and renderer ----------------------------------------------------------------------------------
    SDL_Window *window = SDL_CreateWindow("Click-to-Move Demo",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Load directional player textures using SDL_Texture directly --------------------------------------------------
    SDL_Texture *texDown = IMG_LoadTexture(renderer, "playerAssets/playerForward.png");
    SDL_Texture *texLeft = IMG_LoadTexture(renderer, "playerAssets/playerWalkingLeft1.png");
    SDL_Texture *texRight = IMG_LoadTexture(renderer, "playerAssets/playerWalkingRight1.png");
    SDL_Texture *texUp = IMG_LoadTexture(renderer, "playerAssets/playerBackwards.png");

    // Build the world ---------------------------------------------------------------------------------------------
    initWorld();

    // Initial player and camera state -----------------------------------------------------------------------------
    int worldPX = (WORLD_COLS * TILE_SIZE) / 2 - PLAYER_W / 2;
    int worldPY = (WORLD_ROWS * TILE_SIZE) / 2 - PLAYER_H / 2;
    float camXf = 0.0f, camYf = 0.0f;
    int camX, camY;
    int facing = DOWN;

    // Click-to-move target ----------------------------------------------------------------------------------------
    int targetPX = worldPX;
    int targetPY = worldPY;
    bool movingToClick = false;

    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        // --- Input & Events ----------------------------------------------------------------------------------------
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                // Cancel click-to-move on keyboard input
                movingToClick = false;
                int dx = 0, dy = 0;
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    dy = -MOVE_SPEED;
                    break;
                case SDLK_DOWN:
                    dy = MOVE_SPEED;
                    break;
                case SDLK_LEFT:
                    dx = -MOVE_SPEED;
                    break;
                case SDLK_RIGHT:
                    dx = MOVE_SPEED;
                    break;
                }
                // Keyboard movement with invisible walls
                worldPX = SDL_clamp(worldPX + dx, 0, WORLD_COLS * TILE_SIZE - PLAYER_W);
                worldPY = SDL_clamp(worldPY + dy, 0, WORLD_ROWS * TILE_SIZE - PLAYER_H);
                // Camera hybrid logic
                int scrX = worldPX - camX;
                int scrY = worldPY - camY;
                if (scrX < MARGIN_X)
                    camX = worldPX - MARGIN_X;
                else if (scrX > SCREEN_WIDTH - MARGIN_X - PLAYER_W)
                    camX = worldPX - (SCREEN_WIDTH - MARGIN_X - PLAYER_W);
                if (scrY < MARGIN_Y)
                    camY = worldPY - MARGIN_Y;
                else if (scrY > SCREEN_HEIGHT - MARGIN_Y - PLAYER_H)
                    camY = worldPY - (SCREEN_HEIGHT - MARGIN_Y - PLAYER_H);
                camX = SDL_clamp(camX, 0, MAX_CAM_X);
                camY = SDL_clamp(camY, 0, MAX_CAM_Y);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                // Set click-to-move target in world coordinates (centered)
                int mx = e.button.x;
                int my = e.button.y;
                targetPX = mx + camX - PLAYER_W / 2;
                targetPY = my + camY - PLAYER_H / 2;
                // Clamp target within world bounds
                targetPX = SDL_clamp(targetPX, 0, WORLD_COLS * TILE_SIZE - PLAYER_W);
                targetPY = SDL_clamp(targetPY, 0, WORLD_ROWS * TILE_SIZE - PLAYER_H);
                movingToClick = true;
            }
        }

        // --- Click-to-Move Logic (refactored) ------------------------------------------------
        if (movingToClick)
        {
            // 1) Compute how far we are from the target
            int diffX = targetPX - worldPX;
            int diffY = targetPY - worldPY;

            // 2) Clamp our step so we never overshoot
            int dx = 0, dy = 0;
            if (diffX > MOVE_SPEED)
                dx = MOVE_SPEED;
            else if (diffX < -MOVE_SPEED)
                dx = -MOVE_SPEED;
            else
                dx = diffX; // the last small bit

            if (diffY > MOVE_SPEED)
                dy = MOVE_SPEED;
            else if (diffY < -MOVE_SPEED)
                dy = -MOVE_SPEED;
            else
                dy = diffY; // the last small bit

            // 3) Move
            worldPX += dx;
            worldPY += dy;

            // 4) Stop exactly when we reach it
            if (worldPX == targetPX && worldPY == targetPY)
            {
                movingToClick = false;
            }

            // 5) Hysteresis-based desired camera (integer)
            int scrX = worldPX - camX;
            int scrY = worldPY - camY;
            int desiredCamX = camX;
            int desiredCamY = camY;

            if (scrX < MARGIN_X - HYST)
                desiredCamX = worldPX - MARGIN_X;
            else if (scrX > (SCREEN_WIDTH - MARGIN_X - PLAYER_W) + HYST)
                desiredCamX = worldPX - (SCREEN_WIDTH - MARGIN_X - PLAYER_W);

            if (scrY < MARGIN_Y - HYST)
                desiredCamY = worldPY - MARGIN_Y;
            else if (scrY > (SCREEN_HEIGHT - MARGIN_Y - PLAYER_H) + HYST)
                desiredCamY = worldPY - (SCREEN_HEIGHT - MARGIN_Y - PLAYER_H);

            // 6) Smoothly lerp the *float* camera
            camXf = camlerp(camXf, (float)desiredCamX, 0.2f);
            camYf = camlerp(camYf, (float)desiredCamY, 0.2f);

            // 7) Round once for all rendering/logic
            camX = SDL_clamp((int)(camXf + 0.5f), 0, MAX_CAM_X);
            camY = SDL_clamp((int)(camYf + 0.5f), 0, MAX_CAM_Y);
        }

        // --- Mouse & Facing Direction ----------------------------------------------------------------------------
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        int centerX = worldPX - camX + PLAYER_W / 2;
        int centerY = worldPY - camY + PLAYER_H / 2;
        float angle = atan2f((float)(my - centerY), (float)(mx - centerX));
        float deg = angle * 180.0f / M_PI;
        if (deg < 0)
            deg += 360.0f;
        if (deg >= 315 || deg < 45)
            facing = RIGHT;
        else if (deg < 135)
            facing = DOWN;
        else if (deg < 225)
            facing = LEFT;
        else
            facing = UP;

        // --- Rendering -------------------------------------------------------------------------------------------
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        for (int i = 0; i < WORLD_CAPACITY; ++i)
        {
            QuadtreeNode *n = &worldNodes[i];
            SDL_Rect r = {n->key.x * TILE_SIZE - camX,
                          n->key.y * TILE_SIZE - camY,
                          TILE_SIZE, TILE_SIZE};
            SDL_SetRenderDrawColor(renderer,
                                   n->hasTree ? 34 : 105,
                                   n->hasTree ? 139 : 105,
                                   n->hasTree ? 34 : 105,
                                   255);
            SDL_RenderFillRect(renderer, &r);
        }
        SDL_Rect pd = {worldPX - camX, worldPY - camY, PLAYER_W, PLAYER_H};
        SDL_Texture *curTex = (facing == DOWN ? texDown : (facing == LEFT ? texLeft : (facing == RIGHT ? texRight : texUp)));
        if (curTex)
            SDL_RenderCopy(renderer, curTex, NULL, &pd);
        else
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &pd);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // cap ~60fps
    }

    // Cleanup ----------------------------------------------------------------------------------------------
    if (texDown)
        SDL_DestroyTexture(texDown);
    if (texLeft)
        SDL_DestroyTexture(texLeft);
    if (texRight)
        SDL_DestroyTexture(texRight);
    if (texUp)
        SDL_DestroyTexture(texUp);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
