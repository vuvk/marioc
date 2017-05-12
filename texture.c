#include <stdio.h>

#include "additions.h"
#include "texture.h"



void LoadTexture (SDL_Texture** texture, const char* fileName)
{
    if (FileExists (fileName))
        *texture = EngineLoadTexture (fileName);
    else
        printf ("File '%s' doesn't exists!\n", fileName);
}

void LoadTextures()
{
    register unsigned short i;
    char buffer[30];

    /* TEXTURES for PLAYER */
    for (i = 0; i < 7; i++)
    {
        sprintf (buffer, "./media/tex/player%i.png", i);
        LoadTexture (&playerTextures[i], buffer);
    }

    /* TEXTURES for GOOMBA */
    for (i = 0; i < 3; i++)
    {
        sprintf (buffer, "./media/tex/goomba%i.png", i);
        LoadTexture (&goombaTextures[i], buffer);
    }

    /* TEXTURES for LEVEL OBJECTS */
    /* background */
    LoadTexture (&levelTextures[0], "./media/tex/background.png");
    /* blocks */
    LoadTexture (&levelTextures[1], "./media/tex/block.png");
    LoadTexture (&levelTextures[2], "./media/tex/red_brick.png");
    LoadTexture (&levelTextures[3], "./media/tex/bonus_block0.png");
    LoadTexture (&levelTextures[4], "./media/tex/bonus_block3.png");

    /* misc */
    LoadTexture (&miscTextures[0], "./media/tex/brick.png");
    LoadTexture (&miscTextures[1], "./media/tex/blood.png");
}
