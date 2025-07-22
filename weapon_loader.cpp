#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <SDL2/SDL_image.h>
#include "weapon.h"
#include "hash_table.h"

// Loads a Weapon from a JSON file and returns a pointer to a dynamically allocated Weapon struct
Weapon *loadWeaponFromJson(const char *filename, SDL_Renderer *renderer)
{
    // Open the JSON file
    std::ifstream file(filename);
    if (!file.is_open())
    {
        SDL_Log("Failed to open file: %s", filename);
        return NULL;
    }

    // Parse the JSON file into a nlohmann::json object
    nlohmann::json j;
    file >> j;

    // Allocate memory for a new Weapon struct (so it persists outside this function)
    Weapon *w = (Weapon *)malloc(sizeof(Weapon));

    // Copy data from JSON into the Weapon struct
    // strdup makes a copy of the name string from the JSON so it lives in memory even after the json object is destroyed
    w->name = strdup(j["name"].get<std::string>().c_str());
    w->damage = j["damage"];

    // Load the sprite texture from the file path provided in the JSON
    std::string spritePath = j["sprite"];
    w->sprite = IMG_LoadTexture(renderer, spritePath.c_str());
    if (!w->sprite)
    {
        SDL_Log("Failed to load texture: %s", IMG_GetError());
    }

    return w; // Return the pointer to the Weapon struct
}
