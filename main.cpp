#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <chrono>
#include <cstddef>
#include <random>
#include <thread>
#include <vector>

#include "GridGraph.hpp"

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

    // I need this for holding down keys
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    // Using NodeIds here as well as an alias to make teminology stick
    using NodeId = int;

    // ------------------------------------------------------------------------------------------------------------------------------------

    // MAKING THE GRID GRAPH FOR ENEMIES AND OBECTS ---------------------------------------------------------------------------------------

    GridGraph worldGrid(1700, 1000);
    pair <int, int> playerCoordinates;
    pair <int, int> slimeCoordinates;


    // ------------------------------------------------------------------------------------------------------------------------------------

    // MAKING OBJECTS IN-GAME: ------------------------------------------------------------------------------------------------------------

    // cubes
    //            {xPos, yPos, length, width} 
     SDL_Rect rect{300, 300, 50, 50};
     SDL_Rect rect2{100, 100, 50, 50};

    // PLAYER -------------------------------------------------------------------------------------
    // - LOADING ALL THE SURFACES AND TEXTURES
    SDL_Surface* playerForward = IMG_Load("playerAssets/playerForward.png");
    SDL_Texture* playerForwardTex  = SDL_CreateTextureFromSurface(ren, playerForward);

    SDL_Surface* playerBackward = IMG_Load("playerAssets/playerBackwards.png");
    SDL_Texture* playerBackwardTex  = SDL_CreateTextureFromSurface(ren, playerBackward);

    SDL_Surface* playerLeft = IMG_Load("playerAssets/playerLeftIdle.png");
    SDL_Texture* playerLeftTex  = SDL_CreateTextureFromSurface(ren, playerLeft);
    
    SDL_Surface* playerRight = IMG_Load("playerAssets/playerRightIdle.png");
    SDL_Texture* playerRightTex  = SDL_CreateTextureFromSurface(ren, playerRight);
    
    SDL_Surface* playerWalkingForward1 = IMG_Load("playerAssets/playerWalkingForward1.png");
    SDL_Texture* playerWalkingForward1Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingForward1);
    
    SDL_Surface* playerWalkingForward2 = IMG_Load("playerAssets/playerWalkingForward2.png");
    SDL_Texture* playerWalkingForward2Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingForward2);

    SDL_Surface* playerWalkingBackward1 = IMG_Load("playerAssets/walkingBackwards1.png");
    SDL_Texture* playerWalkingBackward1Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingBackward1);

    SDL_Surface* playerWalkingBackward2 = IMG_Load("playerAssets/walkingBackwards2.png");
    SDL_Texture* playerWalkingBackward2Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingBackward2);
    
    SDL_Surface* playerWalkingLeft1 = IMG_Load("playerAssets/playerWalkingLeft1.png");
    SDL_Texture* playerWalkingLeft1Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingLeft1);
    
    SDL_Surface* playerWalkingLeft2 = IMG_Load("playerAssets/playerWalkingLeft2.png");
    SDL_Texture* playerWalkingLeft2Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingLeft2);

    SDL_Surface* playerWalkingRight1 = IMG_Load("playerAssets/playerWalkingRight1.png");
    SDL_Texture* playerWalkingRight1Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingRight1);

    SDL_Surface* playerWalkingRight2 = IMG_Load("playerAssets/playerWalkingRight2.png");
    SDL_Texture* playerWalkingRight2Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingRight2);

    SDL_Surface* playerWalkingBackRight1 = IMG_Load("playerAssets/playerWalkingBackRight1.png");
    SDL_Texture* playerWalkingBackRight1Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingBackRight1);

    SDL_Surface* playerWalkingBackRight2 = IMG_Load("playerAssets/playerWalkingBackRight2.png");
    SDL_Texture* playerWalkingBackRight2Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingBackRight2);

    SDL_Surface* playerWalkingBackLeft1 = IMG_Load("playerAssets/playerWalkingBackLeft1.png");
    SDL_Texture* playerWalkingBackLeft1Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingBackLeft1);

    SDL_Surface* playerWalkingBackLeft2 = IMG_Load("playerAssets/playerWalkingBackLeft2.png");
    SDL_Texture* playerWalkingBackLeft2Tex  = SDL_CreateTextureFromSurface(ren, playerWalkingBackLeft2);

    SDL_Surface* playerBackLeft = IMG_Load("playerAssets/playerBackLeft.png");
    SDL_Texture* playerBackLeftTex  = SDL_CreateTextureFromSurface(ren, playerBackLeft);

    SDL_Surface* playerBackRight = IMG_Load("playerAssets/playerBackRight.png");
    SDL_Texture* playerBackRightTex  = SDL_CreateTextureFromSurface(ren, playerBackRight);
    

    SDL_Surface* currentPlayer = playerForward;  // default facing forward
    SDL_Texture* currentPlayerTex  = playerForwardTex;
    
    SDL_Rect playerDst{500, 300, 70, 70}; // where to draw and how big


    // FOR FINDING IF PLAYER IS FACING FORWARD, BACKWARD, LEFT OR RIGHT
    bool isplayerRight = false;
    bool isplayerLeft = false;
    bool isplayerForward = false;
    bool isplayerBackward = false;

    // FOR SWITCHING THE FRAMES OF THE MAIN CHARACTER WALKING
    bool isplayerWalkingFrame1 = false;




    // SLIME --------------------------------------------------------------------------------------
    // - LOADING ALL THE SURFACES AND TEXTURES
    SDL_Surface* slimeIdle1 = IMG_Load("slimeAssets/slimeForward1.png");
    SDL_Texture* slimeIdle1Tex  = SDL_CreateTextureFromSurface(ren, slimeIdle1);

    SDL_Surface* slimeIdle2 = IMG_Load("slimeAssets/slimeForward2.png");
    SDL_Texture* slimeIdle2Tex  = SDL_CreateTextureFromSurface(ren, slimeIdle2);

    SDL_Surface* slimeIdle3 = IMG_Load("slimeAssets/slimeForward3.png");
    SDL_Texture* slimeIdle3Tex  = SDL_CreateTextureFromSurface(ren, slimeIdle3);

    SDL_Surface* slimeForward1 = IMG_Load("slimeAssets/slimeDown1.png");
    SDL_Texture* slimeForward1Tex  = SDL_CreateTextureFromSurface(ren, slimeForward1);

    SDL_Surface* slimeForward2 = IMG_Load("slimeAssets/slimeDown2.png");
    SDL_Texture* slimeForward2Tex  = SDL_CreateTextureFromSurface(ren, slimeForward2);

    SDL_Surface* slimeForward3 = IMG_Load("slimeAssets/slimeDown3.png");
    SDL_Texture* slimeForward3Tex  = SDL_CreateTextureFromSurface(ren, slimeForward3);

    SDL_Surface* slimeLeft1 = IMG_Load("slimeAssets/slimeLeft1.png");
    SDL_Texture* slimeLeft1Tex  = SDL_CreateTextureFromSurface(ren, slimeLeft1);

    SDL_Surface* slimeLeft2 = IMG_Load("slimeAssets/slimeLeft2.png");
    SDL_Texture* slimeLeft2Tex  = SDL_CreateTextureFromSurface(ren, slimeLeft2);

    SDL_Surface* slimeLeft3 = IMG_Load("slimeAssets/slimeLeft3.png");
    SDL_Texture* slimeLeft3Tex  = SDL_CreateTextureFromSurface(ren, slimeLeft3);

    SDL_Surface* slimeRight1 = IMG_Load("slimeAssets/slimeRight1.png");
    SDL_Texture* slimeRight1Tex  = SDL_CreateTextureFromSurface(ren, slimeRight1);

    SDL_Surface* slimeRight2 = IMG_Load("slimeAssets/slimeRight2.png");
    SDL_Texture* slimeRight2Tex  = SDL_CreateTextureFromSurface(ren, slimeRight2);

    SDL_Surface* slimeRight3 = IMG_Load("slimeAssets/slimeRight3.png");
    SDL_Texture* slimeRight3Tex  = SDL_CreateTextureFromSurface(ren, slimeRight3);

    SDL_Surface* slimeUp1 = IMG_Load("slimeAssets/slimeUp1.png");
    SDL_Texture* slimeUp1Tex  = SDL_CreateTextureFromSurface(ren, slimeUp1);

    SDL_Surface* slimeUp2 = IMG_Load("slimeAssets/slimeUp2.png");
    SDL_Texture* slimeUp2Tex  = SDL_CreateTextureFromSurface(ren, slimeUp2);

    SDL_Surface* slimeUp3 = IMG_Load("slimeAssets/slimeUp3.png");
    SDL_Texture* slimeUp3Tex  = SDL_CreateTextureFromSurface(ren, slimeUp3);

    SDL_Surface* currentSlime = slimeIdle1;  // default facing forward
    SDL_Texture* currentSlimeTex  = slimeIdle1Tex;
    
    SDL_Rect slimeDst{1000, 500, 100, 100}; // where to draw and how big

    // FOR FINDING IF SLIME IS FACING FORWARD, BACKWARD, LEFT OR RIGHT
    bool isSlimeRight = false;
    bool isSlimeLeft = false;
    bool isSlimeForward = false;
    bool isSlimeBackward = false;
    bool isSlimeUpRight = false;
    bool isSlimeUpLeft = false;
    bool isSlimeDownRight = false;
    bool isSlimeDownLeft = false;
    bool isSlimeIdle = true;
    

    // FOR SWITCHING THE FRAMES OF THE SLIME MOVING
    int slimeFrameCount = 0;

    // LISTS FOR BACK AND FORTH ANIMATION OF SLIME:
    // forward list
    vector<SDL_Surface*> slimeIdleList = {slimeIdle1, slimeIdle2, slimeIdle3, slimeIdle2};
    vector<SDL_Texture*> slimeIdleTextureList = {slimeIdle1Tex, slimeIdle2Tex, slimeIdle3Tex, slimeIdle2Tex};

    // forward list
    vector<SDL_Surface*> slimeForwardList = {slimeForward1, slimeForward2, slimeForward3, slimeForward2};
    vector<SDL_Texture*> slimeForwardTextureList = {slimeForward1Tex, slimeForward2Tex, slimeForward3Tex, slimeForward2Tex};

    // back list
    vector<SDL_Surface*> slimeBackList = {slimeUp1, slimeUp2, slimeUp3, slimeUp2};
    vector<SDL_Texture*> slimeBackTextureList = {slimeUp1Tex, slimeUp2Tex, slimeUp3Tex, slimeUp2Tex};

    // left list
    vector<SDL_Surface*> slimeLeftList = {slimeLeft1, slimeLeft2, slimeLeft3, slimeLeft2};
    vector<SDL_Texture*> slimeLeftTextureList = {slimeLeft1Tex, slimeLeft2Tex, slimeLeft3Tex, slimeLeft2Tex};

    // right list
    vector<SDL_Surface*> slimeRightList = {slimeRight1, slimeRight2, slimeRight3, slimeRight2};
    vector<SDL_Texture*> slimeRightTextureList = {slimeRight1Tex, slimeRight2Tex, slimeRight3Tex, slimeRight2Tex};

    

    // --------------------------------------------------------------------------------------------


    // ------------------------------------------------------------------------------------------------------------------------------------


    // CLOCKS FOR THE ANIMATION:
    using Clock = std::chrono::high_resolution_clock;  
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<float>; // float seconds

    TimePoint lastTime = Clock::now();
    float accumulatedTime = 0.0f;
    float slimeAccumulatedTime = 0.0f;
    
    const float playerAnimateDuration = 0.1f;
    const float slimeAnimateDuration = 0.125f;

    // ------------------------------------------------------------------------------------------------------------------------------------
   



    // WINDOW OPENS, WHILE LOOP STARTS: ---------------------------------------------------------------------------------------------------
    while (!quit) // WHILE THE USEE HASN'T EXITED THE WINDOW
    {
        // TIME
        this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate work

        // CLOCK TIMING FOR PLAYER ANIMATION: -----------------------------------------------------

        auto now = Clock::now();
        Duration delta = now - lastTime; // duration since last frame
        lastTime = now;
        accumulatedTime += delta.count(); // delta.count() gives seconds as float
        slimeAccumulatedTime += delta.count(); 

        // ----------------------------------------------------------------------------------------
    
    // EVENT LOOPS FOR INPUT ------------------------------------------------------------------------------------------
        while (SDL_PollEvent(&e)) // THIS IS FOR EVENTS LIKE INPUTS ON THE WINDOW
        {
            if (e.type == SDL_QUIT) // IF USER CLICKS THE "X" ON THE WINDOW
            {
                quit = 1;
            }
            else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        currentPlayer = playerBackward;
                        currentPlayerTex = playerBackwardTex;
                        isplayerBackward = false;
                        break;
                    case SDLK_DOWN:
                        currentPlayer = playerForward;
                        currentPlayerTex = playerForwardTex;
                        isplayerForward = false;
                        break;
                    case SDLK_LEFT:
                        currentPlayer = playerLeft;
                        currentPlayerTex = playerLeftTex;
                        isplayerLeft = false;
                        break;
                    case SDLK_RIGHT:
                        currentPlayer = playerRight;
                        currentPlayerTex = playerRightTex;
                        isplayerRight = false;
                        break;
                }
                playerForward = playerBackward = playerForward = playerBackward = false;
            }
        }

        if (keystate[SDL_SCANCODE_DOWN] && keystate[SDL_SCANCODE_LEFT]) {
            playerDst.y += 15;
            playerDst.x -= 15;
            currentPlayer = playerLeft;
            currentPlayerTex = playerLeftTex;
            isplayerLeft = true;
            isplayerBackward = isplayerForward = isplayerRight = false;
        }
        else if (keystate[SDL_SCANCODE_DOWN] && keystate[SDL_SCANCODE_RIGHT]) {
            playerDst.y += 15;
            playerDst.x += 15;
            currentPlayer = playerRight;
            currentPlayerTex = playerRightTex;
            isplayerRight = true;
            isplayerBackward = isplayerForward = isplayerLeft = false;
        }
        else if (keystate[SDL_SCANCODE_UP] && keystate[SDL_SCANCODE_RIGHT]) {
            playerDst.y -= 15;
            playerDst.x += 15;
            currentPlayer = playerBackRight;
            currentPlayerTex = playerBackRightTex;
            isplayerRight = isplayerBackward = true;
            isplayerForward = isplayerLeft = false;
        }        
        else if (keystate[SDL_SCANCODE_UP] && keystate[SDL_SCANCODE_LEFT]) {
            playerDst.y -= 15;
            playerDst.x -= 15;
            currentPlayer = playerBackLeft;
            currentPlayerTex = playerBackLeftTex;
            isplayerLeft = isplayerBackward = true;
            isplayerForward = isplayerRight = false;
        }                
        else if (keystate[SDL_SCANCODE_UP]) {
            playerDst.y -= 15;
            currentPlayer = playerBackward;
            currentPlayerTex = playerBackwardTex;
            isplayerBackward = true;
            isplayerRight = isplayerForward = isplayerLeft = false;
        }
        else if (keystate[SDL_SCANCODE_DOWN]) {
            playerDst.y += 15;
            currentPlayer = playerForward;
            currentPlayerTex = playerForwardTex;
            isplayerForward = true;
            isplayerBackward = isplayerRight = isplayerLeft = false;
        }
        else if (keystate[SDL_SCANCODE_LEFT]) {
            playerDst.x -= 15;
            currentPlayer = playerLeft;
            currentPlayerTex = playerLeftTex;
            isplayerLeft = true;
            isplayerBackward = isplayerForward = isplayerRight = false;
        }
        else if (keystate[SDL_SCANCODE_RIGHT]) {
            playerDst.x += 15;
            currentPlayer = playerRight;
            currentPlayerTex = playerRightTex;
            isplayerRight = true;
            isplayerBackward = isplayerForward = isplayerLeft = false;
        }

        // player coordinates need to be updated after movement
        playerCoordinates.first = playerDst.x;
        playerCoordinates.second = playerDst.y;
        
    // ----------------------------------------------------------------------------------------------------------------

    
    // ACTUAL PLAYER ANIMATION: -------------------------------------------------------------------

    if (isplayerBackward && isplayerLeft) {
        isplayerForward = isplayerRight = false; // only facing backwards

        currentPlayer = isplayerWalkingFrame1 ? playerWalkingBackLeft1 : playerWalkingBackLeft2;
        currentPlayerTex = isplayerWalkingFrame1 ? playerWalkingBackLeft1Tex : playerWalkingBackLeft2Tex;

        if (accumulatedTime >= playerAnimateDuration) { // ANIMATION RESETING BACK AND FORTH
                isplayerWalkingFrame1 = !isplayerWalkingFrame1;
                accumulatedTime = 0.0f; // Resets the timer
        }
    }
    else if (isplayerBackward && isplayerRight) {
        isplayerForward = isplayerLeft = false; // only facing backwards

        currentPlayer = isplayerWalkingFrame1 ? playerWalkingBackRight1 : playerWalkingBackRight2;
        currentPlayerTex = isplayerWalkingFrame1 ? playerWalkingBackRight1Tex : playerWalkingBackRight2Tex;

        if (accumulatedTime >= playerAnimateDuration) { // ANIMATION RESETING BACK AND FORTH
                isplayerWalkingFrame1 = !isplayerWalkingFrame1;
                accumulatedTime = 0.0f; // Resets the timer
        }
    }
    else if (isplayerBackward) {
        isplayerForward = isplayerLeft = isplayerRight = false; // only facing backwards

        currentPlayer = isplayerWalkingFrame1 ? playerWalkingBackward1 : playerWalkingBackward2;
        currentPlayerTex = isplayerWalkingFrame1 ? playerWalkingBackward1Tex : playerWalkingBackward2Tex;

        if (accumulatedTime >= playerAnimateDuration) { // ANIMATION RESETING BACK AND FORTH
                isplayerWalkingFrame1 = !isplayerWalkingFrame1;
                accumulatedTime = 0.0f; // Resets the timer
        }

    }
    else if (isplayerForward) {
        isplayerBackward = isplayerLeft = isplayerRight = false; // only facing backwards

        currentPlayer = isplayerWalkingFrame1 ? playerWalkingForward1 : playerWalkingForward2;
        currentPlayerTex = isplayerWalkingFrame1 ? playerWalkingForward1Tex : playerWalkingForward2Tex;

        if (accumulatedTime >= playerAnimateDuration) { // ANIMATION RESETING BACK AND FORTH
                isplayerWalkingFrame1 = !isplayerWalkingFrame1;
                accumulatedTime = 0.0f; // Resets the timer
        }

    }
    else if (isplayerLeft) {
        isplayerBackward = isplayerForward = isplayerRight = false; // only facing backwards

        currentPlayer = isplayerWalkingFrame1 ? playerWalkingLeft1 : playerWalkingLeft2;
        currentPlayerTex = isplayerWalkingFrame1 ? playerWalkingLeft1Tex : playerWalkingLeft2Tex;

        if (accumulatedTime >= playerAnimateDuration) { // ANIMATION RESETING BACK AND FORTH
                isplayerWalkingFrame1 = !isplayerWalkingFrame1;
                accumulatedTime = 0.0f; // Resets the timer
        }

    }
    else if (isplayerRight) {
        isplayerBackward = isplayerForward = isplayerLeft = false; // only facing backwards

        currentPlayer = isplayerWalkingFrame1 ? playerWalkingRight1 : playerWalkingRight2;
        currentPlayerTex = isplayerWalkingFrame1 ? playerWalkingRight1Tex : playerWalkingRight2Tex;

        if (accumulatedTime >= playerAnimateDuration) { // ANIMATION RESETING BACK AND FORTH
                isplayerWalkingFrame1 = !isplayerWalkingFrame1;
                accumulatedTime = 0.0f; // Resets the timer
        }

    }

    // --------------------------------------------------------------------------------------------

    // SLIME MOVEMENT -----------------------------------------------------------------------------

    // slime coordinates
    slimeCoordinates.first = slimeDst.x;
    slimeCoordinates.second = slimeDst.y;

    // slime chasing player
    worldGrid.chasePlayer(slimeCoordinates, playerCoordinates, slimeDst, isSlimeIdle, isSlimeBackward, isSlimeForward, isSlimeLeft, isSlimeRight, isSlimeUpLeft, isSlimeUpRight, isSlimeDownLeft, isSlimeDownRight);


    // SLIME ANIMATION:

    if (isSlimeBackward) {
        isSlimeIdle = isSlimeForward = isSlimeLeft = isSlimeRight = false; // only facing backwards

        currentSlime = slimeBackList[slimeFrameCount];
        currentSlimeTex = slimeBackTextureList[slimeFrameCount];
    }
    else if (isSlimeForward) {
        isSlimeIdle = isSlimeBackward = isSlimeLeft = isSlimeRight = false; // only facing backwards

        currentSlime = slimeForwardList[slimeFrameCount];
        currentSlimeTex = slimeForwardTextureList[slimeFrameCount];
    }
    else if (isSlimeLeft) {
        isSlimeIdle = isSlimeForward = isSlimeBackward = isSlimeRight = false; // only facing backwards

        currentSlime = slimeLeftList[slimeFrameCount];
        currentSlimeTex = slimeLeftTextureList[slimeFrameCount];
    }
    else if (isSlimeRight) {
        isSlimeIdle = isSlimeForward = isSlimeBackward = isSlimeLeft = false; // only facing backwards

        currentSlime = slimeRightList[slimeFrameCount];
        currentSlimeTex = slimeRightTextureList[slimeFrameCount];
    }
    else if (isSlimeIdle) {
        isSlimeForward = isSlimeBackward = isSlimeLeft = isSlimeRight = false; // only facing backwards

        currentSlime = slimeIdleList[slimeFrameCount];
        currentSlimeTex = slimeIdleTextureList[slimeFrameCount];

    }

    if (slimeAccumulatedTime >= slimeAnimateDuration) { // ANIMATION RESETING BACK AND FORTH
        ++slimeFrameCount;
        if (slimeFrameCount == 4)  slimeFrameCount = 0;

        slimeAccumulatedTime = 0.0f; // Resets the timer
    }

    // --------------------------------------------------------------------------------------------

    
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
        SDL_RenderCopy(ren, currentPlayerTex, nullptr, &playerDst);

        // THIS IS THE SLIME
        SDL_RenderCopy(ren, currentSlimeTex, nullptr, &slimeDst);

        // RENDERS EVERTYING UNDER "REN"
        SDL_RenderPresent(ren);
        SDL_Delay(16);

    // --------------------------------------------------------------------------------------------
        
    }


    // Clean up
    SDL_FreeSurface(currentPlayer);
    SDL_DestroyTexture(currentPlayerTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}