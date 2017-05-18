#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "vector2d.h"
#include "level.h"
#include "physObj.h"
#include "corpse.h"


SCorpse* CorpseCreate (float x, float y, ubyte w, ubyte h, float timeToRemove, SDL_Texture* texture)
{
    SCorpse* corpse = (SCorpse*) malloc (sizeof(SCorpse));

    /* search place for body in physObjects array */
    uint16 i = 0;
    while ((physObjects[i] != NULL) && (i < MAX_PHYSOBJECTS_COUNT))
        i++;
    physObjects[i] = PhysObjectCreate (x, y, w, h, PHYSOBJ_COLLISION_WITH_LEVEL);
    corpse->physBodyIndex = i;

    corpse->timeToRemove = timeToRemove;
    corpse->_timeToRemove = 0.0f;
    corpse->texture = texture;

    return corpse;
}

void CorpseDestroy (SCorpse** corpse)
{
    if (corpse == NULL || *corpse == NULL)
        return;

    SPhysObject* physBody = physObjects[(*corpse)->physBodyIndex];
    if (physBody != NULL)
        PhysObjectDestroy (&(physObjects[(*corpse)->physBodyIndex]));

    free (*corpse);
    *corpse = NULL;
}

void CorpseClearAll ()
{
    for (uint16 i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        if (corpses[i] != NULL)
            CorpseDestroy (&(corpses[i]));
    }
}

void CorpseGetSdlRect (SCorpse* corpse, SDL_Rect* rect)
{
    if (corpse == NULL || rect == NULL)
        return;

    SPhysObject* physBody = physObjects[corpse->physBodyIndex];
    if (physBody == NULL)
        return;

    rect->x = (int)(physBody->pos.x - cameraPos.x);
    rect->y = (int)(physBody->pos.y - cameraPos.y);
    rect->w = physBody->w;
    rect->h = physBody->h;
}

SDL_Texture* CorpseGetTexture (SCorpse* corpse)
{
    if (corpse == NULL)
        return NULL;

    return corpse->texture;
}


void CorpsesUpdateAndRender ()
{
    SCorpse* corpse;
    for (uint16 i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        corpse = corpses[i];

        if (corpse != NULL)
        {
            corpse->_timeToRemove += deltaTime;

            /* remove if time out */
            if (corpse->_timeToRemove >= corpse->timeToRemove)
            {
                CorpseDestroy (&(corpses[i]));
                continue;
            }

            /* check edges of level */
            SPhysObject* physBody = physObjects[corpse->physBodyIndex];
            if (physBody == NULL)
            {
                CorpseDestroy (&(corpses[i]));
                continue;
            }

            short xPos = (short)(physBody->pos.x + (physBody->w >> 1)) / BLOCK_SIZE;
            short yPos = (short)(physBody->pos.y + (physBody->h >> 1)) / BLOCK_SIZE;

            if (xPos < 0 || xPos >= LEVEL_WIDTH ||
                yPos < 0 || yPos >= LEVEL_HEIGHT)
            {
                CorpseDestroy (&(corpses[i]));
                continue;
            }

            /* now draw... */
            if (corpse->texture != NULL)
            {
                SDL_Rect rect;
                CorpseGetSdlRect(corpse, &rect);
                // if rect in screen range
                if ((rect.x + rect.w) > 0 &&
                    (rect.x <= WINDOW_WIDTH))
                    EngineRenderImage (corpse->texture, &rect, false);
            }
        }
    }
}
