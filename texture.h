#pragma once

#include "SDL2/SDL.h"

#include "defines.h"
#include "engine.h"

typedef struct
{
    char* fileName;
    int w, h;
} STexture;


/* textures of level */
SDL_Texture* levelTextures;

/* textures of player */
SDL_Texture* playerTextures[13];

/* textures of enemies */
SDL_Texture* goombaTextures[3];

/* textures of surprises */
SDL_Texture* surpriseTextures[2];

/* misc textures */
SDL_Texture* miscTextures[3];




void TextureLoad (SDL_Texture** texture, const char* fileName);
void LoadTextures();

#define TextureSetColor (texture, r, g, b) TextureArraySetColor (texture, r, g, b, 1)
/* можно указать массив для установки цвета всем элементам */
void TextureArraySetColor (SDL_Texture** textures, const ubyte r, const ubyte g, const ubyte b, uint16 count);
