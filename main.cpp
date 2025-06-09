#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
using namespace std;

int main()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow(
        "SDL2 Window",          // Window title
        SDL_WINDOWPOS_CENTERED, // X position
        SDL_WINDOWPOS_CENTERED, // Y position
        1400,                   // Width
        800,                    // Height
        SDL_WINDOW_SHOWN        // Flags
    );

    if (window == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // MAKING OBJECTS IN-GAME:

    //            {xPos, yPos, length, width}
    SDL_Rect rect{300, 300, 50, 50};
    SDL_Rect rect2{100, 100, 50, 50};

    // -----------------------

    // Basic event loop
    SDL_Event e;
    int quit = 0;
    SDL_Renderer *ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    while (!quit) // WHILE THE USEE HASN'T EXITED THE WINDOW
    {
        while (SDL_PollEvent(&e)) // THIS IS FOR EVENTS LIKE INPUTS ON THE WINDOW
        {
            if (e.type == SDL_QUIT) // IF USER CLICKS THE "X" ON THE WINDOW
            {
                quit = 1;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    rect.y -= 10;
                    break;
                case SDLK_DOWN:
                    rect.y += 10;
                    break;
                case SDLK_LEFT:
                    rect.x -= 10;
                    break;
                case SDLK_RIGHT:
                    rect.x += 10;
                    break;
                }
            }

            // 2) Clear the screen to a background color
            SDL_SetRenderDrawColor(ren, 30, 30, 30, 255); // BACKGROUND: dark gray
            SDL_RenderClear(ren);

            /* HOW TO MODIFY THE VALUES FOR RECT OBJECTS:
                rect.x = ... // initial x position
                rect.y = ...; // initial y position
                rect.w = ...;  // width
                rect.h = ...;  // height
            */

            // THIS IS THE RED CUBE
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); // red
            SDL_RenderFillRect(ren, &rect);

            // THIS IS THE BLUE CUBE
            SDL_SetRenderDrawColor(ren, 0, 0, 255, 255); // blue
            SDL_RenderFillRect(ren, &rect2);

            // RENDERS EVERTYING UNDER "REN"
            SDL_RenderPresent(ren);
        }
    }

    // Clean up
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
