#include <stdio.h>
#include <stdlib.h>

#include "level.h"
#include "corpse.h"


SCorpse* CorpseCreate (float x, float y, int w, int h, float timeToRemove, SDL_Texture** texture)
{
    SCorpse* corpse = (SCorpse*) malloc (sizeof(SCorpse));
    /*corpse->pos.x = x;
    corpse->pos.y = y;
    corpse->w = w;
    corpse->h = h;*/


    /* search place for body in physObjects array */
    register unsigned short i = 0;
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
    register unsigned short int i;

    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        if (corpses[i] != NULL)
        {
            CorpseDestroy (&(corpses[i]));
        }
    }
}

void CorpseGetSdlRect (SCorpse* corpse, SDL_Rect* rect)
{
    if (corpse != NULL && rect != NULL)
    {
        SPhysObject* physBody = physObjects[corpse->physBodyIndex];
        if (physBody == NULL)
        {
            rect->x = rect->y = rect->w = rect->h = 0;
        }
        else
        {
            rect->x = (int)physBody->pos.x;
            rect->y = (int)physBody->pos.y;
            rect->w = physBody->w;
            rect->h = physBody->h;
        }
    }
}

SDL_Texture* CorpseGetTexture (SCorpse* corpse)
{
    if (corpse == NULL)
        return NULL;

    return *(corpse->texture);
}


void CorpsesUpdate ()
{
    SCorpse* corpse;
    register unsigned short int i;
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
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
            SPhysObject* corpseBody = physObjects[corpse->physBodyIndex];
            short xPos = (short)(corpseBody->pos.x + (corpseBody->w >> 1)) / BLOCK_SIZE;
            short yPos = (short)(corpseBody->pos.y + (corpseBody->h >> 1)) / BLOCK_SIZE;

            if (xPos < 0 || xPos >= LEVEL_WIDTH ||
                yPos < 0 || yPos >= LEVEL_HEIGHT)
            {
                CorpseDestroy (&(corpses[i]));
                continue;
            }
        }
    }
}
