#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "level.h"
#include "lump.h"


SLump* LumpCreate (float x, float y,
                   int w, int h,
                   float timeToRemove,
                   bool isSpinned,
                   SDL_Texture* texture)
{
    SLump* lump = (SLump*) malloc (sizeof(SLump));


    /* search place for body in physObjects array */
    uint16 i = 0;
    while ((physObjects[i] != NULL) && (i < MAX_PHYSOBJECTS_COUNT))
        i++;
    physObjects[i] = PhysObjectCreate (x, y, w, h, PHYSOBJ_COLLISION_WITH_LEVEL);
    lump->physBodyIndex = i;


    lump->angle = 0.0f;
    lump->isSpinned = isSpinned;
    lump->timeToRemove = timeToRemove;
    lump->_timeToRemove = 0.0f;
    lump->texture = texture;

    return lump;
}


void LumpCreateSeveral (float x, float y,
                        int w, int h,
                        float timeToRemove,
                        bool isSpinned,
                        SDL_Texture* texture,
                        const unsigned int num)
{
    SLump* lump;
    SPhysObject* physBody;

    uint16 j = 0;
    for (uint16 i = 0; i < num; i++)
    {
        while ((lumps[j] != NULL) && (j < MAX_LUMPS_COUNT - 1))
            j++;

        lump = LumpCreate (x, y, w, h, timeToRemove, isSpinned, texture);
        physBody = physObjects[lump->physBodyIndex];
        PhysObjectAddImpulse (physBody, (rand () % 101) / 10.0f - 5.0f, (rand () % 101) / 10.0f - 5.0f);
        lumps [j] = lump;

        j++;
    }
}

void LumpDestroy (SLump** lump)
{
    if (lump == NULL || *lump == NULL)
        return;

    SPhysObject* physBody = physObjects[(*lump)->physBodyIndex];
    if (physBody != NULL)
        PhysObjectDestroy (&(physObjects[(*lump)->physBodyIndex]));

    free (*lump);
    *lump = NULL;
}

void LumpClearAll ()
{
    for (uint16 i = 0; i < MAX_LUMPS_COUNT; i++)
    {
        if (lumps[i] != NULL)
        {
            LumpDestroy (&(lumps[i]));
        }
    }
}

void LumpGetSdlRect (SLump* lump, SDL_Rect* rect)
{
    if (lump == NULL || rect == NULL)
        return;

    SPhysObject* physBody = physObjects[lump->physBodyIndex];
    if (physBody == NULL)
        return;

    rect->x = (int)(physBody->pos.x - cameraPos.x);
    rect->y = (int)(physBody->pos.y - cameraPos.y);
    rect->w = physBody->w;
    rect->h = physBody->h;
}

SDL_Texture* LumpGetTexture (SLump* lump)
{
    if (lump == NULL)
        return NULL;

    return lump->texture;
}


void LumpsUpdateAndRender ()
{
    SLump* lump;
    for (uint16 i = 0; i < MAX_LUMPS_COUNT; i++)
    {
        lump = lumps[i];

        if (lump != NULL)
        {
            lump->_timeToRemove += deltaTime;

            /* remove if time out */
            if (lump->_timeToRemove >= lump->timeToRemove)
            {
                LumpDestroy (&(lumps[i]));
                continue;
            }

            /* check edges of level */
            SPhysObject* physBody = physObjects[lump->physBodyIndex];
            if (physBody == NULL)
            {
                LumpDestroy (&(lumps[i]));
                continue;
            }

            short xPos = (short)(physBody->pos.x + (physBody->w >> 1)) / BLOCK_SIZE;
            short yPos = (short)(physBody->pos.y + (physBody->h >> 1)) / BLOCK_SIZE;

            if (xPos < 0 || xPos >= LEVEL_WIDTH ||
                yPos < 0 || yPos >= LEVEL_HEIGHT)
            {
                LumpDestroy (&(lumps[i]));
                continue;
            }


            /*time for drawing! */
            /* rotate lump */
            if (lump->texture != NULL)
            {
                SDL_Rect rect;
                LumpGetSdlRect(lump, &rect);

                if (lump->isSpinned)
                {
                    if (abs(physBody->impulse.x) > EPSILON)
                    {
                        lump->angle += (physBody->impulse.x) * 100 * deltaTime;
                        if (lump->angle >= 360.0f)
                            lump->angle -= 360.0f;
                        if (lump->angle < 0.0f)
                            lump->angle += 360.0f;
                    }

                    if (physBody->isGrounded)
                    {
                        lump->angle = 0.0f;
                        lump->isSpinned = false;
                    }

                    SVector2f center;
                    center.x = physBody->center.x - physBody->pos.x;
                    center.y = physBody->center.y - physBody->pos.y;

                    /* if rect in screen range */
                    if ((rect.x + rect.w) > 0 &&
                        (rect.x <= WINDOW_WIDTH))
                        EngineRenderImageEx (lump->texture, &rect, lump->angle, &center, SDL_FLIP_NONE);
                }
                else
                    /* if rect in screen range */
                    if ((rect.x + rect.w) > 0 &&
                        (rect.x <= WINDOW_WIDTH))
                        EngineRenderImage (lump->texture, &rect, false);
            }
        }
    }
}
