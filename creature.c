#include <stdlib.h>
#include <stdio.h>

#include "defines.h"
#include "vector2d.h"
#include "texture.h"
#include "creature.h"
#include "player.h"

SCreature* CreatureCreate (ECreatureType creatureType,
                           int health,
                           float x, float y,
                           int w, int h,
                           float moveSpeed,
                           SDL_Texture** textures,
                           int texCount,
                           float animSpeed)
{
    SCreature* creature = (SCreature*) malloc(sizeof(SCreature));

    creature->creatureType = creatureType;
    creature->health = health;
    creature->pos.x = x;
    creature->pos.y = y;
    creature->w = w;
    creature->h = h;
    creature->halfW = w / 2;
    creature->halfH = h / 2;
    creature->center.x = x + creature->halfW;
    creature->center.y = y + creature->halfH;
    creature->impulse.x = 0.0f;
    creature->impulse.y = 0.0f;

    creature->xDir = 0;

    creature->moveSpeed = moveSpeed;
    creature->accelSpeed = 20.0f;
    creature->friction = 5.0f;

    creature->textures = textures;
    creature->texCount = texCount;

    creature->animDelay = 0.0f;
    creature->animSpeed = animSpeed;
    creature->startFrame = 0;
    creature->endFrame = texCount - 1;
    creature->curFrame = 0;

    return creature;
}

void CreatureDestroy (SCreature* creature)
{
    if (creature == NULL)
        return;

    free (creature);
    creature = NULL;
}

void CreatureClearAll ()
{
    int i;
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        CreatureDestroy (creatures[i]);
    }

    creaturesCount = 0;
}

void CreatureAddImpulse (SCreature* creature, float x, float y)
{
    if (creature == NULL)
        return;

    creature->impulse.x += x;
    creature->impulse.y += y;
}

bool CreatureContainsPoint (SCreature* creature, float x, float y)
{
    if (creature == NULL)
        return false;

    return ((x >= creature->pos.x) && (x <= creature->pos.x + creature->w) &&
            (y >= creature->pos.y) && (y <= creature->pos.y + creature->h));
}

bool CreatureIsCollisionCreature (SCreature* c1, SCreature* c2)
{
    if (c1 == NULL || c2 == NULL)
        return false;

    /* минимальное возможное расстояние между центрами */
    float distX = c1->halfW + c2->halfW;
    float distY = c1->halfH + c2->halfH;

    /* есть столкновение по X или Y */
    bool isCollX = false;
    bool isCollY = false;

    /* если расстояние между центрами меньше возможного, то столкновение точно есть */
    if (abs(c1->center.x - c2->center.x) < distX)
        isCollX = true;
    if (abs(c1->center.y - c2->center.y) < distY)
        isCollY = true;

    return (isCollX && isCollY);
}

bool CreatureIsCollisionLevelObject (SCreature* c1, SLevelObject* l2)
{
    if (c1 == NULL || l2 == NULL)
        return false;

    /* минимальное возможное расстояние между центрами */
    float distX = c1->halfW + BLOCK_SIZE / 2;
    float distY = c1->halfH + BLOCK_SIZE / 2;

    /* есть столкновение по X или Y */
    bool isCollX = false;
    bool isCollY = false;

    /* если расстояние между центрами меньше возможного, то столкновение точно есть */
    if (abs (c1->center.x - l2->center.x) < distX)
        isCollX = true;
    if (abs (c1->center.y - l2->center.y) < distY)
        isCollY = true;

    return (isCollX && isCollY);
}


void CreatureUpdateMove (SCreature* creature)
{
    if (creature == NULL)
        return;

    creature->center.x = creature->pos.x + creature->halfW;
    creature->center.y = creature->pos.y + creature->halfH;

    /* самовыпилиться, если за границей уровня */
    int xPos = ((int)creature->center.x) / BLOCK_SIZE;
    int yPos = ((int)creature->center.y) / BLOCK_SIZE;
    if (xPos < 0 || xPos >= LEVEL_WIDTH ||
        yPos < 0 || yPos >= LEVEL_HEIGHT)
    {
        CreatureDestroy (creature);
        return;
    }

    /* meet creature under self */
    /*int i;
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        SCreature* testCreature = creatures[i];
        if ((testCreature != NULL) && (creature != testCreature))
        {
            if (CreatureIsCollisionCreature (creature, testCreature))
            {
                //* jump
                if ((creature->center.y < testCreature->center.y) &&
                    (creature->impulse.y >= EPSILON))
                {
                    creature->pos.y -= 1.0f;

                    CreatureAddImpulse (creature, 0.0f, -3.5f);
                }
            }
        }
    }*/
}


bool IsPlaceFree (float x, float y,
                  bool checkAll,
                  SLevelObject** obstacleLevelObject, SCreature** obstacleCreature)
{
    /* check elements of level */
    int xPos = (int)x / BLOCK_SIZE;
    int yPos = (int)y / BLOCK_SIZE;
    if (xPos < 0 || xPos >= LEVEL_WIDTH ||
        yPos < 0 || yPos >= LEVEL_HEIGHT)
        return false;

    SLevelObject* levelObject = level[yPos][xPos];
    if ((levelObject != NULL) && (levelObject->solid))
    {
        *obstacleLevelObject = levelObject;
        return false;
    }

    /* check creatures... */
    if (checkAll)
    {
        SCreature* creature;
        int i;
        for (i = 0; i < MAX_CREATURES_COUNT; i++)
        {
            creature = creatures[i];
            if (creature != NULL)
            {
                if (CreatureContainsPoint(creature, x, y))
                {
                    *obstacleCreature = creature;
                    return false;
                }
            }
        }
    }

    /* nobody here ;) */
    return true;
}

void CreatureUpdatePhysics (SCreature* creature)
{
    if (creature == NULL)
        return;

    int xPos, yPos;
    //SVector2f oldPos = creature->pos;
    SLevelObject* levelObject;


    /* limits */
    if (creature->impulse.x < MIN_HORIZONTAL_IMPULSE)
        creature->impulse.x = MIN_HORIZONTAL_IMPULSE;
    if (creature->impulse.x > MAX_HORIZONTAL_IMPULSE)
        creature->impulse.x = MAX_HORIZONTAL_IMPULSE;
    if (creature->impulse.y < MIN_VERTICAL_IMPULSE)
        creature->impulse.y = MIN_VERTICAL_IMPULSE;
    if (creature->impulse.y > MAX_VERTICAL_IMPULSE)
        creature->impulse.y = MAX_VERTICAL_IMPULSE;

    /* previously checking */
    SLevelObject* obstacleLevelObject = NULL;
    SCreature* obstacleCreature = NULL;
    //void* obstacle;
    SVector2f testPosition;
    /* если преграда перед существом (крайняя точка существа + impulse.x), то туда нельзя идти... */
    testPosition.x = creature->center.x + (creature->halfW)*(creature->xDir) + creature->impulse.x;
    testPosition.y = creature->center.y;
    if (!IsPlaceFree (testPosition.x, testPosition.y, true, &obstacleLevelObject, &obstacleCreature))
    //if (!IsPlaceFree (testPosition.x, testPosition.y, true, &obstacle))
    {
        //if (creature != obstacle)
        {
            /* you are enemy? */
            if (creature != player)
            {
                /* kick player's ass!! */
                if (obstacleCreature == player)
                {
                    PlayerGetDamage (player, 1);
                }

                /* change direction... */
                if ((obstacleCreature != player) || (obstacleLevelObject != NULL))
                {
                    creature->impulse.x = 0.0f;
                    creature->xDir *= -1;
                }
            }
        }
    }
    /* если преграда под или над существом, то движение невозможно */
    testPosition.x = creature->center.x;
    testPosition.y = creature->center.y + creature->impulse.y;
    if (creature->impulse.y < EPSILON)
        testPosition.y -= creature->halfH;
    else
        testPosition.y += creature->halfH;
    if (!IsPlaceFree (testPosition.x, testPosition.y, true, &obstacleLevelObject, &obstacleCreature))
    //if (!IsPlaceFree (testPosition.x, testPosition.y, true, &obstacle))
    {
        creature->impulse.y = 0.0f;

        /* obstacle is block */
        if (obstacleLevelObject != NULL)
        {
            //creature->impulse.y = 0.0f;
        }

        /* obstacle is creature */
        if (obstacleCreature != NULL)
        {
            if (creature != obstacleCreature)
            {
                /* you are enemy? */
                if (creature != player)
                {
                    /* kick player's ass!! */
                    if (obstacleCreature == player)
                        PlayerGetDamage (player, 1);
                }

                /* jump, if obstacle is creature under you */
                if (obstacleCreature->center.y > creature->center.y)
                {
                    //creature->pos.y -= 1.0f;
                    CreatureAddImpulse (creature, 0.0f, -3.5f);
                }
            }
        }
    }


    /*****************/
    /* VERTICAL MOVE */
    /*****************/

    /* check gravity */
    creature->isGrounded = false;

    /* apply gravity */
    float gravity = creature->impulse.y;
    if (gravity <= GRAVITY)
        gravity += GRAVITY*deltaTime;
    else
        gravity = GRAVITY;

    xPos = ((int)creature->center.x) / BLOCK_SIZE;
    yPos = ((int)(creature->pos.y + creature->h)) / BLOCK_SIZE;

    levelObject = level [yPos][xPos];
    if (levelObject != NULL)
    {
        if (levelObject->solid)
        {
            creature->isGrounded = true;
        }
    }

    if (creature->isGrounded)
    {
        gravity = 0.0f;
    }

    creature->impulse.y = gravity;


    /*******************/
    /* HORIZONTAL MOVE */
    /*******************/

    /* ограничение по максимальной скорости */
    if (abs (creature->impulse.x) > creature->moveSpeed)
        creature->impulse.x = (creature->moveSpeed)*(creature->xDir);

    /* friction */
    if (abs (creature->impulse.x) > EPSILON)
    {
        if (creature->impulse.x > EPSILON)
            creature->impulse.x -= (creature->friction)*deltaTime;

        if (creature->impulse.x < EPSILON)
            creature->impulse.x += (creature->friction)*deltaTime;
    }
    else
    {
        creature->impulse.x = 0.0f;
    }

    /* check horizontal position */
    xPos = ((int)creature->center.x + creature->impulse.x) / BLOCK_SIZE;
    yPos = ((int)creature->center.y) / BLOCK_SIZE;

    levelObject = level [yPos][xPos];
    if (levelObject != NULL)
    {
        if (levelObject->solid)
        {
            creature->impulse.x = 0.0f;
        }
    }

    /* apply impulse */
    creature->pos = AddVector2f (creature->pos, creature->impulse);



    /*******************/
    /* CHECK COLLISION */
    /*******************/

    xPos = ((int)creature->center.x) / BLOCK_SIZE;
    yPos = ((int)creature->center.y) / BLOCK_SIZE;
    //SVector2f offset;

    /* проверим не застрял ли по горизонтали */
    if (xPos > 0)
    {
        levelObject = level [yPos][xPos - 1];
        if ((levelObject != NULL) && (levelObject->solid))
        {
            /* слева */
            if (creature->pos.x < (levelObject->pos.x + BLOCK_SIZE))
            {
                float offsetX = (levelObject->pos.x + BLOCK_SIZE) - creature->pos.x;
                creature->pos.x += offsetX;
                creature->impulse.x = 0.0f;
            }
        }
    }
    if (xPos < LEVEL_WIDTH - 1)
    {
        levelObject = level [yPos][xPos + 1];
        if ((levelObject != NULL) && (levelObject->solid))
        {
            /* справа */
            if ((creature->pos.x + creature->w) > levelObject->pos.x)
            {
                float offsetX = (creature->pos.x + creature->w) - levelObject->pos.x;
                creature->pos.x -= offsetX;
                creature->impulse.x = 0.0f;
            }
        }
    }
    /* проверим не застрял ли по вертикали */
    if (yPos > 0)
    {
        levelObject = level [yPos - 1][xPos];
        if ((levelObject != NULL) && (levelObject->solid))
        {
            /* сверху */
            if (creature->pos.y < (levelObject->pos.y + BLOCK_SIZE))
            {
                float offsetY = (levelObject->pos.y + BLOCK_SIZE) - creature->pos.y;
                creature->pos.y += offsetY;

                /* также impulse выставить в 0, чтобы создание начало падать */
                if (creature->impulse.y < EPSILON)
                    creature->impulse.y = 0.0f;
            }
        }
    }
    if (yPos < LEVEL_HEIGHT - 1)
    {
        levelObject = level [yPos + 1][xPos];
        if ((levelObject != NULL) && (levelObject->solid))
        {
            /* снизу */
            if (levelObject->pos.y < (creature->pos.y + creature->h))
            {
                float offsetY = (creature->pos.y + creature->h) - levelObject->pos.y;
                creature->pos.y -= offsetY;
            }
        }
    }

    /* застрял прямо здесь и сейчас?! */
    levelObject = level [yPos][xPos];
    if ((levelObject != NULL) && (levelObject->solid))
    {
        float offsetX = levelObject->center.x - creature->center.x;
        float offsetY = levelObject->center.y - creature->center.y;

        if (abs (offsetX) > abs (offsetY))
        {
            if (creature->center.x < levelObject->center.x)
                creature->pos.x = levelObject->pos.x - creature->w;
            else
                creature->pos.x = levelObject->pos.x + BLOCK_SIZE;
        }
        else
        {
            if (creature->center.y < levelObject->center.y)
                creature->pos.y = levelObject->pos.y - creature->h;
            else
                creature->pos.y = levelObject->pos.y + BLOCK_SIZE;
        }
    }
}

void CreatureUpdateAI (SCreature* creature)
{
    if (creature == NULL)
        return;

    /* random direction */
    if (creature->xDir == 0)
    {
        creature->xDir = rand() % 2;
        if (creature->xDir == 0)
            creature->xDir = -1;
    }

    /* change direction if place in level not free */
    /*int xPos = ((int)creature->center.x) / BLOCK_SIZE;
    int yPos = ((int)creature->center.y) / BLOCK_SIZE;

    SLevelObject* levelObject = level [yPos][xPos + creature->xDir];
    if (levelObject != NULL && levelObject->solid)
    {
        creature->impulse.x = 0.0f;
        creature->xDir = -creature->xDir;
    }*/

    /* meet creature */
    /*int i;
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        SCreature* testCreature = creatures[i];
        if ((testCreature != NULL) && (creature != testCreature))
        {
            if (CreatureIsCollisionCreature (creature, testCreature))
            {
                //* change direction /
                creature->impulse.x = 0.0f;
                creature->xDir = -creature->xDir;

                if (creature->center.x < testCreature->center.x)
                {
                    creature->pos.x = testCreature->pos.x - creature->w;
                    testCreature->pos.x = creature->pos.x + creature->w;
                }
                else
                {
                    creature->pos.x = testCreature->pos.x + creature->w;
                    testCreature->pos.x = creature->pos.x - testCreature->w;
                }
            }
        }
    }*/

    /* move... */
    if (abs (creature->impulse.x) < EPSILON)
        CreatureAddImpulse (creature, creature->xDir, 0.0f);
    CreatureAddImpulse (creature, (creature->accelSpeed)*(creature->xDir)*deltaTime, 0.0f);
}

void CreatureSetFrameRange (SCreature* creature, int startFrame, int endFrame)
{
    if (creature == NULL)
        return;

    creature->startFrame = startFrame;
    creature->endFrame = endFrame;
}

void CreatureUpdateAnimation (SCreature* creature)
{
    if (creature == NULL)
        return;

    /* всяческие проверки диапазона анимации */
    if (creature->startFrame > creature->endFrame)
        Swapi (&(creature->startFrame), &(creature->endFrame));

    if (creature->startFrame < 0)
        creature->startFrame = 0;

    if (creature->endFrame > creature->texCount - 1)
        creature->endFrame = creature->texCount - 1;

    /* кадр один */
    if (creature->startFrame == creature->endFrame)
    {
        if (creature->curFrame != creature->startFrame)
            creature->curFrame = creature->startFrame;
        return;
    }


    creature->animDelay += deltaTime;
    if (creature->animDelay >= creature->animSpeed)
    {
        creature->animDelay -= creature->animSpeed;
        creature->curFrame++;
    }

    if (creature->curFrame > creature->endFrame)
        creature->curFrame = creature->startFrame;
}

void CreatureGetSdlRect (SCreature* creature, SDL_Rect* rect)
{
    if (creature != NULL)
    {
        rect->x = (int)(creature->pos.x);
        rect->y = (int)(creature->pos.y);
        rect->w = (int)(creature->w);
        rect->h = (int)(creature->h);
    }
}

SDL_Texture* CreatureGetTexture (SCreature* creature, int numFrame)
{
    SDL_Texture** p = (SDL_Texture**) creature->textures;
    p += numFrame;
    return *p;
}
