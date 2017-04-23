#pragma once

#include "SDL2/SDL.h"

typedef struct
{
    char* fileName;
    int w, h;
} STexture;


/* textures of level */
SDL_Texture* levelTextures[10];

/* textures of player */
SDL_Texture* playerTextures[6];


/* textures of enemies */
SDL_Texture* goombaTextures[2];
