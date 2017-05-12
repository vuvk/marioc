#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*#include <conio.h>*/

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
#include "physObj.h"
#include "player.h"

SDL_Rect backgroundRect;

SDL_Event sdlEvent;
bool isGameActive = true;

/* не выше LIMIT_FPS */
/*float limitFps;*/

int Initialize ()
{
    /* set seed for randomizer */
    srand (time (0));

    /*limitFps = 0.0f;*/

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

        if (!EngineCreateRenderer (-1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))
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
    SDL_Rect rect;
    SCreature* creature;
    SCorpse* corpse;

    EngineRenderClear();

    /* draw level */
    EngineRenderImage (levelTextures[0], &backgroundRect, false);
    LevelUpdateAndRender ();
    LumpsUpdateAndRender();

    /* draw all creatures and corpses */
    register unsigned short i;
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        creature = creatures[i];
        corpse = corpses[i];

        if (corpse != NULL)
        {
            CorpseGetSdlRect (corpse, &rect);
            EngineRenderImage (CorpseGetTexture(corpse), &rect, false);
        }

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

    PlayerUpdate (player);
    PlayerUpdateFrames(player);

    CreaturesUpdate();
    CorpsesUpdate ();
    PhysObjectsUpdate();
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
                            player->moveSpeed = 7.5f;
                            player->accelSpeed = 30.0f;
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
                                yPos = (short)(physBody->pos.y - 1.0f) / BLOCK_SIZE;

                                /* проверяем позицию левого края над головой */
                                xPos = (short)(physBody->pos.x) / BLOCK_SIZE;
                                levelObject = level[yPos][xPos];
                                if (levelObject != NULL &&
                                    levelObject->isSolid &&
                                    levelObject->isStatic)
                                    canJump = false;

                                /* если всё ещё можешь прыгать, то проверим позицию в центре над головой */
                                if (canJump)
                                {
                                    xPos = (short)(physBody->center.x) / BLOCK_SIZE;
                                    levelObject = level[yPos][xPos];
                                    if (levelObject != NULL &&
                                        levelObject->isSolid &&
                                        levelObject->isStatic)
                                    canJump = false;
                                }

                                /* осталась последняя проверка в правом углу... */
                                if (canJump)
                                {
                                    xPos = (short)(physBody->pos.x + physBody->w) / BLOCK_SIZE;
                                    levelObject = level[yPos][xPos];
                                    if (levelObject != NULL &&
                                        levelObject->isSolid &&
                                        levelObject->isStatic)
                                    canJump = false;
                                }

                                /*if (IsPlaceFree (physBody->pos.x,               physBody->center.y - physBody->halfH - 1.0f, false, NULL, NULL) &&
                                    IsPlaceFree (physBody->center.x,            physBody->center.y - physBody->halfH - 1.0f, false, NULL, NULL) &&
                                    IsPlaceFree (physBody->pos.x + physBody->w, physBody->center.y - physBody->halfH - 1.0f, false, NULL, NULL))*/
                                if (canJump)
                                {
                                    physBody->pos.y -= 1.0f;
                                    CreatureAddImpulse (player, 0.0f, -6.5f);

                                    Mix_PlayChannel (-1, sndJump, 0);
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
                        LevelLoad();
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
                            player->moveSpeed = 5.0f;
                            player->accelSpeed = 20.0f;
                        }
                        break;
                    }
                }
            }
        }

        /* player movement */
        if (player != NULL)
        {
            if (!moveL && !moveR)
            {
                /* player->impulse.x = 0.0f; */
            }
            else
            {
                SPhysObject* physBody = physObjects[player->physBodyIndex];
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

        /*limitFps += deltaTime;
        if (limitFps >= LIMIT_FPS)
        {
            limitFps -= LIMIT_FPS;
            Update ();
            Render ();
        }*/

        Render();
        deltaTime = 0.0f;

        /* show fps */
        char buffer[25];
        sprintf (buffer, "Mario in C    FPS %d", fps);
        EngineSetWindowTitle (buffer);
    }

    CreatureClearAll();
    CorpseClearAll();
    LumpClearAll();
    PhysObjectClearAll();
    LevelClear();

    EngineStop();
    printf ("\nGoodbye, my love...\n");

    return ERR_NONE;
}
