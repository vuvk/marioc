#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "SDL2/SDL.h"

#include "defines.h"
#include "additions.h"
#include "engine.h"
#include "texture.h"
#include "sound.h"
#include "vector2d.h"
#include "creature.h"
#include "corpse.h"
#include "lump.h"
#include "surprise.h"
#include "physObj.h"
#include "player.h"


SDL_Event sdlEvent;
bool isGameActive = true;

int Initialize (bool soft)
{
    /* set seed for randomizer */
    srand (time (0));

    int flags;
    if (soft)
        flags = SDL_RENDERER_SOFTWARE;
    else
        flags = SDL_RENDERER_ACCELERATED;

    if (EngineStart())
    {
        printf ("SDL Initialized.\n");

        if (!EngineCreateWindow ("Mario in C",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 WINDOW_WIDTH, WINDOW_HEIGHT,
                                 SDL_WINDOW_SHOWN))
        {
            printf ("I can't create SDL window :(\n");
            return ERR_SDL_WINDOW_NOT_CREATED;
        }

        if (!EngineCreateRenderer (-1, flags))
        {
            printf ("I can't create SDL renderer :(\n");
            return ERR_SDL_RENDERER_NOT_CREATED;
        }

        if (!EngineInitAudio())
        {
            printf ("I can't initialize audio :(\n");
            return ERR_SDL_AUDIO_NOT_INITIALIZED;
        }

        return ERR_NONE;
    }
    else
    {
        return ERR_SDL_NOT_INITIALIZED;
    }
}

void LoadResources()
{
    LoadTextures();

    LoadSounds();
}

void Render ()
{
    EngineRenderClear();

    /* draw level */
    LevelUpdateAndRender ();

    /* update and draw objects */
    LumpsUpdateAndRender ();
    SurprisesUpdateAndRender ();
    CorpsesUpdateAndRender ();
    CreaturesUpdateAndRender ();

    /* draw present */
    EngineRenderPresent();
}

void Update ()
{
    /*if (SDL_GetError())
    {
        printf ("%s\n", SDL_GetError());
        exit(666);
    }*/

    /* update ticks and deltaTime */
    EngineUpdateTime();

    PlayerUpdate ();
    PlayerUpdateFrames();

    PhysObjectsUpdate();
}

int main (int argc, char *argv[])
{
    /* software rendering */
    bool soft = false;

    /* check params */
    if (argc > 1)
    {
        for (uint16 i = 0; i < argc; i++)
            if (strcmp ("-soft", argv[i]) == 0)
                soft = true;
    }

    Initialize (soft);
    EngineClearAllInstances();
    LoadResources ();
    LevelLoad ("./media/maps/level01.tmx");

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
                        if (player != NULL)
                            player->xDir =  1;
                        moveR = true;
                        moveL = false;
                        break;
                    }
                    case SDLK_a :
                    case SDLK_LEFT :
                    {
                        if (player != NULL)
                            player->xDir = -1;
                        moveR = false;
                        moveL = true;
                        break;
                    }

                    case SDLK_LCTRL :
                    case SDLK_RCTRL :
                    {
                        if (player != NULL)
                        {
                            player->moveSpeed = BLOCK_SIZE / 4.0f;
                            player->accelSpeed = BLOCK_SIZE;
                        }
                        break;
                    }

                    case SDLK_UP :
                    case SDLK_w :
                    case SDLK_SPACE :
                    {
                        if (player != NULL)
                        {
                            SPhysObject* physBody = physObjects[player->physBodyIndex];
                            if (physBody != NULL && physBody->isGrounded)
                            {
                                bool canJump = true;
                                SLevelObject* levelObject;
                                short xPos, yPos;
                                SVector2f testPosition;
                                testPosition.x = physBody->pos.x + 1.0f;
                                testPosition.y = physBody->pos.y - 2.0f;
                                yPos = (short)(physBody->pos.y - 2.0f) / BLOCK_SIZE;

                                for (ubyte i = 0; (i < 3) && (canJump); i++)
                                {
                                    /* проверяем позицию левого края над головой */
                                    xPos = (short)(testPosition.x) / BLOCK_SIZE;
                                    levelObject = level[yPos][xPos];
                                    if (levelObject != NULL &&
                                        levelObject->isSolid &&
                                        levelObject->isStatic)
                                    {
                                        canJump = false;
                                        continue;
                                    }

                                    testPosition.x += physBody->halfW - 1.0f;
                                }

                                if (canJump)
                                {
                                    physBody->pos.y -= 1.0f;
                                    CreatureAddImpulse (player, 0.0f, -BLOCK_SIZE / 5.0f);

                                    SoundPlay (sndJump, 0);
                                }
                            }

                        }
                        break;
                    }

                    case SDLK_DOWN :
                    case SDLK_s :
                    {
                        if (player != NULL)
                        {
                            SPhysObject* physBody = physObjects[player->physBodyIndex];
                            if ((physBody != NULL) && (player->health == 2))
                            {
                                /*physBody->h = BLOCK_SIZE;*/
                                physBody->pos.y += BLOCK_SIZE;

                                player->health = 1;
                            }
                        }
                        break;
                    }

                    case SDLK_r :
                    {
                        StopAllSounds();
                        EngineClearAllInstances();
                        LevelLoad ("./media/maps/level01.tmx");
                        break;
                    }


                    case SDLK_q :
                    {
                        int i, count = 0;
                        for (i = 0; i < MAX_CREATURES_COUNT; i++)
                        {
                            if (corpses[i] != NULL)
                                count ++;
                        }
                        printf("corpses - %d\n", count);
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

                    case SDLK_DOWN :
                    case SDLK_s :
                    {
                        if (player != NULL)
                        {
                            SPhysObject* physBody = physObjects[player->physBodyIndex];
                            if ((physBody != NULL) && (player->health == 1))
                            {
                                /*physBody->h = BLOCK_SIZE << 1;*/
                                physBody->pos.y -= BLOCK_SIZE;

                                player->health = 2;
                            }

                        }
                        break;
                    }

                    case SDLK_LCTRL :
                    case SDLK_RCTRL :
                    {
                        if (player != NULL)
                        {
                            player->moveSpeed = BLOCK_SIZE / 5.0f;
                            player->accelSpeed = BLOCK_SIZE * 0.65f;
                        }
                        break;
                    }
                }
            }
        }

        /* player movement */
        if (player != NULL)
        {
            SPhysObject* physBody = physObjects[player->physBodyIndex];

            if (!moveL && !moveR)
            {
                /* player->impulse.x = 0.0f; */
            }
            else
            {
                if (physBody != NULL)
                {
                    /* куда должен сместиться игрок */
                    float dx = (player->accelSpeed)*deltaTime;

                    /* проверяем, можно ли двигаться */
                    if (moveL)
                    if (IsPlaceFree (physBody->pos.x - dx, physBody->center.y, false, NULL, NULL))
                    {
                        /* просто первоначальный толчок, чтобы перебороть трение (костыль какой-то...) */
                        if (abs (physBody->impulse.x) < EPSILON)
                            CreatureAddImpulse (player, player->xDir, 0.0f);

                        CreatureAddImpulse (player, -dx, 0.0f);
                    }

                    if (moveR)
                    if (IsPlaceFree (physBody->pos.x + physBody->w + dx, physBody->center.y, false, NULL, NULL))
                    {
                        /* просто первоначальный толчок, чтобы перебороть трение (костыль какой-то...) */
                        if (abs (physBody->impulse.x) < EPSILON)
                            CreatureAddImpulse (player, player->xDir, 0.0f);

                        CreatureAddImpulse (player, dx, 0.0f);
                    }
                }
            }
        }

        Render();

        /* show fps */
        char buffer[25];
        sprintf (buffer, "Mario in C    FPS %d", fps);
        EngineSetWindowTitle (buffer);

        /* limit fps now */
        float frameRate = 1000.0f / LIMIT_FPS;
        if (deltaTime < frameRate)
            SDL_Delay ((int)(frameRate - deltaTime));
    }

    CreatureClearAll();
    CorpseClearAll();
    LumpClearAll();
    SurpriseClearAll();
    PhysObjectClearAll();
    LevelClear();

    EngineStop();
    printf ("\nGoodbye, my love...\n");

    return ERR_NONE;
}
