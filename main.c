#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*#include <conio.h>*/

/*#include "SDL2/SDL.h"*/

#include "defines.h"
#include "additions.h"
#include "engine.h"
#include "texture.h"
#include "vector2d.h"
#include "creature.h"
#include "player.h"

SDL_Rect backgroundRect;

SDL_Event sdlEvent;
bool isGameActive = true;

int Initialize ()
{
    /* set seed for randomizer */
    srand (time (0));

    if (EngineStart())
    {
        printf ("SDL Initialized.\n");

        if (!EngineCreateWindow ("Mario in C",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 WINDOW_WIDTH, WINDOW_HEIGHT,
                                 SDL_WINDOW_SHOWN))
        {
            printf ("I can't create SDL window :(");
            return ERR_SDL_WINDOW_NOT_CREATED;
        }

        if (!EngineCreateRenderer (-1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))
        {
            printf ("I can't create SDL renderer :(");
            return ERR_SDL_RENDERER_NOT_CREATED;
        }

        return ERR_NONE;
    }
    else
    {
        return ERR_SDL_NOT_INITIALIZED;
    }

}

void LoadTexture (SDL_Texture** texture, const char* fileName)
{
    if (FileExists (fileName))
        *texture = EngineLoadTexture (fileName);
    else
        printf ("File '%s' doesn't exists!\n", fileName);
}

void LoadResources()
{
    int i;
    char buffer[30];

    /* TEXTURES for PLAYER */
    for (i = 0; i < 6; i++)
    {
        sprintf (buffer, "./media/player%i.png", i);
        LoadTexture (&playerTextures[i], buffer);
    }

    /* TEXTURES for GOOMBA */
    for (i = 0; i < 2; i++)
    {
        sprintf (buffer, "./media/goomba%i.png", i);
        LoadTexture (&goombaTextures[i], buffer);
    }

    /* TEXTURES for LEVEL OBJECTS */
    /* background */
    LoadTexture (&levelTextures[0], "./media/background.png");
    /* blocks */
    LoadTexture (&levelTextures[1], "./media/block.png");
}

void Render ()
{
    SDL_Rect rect;
    SCreature* creature;

    EngineRenderClear();

    /* draw level */
    EngineRenderImage (levelTextures[0], &backgroundRect, false);
    EngineRenderLevel();


    /* draw all creatures */
    int i;
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        creature = creatures[i];

        if (creature != NULL)
        {
            CreatureGetSdlRect (creature, &rect);

            EngineRenderImage (CreatureGetTexture (creature, creature->curFrame), &rect, (creature->xDir < 0));
        }
    }

    /* draw present */
    EngineRenderPresent();

    SDL_Delay (1);
}

void Update ()
{
    /* update ticks and deltaTime */
    EngineUpdateTime();

    int i;
    SCreature* creature;

    PlayerUpdateFrames (player);
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        creature = creatures[i];

        if (creature != NULL)
        {
            CreatureUpdateMove (creature);
            CreatureUpdatePhysics (creature);
            CreatureUpdateAnimation (creature);

            if (creature != player)
                CreatureUpdateAI (creature);
        }
    }
}

int main (int argc, char *argv[])
{
    Initialize();
    LoadResources();
    LevelLoad();

    backgroundRect.x = 0;
    backgroundRect.y = 0;
    backgroundRect.w = WINDOW_WIDTH;
    backgroundRect.h = WINDOW_HEIGHT;

    while (isGameActive)
    {
        Update();

        while (SDL_PollEvent (&sdlEvent))
        {
            SDL_PumpEvents();

            /* close window */
            if (sdlEvent.type == SDL_QUIT)
            {
                isGameActive = false;
            }

            /* key input */
            if (sdlEvent.type == SDL_KEYDOWN)
            {
                switch (sdlEvent.key.keysym.sym)
                {
                    case SDLK_d :
                    case SDLK_RIGHT :
                    {
                        player->xDir = 1;
                        moveR = true;
                        moveL = false;
                        break;
                    }
                    case SDLK_a :
                    case SDLK_LEFT :
                    {
                        player->xDir = -1;
                        moveR = false;
                        moveL = true;
                        break;
                    }

                    case SDLK_LCTRL :
                    case SDLK_RCTRL :
                    {
                        player->moveSpeed = 7.5f;
                        player->accelSpeed = 30.0f;
                        break;
                    }

                    case SDLK_UP :
                    case SDLK_w :
                    case SDLK_SPACE :
                    {
                        if (player->isGrounded)
                        {
                            player->pos.y -= 1.0f;
                            CreatureAddImpulse (player, 0.0f, -6.5f);
                        }
                        break;
                    }

                    case SDLK_r :
                    {
                        LevelLoad();
                        break;
                    }

                    case SDLK_ESCAPE :
                    {
                        isGameActive = false;
                        break;
                    }
                }
            }

            if (sdlEvent.type == SDL_KEYUP)
            {
                switch (sdlEvent.key.keysym.sym)
                {
                    case SDLK_a :
                    case SDLK_LEFT :
                    {
                        moveL = false;
                        break;
                    }

                    case SDLK_d :
                    case SDLK_RIGHT :
                    {
                        moveR = false;
                        break;
                    }

                    case SDLK_LCTRL :
                    case SDLK_RCTRL :
                    {
                        player->moveSpeed = 5.0f;
                        player->accelSpeed = 20.0f;
                        break;
                    }
                }
            }
        }

        /* player movement */
        if (!moveL && !moveR)
        {
            //player->impulse.x = 0.0f;
        }
        else
        {
            if (abs (player->impulse.x) < EPSILON)
                CreatureAddImpulse (player, player->xDir, 0.0f);

            if (moveL)
                CreatureAddImpulse (player, -(player->accelSpeed)*deltaTime, 0.0f);
            if (moveR)
                CreatureAddImpulse (player,  (player->accelSpeed)*deltaTime, 0.0f);
        }

        Render();

        /* show fps */
        char buffer[25];
        sprintf (buffer, "Mario in C    FPS %d", fps);
        EngineSetWindowTitle (buffer);
    }

    CreatureClearAll();
    LevelClear();

    EngineStop();
    printf ("\nGoodbye, my love...\n");
    //getch();

    return ERR_NONE;
}
