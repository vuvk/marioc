#pragma once

#include "SDL2/SDL.h"

#include "engine.h"

typedef struct
{
    char* fileName;
    int w, h;
} STexture;


/* textures of level */
SDL_Texture* levelTextures[10];

/* textures of player */
SDL_Texture* playerTextures[7];

/* textures of enemies */
SDL_Texture* goombaTextures[3];




void LoadTexture (SDL_Texture** texture, const char* fileName);

void LoadTextures();
