// === quadtree_world_bounded_hybrid.c ===
// C SDL2 demo: finite quadtree-backed map with hybrid camera/player movement and invisible walls

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Screen and tile dimensions
#define SCREEN_WIDTH 800  // viewport width in pixels
#define SCREEN_HEIGHT 600 // viewport height in pixels
#define TILE_SIZE 64      // size of each square tile in pixels

// World dimensions (number of tiles)
#define WORLD_COLS 30 // world is 30 tiles wide (1920 px)
#define WORLD_ROWS 20 // world is 20 tiles tall (1280 px)
#define WORLD_CAPACITY (WORLD_COLS * WORLD_ROWS)

// Camera movement margin and bounds
#define MARGIN 200 // px from viewport edge before camera moves
#define MAX_CAM_X ((WORLD_COLS * TILE_SIZE) - SCREEN_WIDTH)
#define MAX_CAM_Y ((WORLD_ROWS * TILE_SIZE) - SCREEN_HEIGHT)

// Data structures
// Vec2: 2D integer coordinate key for tiles
typedef struct
{
    int x, y;
} Vec2;

// QuadtreeNode: stores static tile info (tree or grass)
typedef struct
{
    Vec2 key;
    bool hasTree;
} QuadtreeNode;

// Static storage for all tiles in finite world
static QuadtreeNode worldNodes[WORLD_CAPACITY];

// Initialize world map (finite grid), e.g., static tree pattern
void initWorld()
{
    int idx = 0;
    for (int y = 0; y < WORLD_ROWS; ++y)
    {
        for (int x = 0; x < WORLD_COLS; ++x)
        {
            bool tree = ((x + y) % 3 == 0); // example pattern
            worldNodes[idx++] = (QuadtreeNode){.key = {x, y}, .hasTree = tree};
        }
    }
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("Bounded Quadtree World",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    initWorld(); // preload tiles

    // Player world and screen state
    int playerW = TILE_SIZE / 2, playerH = TILE_SIZE / 2;
    // Start player at world center
    int worldPlayerX = (WORLD_COLS * TILE_SIZE) / 2 - playerW / 2;
    int worldPlayerY = (WORLD_ROWS * TILE_SIZE) / 2 - playerH / 2;
    // Camera offset (top-left of viewport in world coords)
    int camX = 0, camY = 0;

    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        // Input
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                break;
            }
            if (e.type == SDL_KEYDOWN)
            {
                int dx = 0, dy = 0;
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    dy = -10;
                    break;
                case SDLK_DOWN:
                    dy = 10;
                    break;
                case SDLK_LEFT:
                    dx = -10;
                    break;
                case SDLK_RIGHT:
                    dx = 10;
                    break;
                }
                // Compute new world position
                int newWPX = worldPlayerX + dx;
                int newWPY = worldPlayerY + dy;
                // Invisible walls: clamp world position within [0, worldSize - playerSize]
                if (newWPX < 0)
                    newWPX = 0;
                if (newWPX > WORLD_COLS * TILE_SIZE - playerW)
                    newWPX = WORLD_COLS * TILE_SIZE - playerW;
                if (newWPY < 0)
                    newWPY = 0;
                if (newWPY > WORLD_ROWS * TILE_SIZE - playerH)
                    newWPY = WORLD_ROWS * TILE_SIZE - playerH;
                worldPlayerX = newWPX;
                worldPlayerY = newWPY;

                // Hybrid camera logic:
                // If player approaches margin and camera can move, pan camera
                int screenPX = worldPlayerX - camX;
                int screenPY = worldPlayerY - camY;
                // Left margin
                if (screenPX < MARGIN)
                {
                    camX = worldPlayerX - MARGIN;
                }
                // Right margin
                else if (screenPX > SCREEN_WIDTH - MARGIN - playerW)
                {
                    camX = worldPlayerX - (SCREEN_WIDTH - MARGIN - playerW);
                }
                // Top margin
                if (screenPY < MARGIN)
                {
                    camY = worldPlayerY - MARGIN;
                }
                // Bottom margin
                else if (screenPY > SCREEN_HEIGHT - MARGIN - playerH)
                {
                    camY = worldPlayerY - (SCREEN_HEIGHT - MARGIN - playerH);
                }
                // Clamp camera to world bounds to avoid black space
                if (camX < 0)
                    camX = 0;
                if (camX > MAX_CAM_X)
                    camX = MAX_CAM_X;
                if (camY < 0)
                    camY = 0;
                if (camY > MAX_CAM_Y)
                    camY = MAX_CAM_Y;
            }
        }

        // Render
        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);
        // Draw all tiles, offset by camera
        for (int i = 0; i < WORLD_CAPACITY; ++i)
        {
            QuadtreeNode *n = &worldNodes[i];
            SDL_Rect r = {n->key.x * TILE_SIZE - camX,
                          n->key.y * TILE_SIZE - camY,
                          TILE_SIZE, TILE_SIZE};
            SDL_SetRenderDrawColor(ren,
                                   n->hasTree ? 34 : 105,
                                   n->hasTree ? 139 : 105,
                                   n->hasTree ? 34 : 105,
                                   255);
            SDL_RenderFillRect(ren, &r);
        }
        // Draw player at screen coords
        SDL_Rect p = {worldPlayerX - camX,
                      worldPlayerY - camY,
                      playerW, playerH};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}