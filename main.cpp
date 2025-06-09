// === quadtree_world_bounded_hybrid.cpp ===
// C++ SDL2 demo: finite quadtree-backed map with hybrid camera/player movement and invisible walls

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <chrono>
#include <cstddef>
#include <random>

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
typedef struct
{
    int x, y;
} Vec2;

typedef struct
{
    Vec2 key;
    bool hasTree;
} QuadtreeNode;

static QuadtreeNode worldNodes[WORLD_CAPACITY];

void initWorld()
{
    int idx = 0;
    for (int y = 0; y < WORLD_ROWS; ++y)
    {
        for (int x = 0; x < WORLD_COLS; ++x)
        {
            QuadtreeNode node;
            node.key = {x, y};
            node.hasTree = ((x + y) % 3 == 0);
            worldNodes[idx++] = node;
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

    initWorld();

    int playerW = TILE_SIZE / 2, playerH = TILE_SIZE / 2;
    int worldPlayerX = (WORLD_COLS * TILE_SIZE) / 2 - playerW / 2;
    int worldPlayerY = (WORLD_ROWS * TILE_SIZE) / 2 - playerH / 2;
    int camX = 0, camY = 0;

    bool quit = false;
    SDL_Event e;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Surface *player = IMG_Load("playerAssets/playerIdle.png");
    if (!player)
    {
        std::cerr << "player image loading Error: " << IMG_GetError() << "\n";
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture *playerTex = SDL_CreateTextureFromSurface(ren, player);
    SDL_FreeSurface(player);
    SDL_Rect dst{500, 300, 70, 70};

    while (!quit)
    {
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

                int newWPX = worldPlayerX + dx;
                int newWPY = worldPlayerY + dy;

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

                int screenPX = worldPlayerX - camX;
                int screenPY = worldPlayerY - camY;

                if (screenPX < MARGIN)
                    camX = worldPlayerX - MARGIN;
                else if (screenPX > SCREEN_WIDTH - MARGIN - playerW)
                    camX = worldPlayerX - (SCREEN_WIDTH - MARGIN - playerW);

                if (screenPY < MARGIN)
                    camY = worldPlayerY - MARGIN;
                else if (screenPY > SCREEN_HEIGHT - MARGIN - playerH)
                    camY = worldPlayerY - (SCREEN_HEIGHT - MARGIN - playerH);

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

        SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
        SDL_RenderClear(ren);

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

        SDL_Rect p = {worldPlayerX - camX, worldPlayerY - camY, playerW, playerH};
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &p);
        SDL_RenderCopy(ren, playerTex, nullptr, &dst);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(playerTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
