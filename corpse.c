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

    corpse->physBody = PhysObjectCreate (x, y, w, h, PHYSOBJ_COLLISION_WITH_LEVEL);
    ListAddElement (physObjects, corpse->physBody);

    corpse->timeToRemove = timeToRemove;
    corpse->_timeToRemove = 0.0f;
    corpse->texture = texture;

    return corpse;
}

void CorpseDestroy (SCorpse** corpse)
{
    if (corpse == NULL || *corpse == NULL)
        return;

    if ((*corpse)->physBody != NULL)
        ListDeleteElementByValue (physObjects, (*corpse)->physBody);

    free (*corpse);
    *corpse = NULL;
}

void CorpseClearAll ()
{
    for (SListElement* element = corpses->first; element; element = element->next)
    {
        if (element->value != NULL)
            CorpseDestroy((SCorpse**) &element->value);
    }

    ListClear (corpses);
}

void CorpseGetSdlRect (SCorpse* corpse, SDL_Rect* rect)
{
    if (corpse == NULL || corpse->physBody == NULL || rect == NULL)
        return;

    SPhysObject* physBody = corpse->physBody;

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
    if (corpses->first == NULL)
        return;

    SCorpse* corpse;
    SListElement* element = corpses->first;
    while (element != NULL)
    {
        corpse = NULL;
        if (element)
            corpse = (SCorpse*) element->value;

        if (corpse != NULL)
        {
            SPhysObject* physBody = corpse->physBody;

            if (physBody == NULL)
            {
                SListElement* nextElement = element->next;
                ListDeleteElementByValue (corpses, corpse);
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
                ListDeleteElementByValue (physObjects, physBody);
                ListDeleteElementByValue (corpses, corpse);
                element = nextElement;

                continue;
            }


            corpse->_timeToRemove += deltaTime;
            // remove if time out
            if (corpse->_timeToRemove >= corpse->timeToRemove)
            {
                SListElement* nextElement = element->next;
                ListDeleteElementByValue (physObjects, physBody);
                ListDeleteElementByValue (corpses, corpse);
                element = nextElement;

                continue;
            }

            // now draw
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

        element = element->next;
    }
}
