// weapon.h
#pragma once
#include <SDL2/SDL.h>
#ifdef __cplusplus
extern "C"
{
#endif

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
#ifdef __cplusplus
}
#endif
