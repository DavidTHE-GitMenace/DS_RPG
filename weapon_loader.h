// weapon_loader.h
#pragma once
#include "weapon.h"

#ifdef __cplusplus
extern "C"
{
#endif

    Weapon *loadWeaponFromJson(const char *filename, SDL_Renderer *renderer);

#ifdef __cplusplus
}
#endif
