// weapon.h
#pragma once
#include <SDL_image.h>
#include <string.h>

// (structs go here)

// Weapon structure — raw data
typedef struct
{
    const char *name;
    int damage;
    SDL_Texture *sprite;
} Weapon;

// Inventory entry (optional but related)
typedef struct
{
    Weapon *weapon;
    int quantity;
    SDL_Rect iconRect; // Used for UI display (optional)
} InventoryEntry;
