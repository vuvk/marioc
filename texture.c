#include <stdio.h>

#include "additions.h"
#include "texture.h"



void TextureLoad (SDL_Texture** texture, const char* fileName)
{
    if (FileExists (fileName))
        *texture = EngineLoadTexture (fileName);
    else
        printf ("File '%s' doesn't exists!\n", fileName);
}

void LoadTextures()
{
    uint16 i;
    char textBuffer[30];

    /* TEXTURES for PLAYER */
    for (i = 0; i < 7; i++)
    {
        sprintf (textBuffer, "./media/tex/player%i.png", i);
        TextureLoad (&playerTextures[i], textBuffer);
    }
    /* TEXTURES for BIG PLAYER */
    for (i = 0; i < 6; i++)
    {
        sprintf (textBuffer, "./media/tex/big_player%i.png", i);
        TextureLoad (&playerTextures[i + 7], textBuffer);
    }

    /* TEXTURES for GOOMBA */
    for (i = 0; i < 3; i++)
    {
        sprintf (textBuffer, "./media/tex/goomba%i.png", i);
        TextureLoad (&goombaTextures[i], textBuffer);
    }

    /* TEXTURES for LEVEL OBJECTS */
    TextureLoad (&levelTextures, "./media/tex/tiles.png");

    /* surprises */
    TextureLoad (&surpriseTextures[0], "./media/tex/mushroom.png");

    /* misc */
    TextureLoad (&miscTextures[0], "./media/tex/brick.png");
    TextureLoad (&miscTextures[1], "./media/tex/blood.png");
}

void TextureArraySetColor (SDL_Texture** textures, const ubyte r, const ubyte g, const ubyte b, uint16 count)
{
    for (uint16 i = 0; i < count; i++)
    {
        SDL_SetTextureColorMod (*textures, r, g, b);
        textures++;
    }
}
