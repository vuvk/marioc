#include <stdlib.h>
#include <math.h>


#include "engine.h"
#include "sound.h"
#include "physObj.h"
#include "surprise.h"
#include "player.h"



SSurprise* SurpriseCreate (ESurpriseType surpriseType,
                           float x, float y,
                           ubyte w, ubyte h,
                           float moveSpeed,
                           SDL_Texture* texture)
{
    SSurprise* surprise = (SSurprise*) malloc (sizeof (SSurprise));

    surprise->surpriseType = surpriseType;
    surprise->moveSpeed = moveSpeed;
    surprise->accelSpeed = 20.0f;
    surprise->texture = texture;
    surprise->xDir = 0;

    surprise->physBody = PhysObjectCreate (x, y, w, h, PHYSOBJ_COLLISION_WITH_ALL);
    ListAddElement (physObjects, surprise->physBody);

    return surprise;
}

void SurpriseDestroy (SSurprise** surprise)
{
    if (surprise == NULL || *surprise == NULL)
        return;

    if ((*surprise)->physBody != NULL)
        ListDeleteElementByValue (physObjects, (*surprise)->physBody);

    free (*surprise);
    *surprise = NULL;
}

void SurpriseClearAll ()
{
    for (SListElement* element = surprises->first; element; element = element->next)
    {
        if (element->value != NULL)
            SurpriseDestroy((SSurprise**) &element->value);
    }
    ListClear (surprises);
}

void SurpriseAddImpulse (SSurprise* surprise, float x, float y)
{
    if (surprise == NULL || surprise->physBody == NULL)
        return;

    PhysObjectAddImpulse (surprise->physBody, x, y);
}

bool IsPlaceFreeForSurprise (float x, float y,
                             SLevelObject** obstacleLevelObject)
{
    /* check elements of level */
    short xPos = (short)x / BLOCK_SIZE;
    short yPos = (short)y / BLOCK_SIZE;
    if (xPos < 0 || xPos >= LEVEL_WIDTH ||
        yPos < 0 || yPos >= LEVEL_HEIGHT)
        return false;

    SLevelObject* levelObject = level[yPos][xPos];
    if ((levelObject != NULL) && (levelObject->isSolid))
    {
        *obstacleLevelObject = levelObject;
        return false;
    }

    return true;
}

void SurpriseUpdatePhysics (SSurprise* surprise)
{
    if ((surprise == NULL) || (surprise->physBody == NULL) || (surprise->moveSpeed == 0.0f))
        return;

    // get physbody if this exists
    SPhysObject* physBody = surprise->physBody;

    // ограничение скорости движения физического тела
    if (abs(physBody->impulse.x) > surprise->moveSpeed)
        physBody->impulse.x = (surprise->moveSpeed)*(surprise->xDir);

    SLevelObject* obstacleLevelObject;
    SVector2f testPosition;
    bool isCollided;     /* было ли уже взаимодействие */

    /* если преграда впереди (крайняя точка + impulse.x), то туда нельзя идти... */
    /* проверяем три точки: у головы, у центра, у ног по Y до первого взаимодействия */
    isCollided = false;
    testPosition.x = physBody->center.x;
    testPosition.y = physBody->pos.y + 1.0f;
    if (physBody->impulse.x < -EPSILON)
        testPosition.x = physBody->pos.x - 8.0f;
    if (physBody->impulse.x >  EPSILON)
        testPosition.x = physBody->pos.x + physBody->w + 8.0f;

    for (uint16 i = 0; ((i < 3) && (!isCollided)); i++)
    {
        obstacleLevelObject = NULL;
        if ((!IsPlaceFreeForSurprise (testPosition.x, testPosition.y, &obstacleLevelObject))  &&
            (obstacleLevelObject != NULL))
        {
            //physBody->impulse.x *= -1;
            physBody->impulse.x = 0.0f;
            surprise->xDir *= -1;

            isCollided = true;
        }

        testPosition.y += physBody->halfH - 1;
    }
}

bool SurpriseIsCollisionPhysObject (SSurprise* s1, SPhysObject* o2)
{
    if (s1 == NULL || s1->physBody == NULL || o2 == NULL)
        return false;

    return (PhysObjectIsCollisionPhysObject (s1->physBody, o2));
}

bool SurpriseIsCollisionCreature (SSurprise* s1, SCreature* c2)
{
    if (s1 == NULL || c2 == NULL)
        return false;

    SPhysObject* physBody1 = s1->physBody;
    SPhysObject* physBody2 = c2->physBody;

    if (physBody1 != NULL && physBody2 != NULL)
        return (PhysObjectIsCollisionPhysObject (physBody1, physBody2));
    else
        return false;
}

bool SurpriseIsCollisionLevelObject (SSurprise* s1, SLevelObject* l2)
{
    if (s1 == NULL || s1->physBody == NULL || l2 == NULL)
        return false;

    return (PhysObjectIsCollisionLevelObject (s1->physBody, l2));
}

void SurpriseGetSdlRect (SSurprise* surprise, SDL_Rect* rect)
{
    if (surprise == NULL || surprise->physBody == NULL || rect == NULL)
        return;

    SPhysObject* physBody = surprise->physBody;

    rect->x = (int)(physBody->pos.x - cameraPos.x);
    rect->y = (int)(physBody->pos.y - cameraPos.y);
    rect->w = physBody->w;
    rect->h = physBody->h;
}

SDL_Texture* SurpriseGetTexture (SSurprise* surprise)
{
    if (surprise == NULL)
        return NULL;

    return surprise->texture;
}

void SurpriseUpdateAI (SSurprise* surprise)
{
    if (surprise == NULL || surprise->physBody == NULL)
        return;

    SPhysObject* physBody = surprise->physBody;

    if (surprise->moveSpeed != 0.0f)
    {
        /* random direction */
        if (surprise->xDir == 0)
        {
            surprise->xDir = rand() % 2;
            if (surprise->xDir == 0)
                surprise->xDir = -1;
        }

        /* move... */
        if (abs (physBody->impulse.x) < EPSILON)
            SurpriseAddImpulse (surprise, surprise->xDir, 0.0f);
        SurpriseAddImpulse (surprise, (surprise->accelSpeed)*(surprise->xDir)*deltaTime, 0.0f);
    }
}

void SurprisesUpdateAndRender ()
{
    if (surprises->first == NULL)
        return;

    SSurprise* surprise;
    SListElement* element = surprises->first;
    while (element != NULL)
    {
        surprise = NULL;
        if (element)
            surprise = (SSurprise*) element->value;

        if (surprise != NULL)
        {
            SurpriseUpdatePhysics (surprise);
            SurpriseUpdateAI (surprise);

            SPhysObject* physBody = surprise->physBody;

            if (physBody == NULL)
            {
                SListElement* nextElement = element->next;
                ListDeleteElementByValue (surprises, surprise);
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
                ListDeleteElementByValue (surprises, surprise);
                element = nextElement;

                continue;
            }

            // player eat surprise?
            if (player != NULL)
            if (SurpriseIsCollisionCreature (surprise, player))
            {
                switch (surprise->surpriseType)
                {
                    case stMushroom :
                    {
                        if (player->health < 2)
                            player->health ++;

                        SoundPlay (sndPowerUp, 0);
                    }

                    default :
                        break;
                }

                SListElement* nextElement = element->next;
                ListDeleteElementByValue (physObjects, physBody);
                ListDeleteElementByValue (surprises, surprise);
                element = nextElement;

                continue;
            }


            // now draw
            if (surprise->texture != NULL)
            {
                SDL_Rect rect;
                SurpriseGetSdlRect(surprise, &rect);
                // if rect in screen range
                if ((rect.x + rect.w) > 0 &&
                    (rect.x <= WINDOW_WIDTH))
                    EngineRenderImage (surprise->texture, &rect, false);
            }
        }

        element = element->next;
    }
}
