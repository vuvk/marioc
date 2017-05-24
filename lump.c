#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
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

    lump->physBody = PhysObjectCreate (x, y, w, h, PHYSOBJ_COLLISION_WITH_LEVEL);
    ListAddElement (physObjects, lump->physBody);

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
                        const uint32 count)
{
    SLump* lump;
    for (uint32 i = 0; i < count; i++)
    {
        lump = LumpCreate (x, y, w, h, timeToRemove, isSpinned, texture);
        /* random direction */
        if (lump->physBody)
            PhysObjectAddImpulse (lump->physBody, (rand () % 1001) / 100.0f - 5.0f, (rand () % 1001) / 100.0f - 5.0f);

        ListAddElement (lumps, lump);
    }
}

void LumpDestroy (SLump** lump)
{
    if (lump == NULL || *lump == NULL)
        return;

    if ((*lump)->physBody != NULL)
        ListDeleteElementByValue (physObjects, (*lump)->physBody);

    free (*lump);
    *lump = NULL;
}

void LumpClearAll ()
{
    for (SListElement* element = lumps->first; element; element = element->next)
    {
        if (element->value != NULL)
            LumpDestroy ((SLump**) &element->value);
    }

    ListClear (lumps);
}

void LumpGetSdlRect (SLump* lump, SDL_Rect* rect)
{
    if (lump == NULL || lump->physBody == NULL || rect == NULL)
        return;

    SPhysObject* physBody = lump->physBody;

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
    if (lumps->first == NULL)
        return;

    SLump* lump;
    SListElement* element = lumps->first;
    while (element != NULL)
    {
        lump = NULL;
        if (element)
            lump = (SLump*) element->value;

        if (lump != NULL)
        {
            SPhysObject* physBody = lump->physBody;

            if (physBody == NULL)
            {
                SListElement* nextElement = element->next;
                ListDeleteElementByValue(lumps, lump);
                element = nextElement;

                continue;
            }

            // check edges of level
            short xPos = (short)(physBody->pos.x + (physBody->w >> 1)) / BLOCK_SIZE;
            short yPos = (short)(physBody->pos.y + (physBody->h >> 1)) / BLOCK_SIZE;
            if (xPos < 0 || xPos > (LEVEL_WIDTH - 1) ||
                yPos < 0 || yPos > (LEVEL_HEIGHT - 1))
            {
                SListElement* nextElement = element->next;
                ListDeleteElementByValue (physObjects, lump->physBody);
                ListDeleteElementByValue (lumps, lump);
                element = nextElement;

                continue;
            }

            // remove if time out
            lump->_timeToRemove += deltaTime;
            if (lump->_timeToRemove >= lump->timeToRemove)
            {
                SListElement* nextElement = element->next;
                ListDeleteElementByValue (physObjects, lump->physBody);
                ListDeleteElementByValue(lumps, lump);
                element = nextElement;

                continue;
            }

            // time for drawing!
            // rotate lump
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

                    // if rect in screen range
                    if ((rect.x + rect.w) > 0 &&
                        (rect.x <= WINDOW_WIDTH))
                        EngineRenderImageEx (lump->texture, &rect, lump->angle, &center, SDL_FLIP_NONE);
                }
                else
                    // if rect in screen range
                    if ((rect.x + rect.w) > 0 &&
                        (rect.x <= WINDOW_WIDTH))
                        EngineRenderImage (lump->texture, &rect, false);
            }
        }

        element = element->next;
    }
}
