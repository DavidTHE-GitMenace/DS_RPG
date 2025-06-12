#pragma once 
/*
    ^
    |
   Makes sure that the code content in files doesn't add twice.
   So if Graph.hpp were to be #included twice, #pragma once
   would make sure when it compiles and adds the contents
   of Graph.hpp and maybe a file that also has 
   '#include Graph.hpp,' it won't add the same content 
   of Graph.hpp twice.

   #pragma once is a better version of:

   #ifndef ...
   #define ...
   ...
   ...
   #endif

*/ 

#include <vector>
#include <utility> // important to return two values at once with the use of std::pair: pair<type, type>
#include <SDL.h>
#include <SDL_image.h>
using namespace std;

class GridGraph { // the class definition starts here
public:
    using NodeId = int; // we are using the name "NodeId" as another name for int

    // constructor
    GridGraph(int rows, int cols) : _rows(rows), _cols(cols), 
    _adj(rows * cols) // assigning the vector to how many vectors of NodeIds it'll have
    {
        for (int r = 0; r < _rows; ++r) {
            for (int c = 0; c < _cols; ++c) {
                NodeId id = nodeId(r, c); // to go to each NodeId's neighbor vector list in _adj

                // to find and add the number above it if there is a row that exists above the current row
                if (r > 0)                  _adj[id].push_back(nodeId(r - 1, c)); 
                /*
                                                            ^
                                                            |
                                                            |
                              it'll push NodeId's neighbor number to the current NodeId 
                              vector where it stores the current number's neighboring numbers
                */                          

                // to find and add the number below it if there is a row that exists below the current row
                if (r < _rows - 1)          _adj[id].push_back(nodeId(r + 1, c));

                // to find and add the number on its left if there is a column that exists to the left of the current column
                if (c > 0)                  _adj[id].push_back(nodeId(r, c - 1)); 

                // to find and add the number on its right if there is a column that exists to the right of the current column
                if (c < _cols - 1)          _adj[id].push_back(nodeId(r, c + 1)); 
            }
        }

    }

    NodeId nodeId(int r, int c) const { 
        /*  Each NodeId has its own number within the Grid, starting from 0 to (_rows * _cols) - 1
            This function just generates that number for each NodeId starting from (0,0) - top left to (_rows,_cols) - bottom right
            This function can also be used to find the nodeId from coordinates
        */ 
        return r * _cols + c;
    }

    pair<int, int> idToCoords(NodeId id) const{ // returns the coords of the NodeId
        return {id / _cols, id % _cols};
    }

    int idToCoordsX(NodeId id) const{ // returns the x coordinate of the NodeId
        return id / _cols;
    }

    int idToCoordsY(NodeId id) const{ // returns the y coordinate of the NodeId
        return id % _cols;
    }  

    const vector<NodeId>& neighbors(NodeId id) const { // get the neighboring numbers of NodeId
        return _adj[id];
    }

    void chasePlayer(
        const std::pair<int, int>& enemyCoords,
        const std::pair<int, int>& playerCoords,
        SDL_Rect& object,
        bool& idle, bool& up, bool& down, bool& left, bool& right,
        bool& upLeft, bool& upRight, bool& downLeft, bool& downRight)
    {
        // Reset flags
        up = down = left = right = upLeft = upRight = downLeft = downRight = false;

        // Convert the enemy and player coordinates to a NodeID (for future pathfinding if needed)
        // NodeId enemyPoint = nodeId(enemyCoords.first, enemyCoords.second);
        // NodeId playerPoint = nodeId(playerCoords.first, playerCoords.second);

        if (enemyCoords.second > playerCoords.second && enemyCoords.first < playerCoords.first + 30 && enemyCoords.first > playerCoords.first - 30) {
            // player is directly above
            object.y -= 2;
            up = true;
        }
        else if (enemyCoords.second < playerCoords.second && enemyCoords.first < playerCoords.first + 30 && enemyCoords.first > playerCoords.first - 30) {
            // player is directly below
            object.y += 2;
            down = true;
        }
        else if (enemyCoords.first > playerCoords.first && enemyCoords.second < playerCoords.second + 30 && enemyCoords.second > playerCoords.second - 30) {
            // player is to the left
            object.x -= 2;
            left = true;
        }
        else if (enemyCoords.first < playerCoords.first && enemyCoords.second < playerCoords.second + 30 && enemyCoords.second > playerCoords.second - 30) {
            // player is to the right
            object.x += 2;
            right = true;
        }
        else if (enemyCoords.first < playerCoords.first && enemyCoords.second < playerCoords.second) {
            // player is below and to the right
            object.x += 2;
            object.y += 2;
            right = true;
        }
        else if (enemyCoords.second < playerCoords.second && enemyCoords.first > playerCoords.first) {
            // player is below and to the left
            object.y += 2;
            object.x -= 2;
            left = true;
        }
        else if (enemyCoords.second > playerCoords.second && enemyCoords.first < playerCoords.first) {
            // player is above and to the right
            object.y -= 2;
            object.x += 2;
            right = true;
        }
        else if (enemyCoords.second > playerCoords.second && enemyCoords.first > playerCoords.first) {
            // player is above and to the left
            object.y -= 2;
            object.x -= 2;
            left = true;
        }
        else {
            idle = true;
        }
    }
    
    // I realized

    int rows() {return _rows;}
    int columns() {return _cols;}


private:
    int _rows;
    int _cols;
    vector<vector<NodeId>> _adj;
};
