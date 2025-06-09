#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <chrono>
#include <cstddef>
#include <random>
#include <thread>

using namespace std;

int main() {
    // Initialize SDL and Window/Image Settings -------------------------------------------------------------------------------------------

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow(
        "SDL2 Window",                  // Window title
        SDL_WINDOWPOS_CENTERED,         // X position
        SDL_WINDOWPOS_CENTERED,         // Y position
        1400,                           // Width
        800,                            // Height
        SDL_WINDOW_SHOWN                // Flags
    );

    if (window == NULL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Basic event loop
    SDL_Event e;
    int quit = 0;
    SDL_Renderer* ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // ------------------------------------------------------------------------------------------------------------------------------------


    // MAKING OBJECTS IN-GAME: ------------------------------------------------------------------------------------------------------------

    // cubes
    //            {xPos, yPos, length, width} 
     SDL_Rect rect{300, 300, 50, 50};
     SDL_Rect rect2{100, 100, 50, 50};

    // PLAYER
    SDL_Surface* player = IMG_Load("playerAssets/playerForward.png");
    if (!player) {
        cerr << "player image loading Error: " << IMG_GetError() << "\n";
        // … clean up and exit …
    }
    SDL_Texture* playerTex  = SDL_CreateTextureFromSurface(ren, player);
    SDL_FreeSurface(player);
    SDL_Rect dst{500, 300, 70, 70}; // where to draw and how big

    bool playerRight = false;
    bool playerLeft = false;
    bool playerForward = true;
    bool playerBackward = false;


    // ------------------------------------------------------------------------------------------------------------------------------------


    // CLOCKS FOR THE ANIMATION:
    using Clock = std::chrono::high_resolution_clock;  
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<float>; // float seconds

    TimePoint lastTime = Clock::now();
    float accumulatedTime = 0.0f;
    const float playerAnimateDuration = 0.1f;

    // ------------------------------------------------------------------------------------------------------------------------------------
   



    // WINDOW OPENS, WHILE LOOP STARTS: ---------------------------------------------------------------------------------------------------
    while (!quit) // WHILE THE USEE HASN'T EXITED THE WINDOW
    {
        // TIME
        this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate work

        // ANIMATION FOR PLAYER: ------------------------------------------------------------------
        auto now = Clock::now();
        Duration delta = now - lastTime; // duration since last frame
        lastTime = now;
        accumulatedTime += delta.count(); // delta.count() gives seconds as float




        // ----------------------------------------------------------------------------------------
    
    // EVENT LOOPS FOR INPUT ------------------------------------------------------------------------------------------
        while (SDL_PollEvent(&e)) // THIS IS FOR EVENTS LIKE INPUTS ON THE WINDOW
        {
            if (e.type == SDL_QUIT) // IF USER CLICKS THE "X" ON THE WINDOW
            {
                quit = 1;
            }
            else if (e.type == SDL_KEYDOWN) { 
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        dst.y -= 5;
                        break;
                    case SDLK_DOWN:
                        dst.y += 5;
                        break;
                    case SDLK_LEFT:
                        dst.x -= 5;
                        break;
                    case SDLK_RIGHT:
                        dst.x += 5;
                        break;
                }
            }
        }

    // ----------------------------------------------------------------------------------------------------------------


    // RENDERING THE OBJECTS: ---------------------------------------------------------------------

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
        // SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); // red
        // SDL_RenderFillRect(ren, &rect);

        // THIS IS THE BLUE CUBE
        // SDL_SetRenderDrawColor(ren, 0, 0, 255, 255); // blue
        // SDL_RenderFillRect(ren, &rect2);

        // THIS IS THE PLAYER
        SDL_RenderCopy(ren, playerTex, nullptr, &dst);

        // RENDERS EVERTYING UNDER "REN"
        SDL_RenderPresent(ren);

    // --------------------------------------------------------------------------------------------
        
    }


    // Clean up
    SDL_DestroyTexture(playerTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

