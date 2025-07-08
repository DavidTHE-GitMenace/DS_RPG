#pragma once
#include <iostream>
#include <utility> // important to return two values at once with the use of std::pair: pair<type, type>
#include <SDL.h>
#include <SDL_image.h>

#include "GridGraph.hpp"
using namespace std;

struct Rock {
  Rock(pair<int, int> coords, pair<int, int> area, GridGraph& graph)
    : coordinates(coords), areaBounds(area)
  {
    // figure out which tiles this rock covers:
    int tx0 = worldX / TILE_SIZE;
    int ty0 = worldY / TILE_SIZE;
    int tx1 = (worldX + w) / TILE_SIZE;
    int ty1 = (worldY + h) / TILE_SIZE;
    graph.blockRegion(tx0, ty0, tx1, ty1);
  }
    pair<int, int> areaBounds; // the space it'll take up on the map with it's width and length
    pair<int, int> coordinates; // where it's at on the map
    string obJectType = "R";
    SDL_Surface* rock1 = IMG_Load("environmentAssets/rock.png");
    SDL_Rect rockFrame{500, 200, 150, 150}; 

    
};
