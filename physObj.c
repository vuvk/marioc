#include "defines.h"
#include "additions.h"
#include "physObj.h"

SPhysObject* PhysObjectCreate (float x, float y, ubyte w, ubyte h, ubyte collisionFlag)
{
    SPhysObject* physObject = (SPhysObject*) malloc (sizeof(SPhysObject));
    physObject->pos.x = x;
    physObject->pos.y = y;
    physObject->w = w;
    physObject->h = h;
    physObject->halfW = w >> 1;
    physObject->halfH = h >> 1;
    physObject->center.x = x + physObject->halfW;
    physObject->center.y = y + physObject->halfH;
    physObject->collisionFlag = collisionFlag;
    physObject->isGrounded = false;
    physObject->friction = 5.0f;

    return physObject;
}

void PhysObjectDestroy (SPhysObject** physObject)
{
    if ((physObject == NULL) || (*physObject == NULL))
        return;

    free(*physObject);
    *physObject = NULL;
}

void PhysObjectClearAll ()
{
    register unsigned short i;
    for (i = 0; i < MAX_PHYSOBJECTS_COUNT; i++)
    {
        if (physObjects[i] != NULL)
            PhysObjectDestroy(&(physObjects[i]));
    }
}

void PhysObjectUpdateState (SPhysObject* physObject)
{
    if (physObject == NULL)
        return;

    physObject->halfW = physObject->w >> 1;
    physObject->halfH = physObject->h >> 1;

    physObject->center.x = physObject->pos.x + physObject->halfW;
    physObject->center.y = physObject->pos.y + physObject->halfH;
}

void PhysObjectAddImpulse (SPhysObject* physObject, float x, float y)
{
    if (physObject == NULL)
        return;

    physObject->impulse.x += x;
    physObject->impulse.y += y;
}

void PhysObjectUpdatePhysics (SPhysObject* physObject)
{
    if (physObject == NULL)
        return;

    /* limits */
    if (physObject->impulse.x < MIN_HORIZONTAL_IMPULSE)
        physObject->impulse.x = MIN_HORIZONTAL_IMPULSE;
    if (physObject->impulse.x > MAX_HORIZONTAL_IMPULSE)
        physObject->impulse.x = MAX_HORIZONTAL_IMPULSE;
    if (physObject->impulse.y < MIN_VERTICAL_IMPULSE)
        physObject->impulse.y = MIN_VERTICAL_IMPULSE;
    if (physObject->impulse.y > MAX_VERTICAL_IMPULSE)
        physObject->impulse.y = MAX_VERTICAL_IMPULSE;


    /*****************/
    /* VERTICAL MOVE */
    /*****************/

    /* check gravity */
    physObject->isGrounded = false;

    if (physObject->impulse.y <= GRAVITY)
        physObject->impulse.y += GRAVITY*deltaTime;
    else
        physObject->impulse.y = GRAVITY;


    /*******************/
    /* HORIZONTAL MOVE */
    /*******************/

    /* friction */
    if (abs (physObject->impulse.x) > EPSILON)
    {
        if (physObject->impulse.x > EPSILON)
            physObject->impulse.x -= (physObject->friction)*deltaTime;

        if (physObject->impulse.x < EPSILON)
            physObject->impulse.x += (physObject->friction)*deltaTime;
    }
    else
    {
        physObject->impulse.x = 0.0f;
    }


    /* apply impulse */
    physObject->pos = AddVector2f (physObject->pos, physObject->impulse);


    /* previously checking */
    /* если объект может с чем-то сталкиваться */
    if (physObject->collisionFlag > PHYSOBJ_NO_COLLISION)
    {
        SLevelObject* levelObject;

        /* вычисляем, какие блоки может задеть объект */
        short xStart = ((short)(physObject->pos.x)) / BLOCK_SIZE;
        short xEnd   = ((short)(physObject->pos.x + physObject->w)) / BLOCK_SIZE;
        short yStart = ((short)(physObject->pos.y)) / BLOCK_SIZE;
        short yEnd   = ((short)(physObject->pos.y + physObject->h)) / BLOCK_SIZE;

        /* ограничиваем номера ячеек размерами уровня */
        LimitShort (&xStart, 0, LEVEL_WIDTH - 1);
        LimitShort (&xEnd,   0, LEVEL_WIDTH - 1);
        LimitShort (&yStart, 0, LEVEL_HEIGHT - 1);
        LimitShort (&yEnd,   0, LEVEL_HEIGHT - 1);

        /* полблока */
        float halfW = BLOCK_SIZE >> 1;
        float halfH = halfW;

        /* минимальное расстояние между объектом и блоком = сумма половины их размеров */
        float minDistX = physObject->halfW + halfW;
        float minDistY = physObject->halfH + halfH;

        register unsigned short i;
        register unsigned short j;
        for (j = yStart; j <= yEnd; j++)
            for (i = xStart; i <= xEnd; i++)
            {
                levelObject = level[j][i];
                if (levelObject != NULL && levelObject->isSolid)
                {
                    /* высчитываем погружение объекта в блок */
                    float xOffset = abs(physObject->center.x - levelObject->center.x);
                    float yOffset = abs(physObject->center.y - levelObject->center.y);

                    /* если расстояние между центрами меньше минимально возможного - было пересечение! */
                    if ((xOffset <= minDistX) &&
                        (yOffset <= minDistY))
                    {
                        /* если погрузился по горизонтали больше, чем по вертикали */
                        if (xOffset > yOffset)
                        {
                            /* блок справа */
                            if (physObject->center.x < levelObject->center.x)
                            if (yOffset <= halfH + (physObject->halfH >> 1))
                                physObject->pos.x = levelObject->pos.x - physObject->w;

                            /* блок слева */
                            if (physObject->center.x > levelObject->center.x)
                            if (yOffset <= halfH + (physObject->halfH >> 1))
                                physObject->pos.x = levelObject->pos.x + BLOCK_SIZE;

                            physObject->impulse.x = 0.0f;
                        }
                        /* погрузился по вертикали больше, чем по горизонтали */
                        else
                        {
                            /* блок сверху */
                            if (physObject->center.y > levelObject->center.y)
                            if (xOffset <= halfW + (physObject->halfW >> 1))
                            {
                                physObject->pos.y = levelObject->pos.y + BLOCK_SIZE;
                                physObject->impulse.y = 0.0f;
                            }

                            /* блок снизу */
                            if (physObject->center.y < levelObject->center.y)
                            if (xOffset <= halfW + (physObject->halfW >> 1))
                            {
                                physObject->pos.y = levelObject->pos.y - physObject->h;
                                physObject->impulse.y = 0.0f;

                                physObject->isGrounded = true;
                            }
                        }
                    }
                }
            }

        /* проверка столкновения с другими физическими объектами */
        if (physObject->collisionFlag == PHYSOBJ_COLLISION_WITH_ALL)
        {
            SPhysObject* anotherPhysObject;
            for (i = 0; i < MAX_PHYSOBJECTS_COUNT; i++)
            {
                anotherPhysObject = physObjects[i];
                if ((anotherPhysObject != NULL) &&
                    (anotherPhysObject != physObject) &&
                    (anotherPhysObject->collisionFlag == PHYSOBJ_COLLISION_WITH_ALL))
                {
                    /* минимальное расстояние между объектaми = сумма половины их размеров */
                    minDistX = physObject->halfW + anotherPhysObject->halfW;
                    minDistY = physObject->halfH + anotherPhysObject->halfH;

                    /* высчитываем погружение объекта в объект */
                    float xOffset = abs(physObject->center.x - anotherPhysObject->center.x);
                    float yOffset = abs(physObject->center.y - anotherPhysObject->center.y);

                    /* если расстояние между центрами меньше минимально возможного - было пересечение! */
                    if ((xOffset <= minDistX) &&
                        (yOffset <= minDistY))
                    {
                        /* если погрузился по горизонтали больше, чем по вертикали */
                        if (xOffset > yOffset)
                        {
                            if (physObject->impulse.x == 0.0f)
                                continue;

                            /* другой справа */
                            if (physObject->center.x < anotherPhysObject->center.x)
                            {
                                physObject->pos.x = anotherPhysObject->pos.x - physObject->w - 1.0f;
                                physObject->pos.x -= physObject->impulse.x;
                            }

                            /* другой слева */
                            if (physObject->center.x > anotherPhysObject->center.x)
                            {
                                physObject->pos.x = anotherPhysObject->pos.x + anotherPhysObject->w + 1.0f;
                                physObject->pos.x -= physObject->impulse.x;
                            }

                            physObject->impulse.x = 0.0f;
                        }
                        /* погрузился по вертикали больше, чем по горизонтали */
                        else
                        {
                            if (physObject->impulse.y == 0.0f)
                                continue;

                            /* другой снизу */
                            if (physObject->center.y < anotherPhysObject->center.y)
                            {
                                physObject->pos.y = anotherPhysObject->pos.y - physObject->h - 1.0f;
                                physObject->pos.y -= physObject->impulse.y;
                            }

                            /* другой сверху */
                            if (physObject->center.y > anotherPhysObject->center.y)
                            {
                                physObject->pos.y = anotherPhysObject->pos.y + anotherPhysObject->h + 1.0f;
                                physObject->pos.y -= physObject->impulse.y;
                            }

                            physObject->impulse.y = 0.0f;
                        }
                    }
                }
            }
        }
    }
}

bool PhysObjectContainsPoint (SPhysObject* physObject, float x, float y)
{
    if (physObject == NULL)
        return false;

    return ((x >= physObject->pos.x) && (x <= physObject->pos.x + physObject->w) &&
            (y >= physObject->pos.y) && (y <= physObject->pos.y + physObject->h));
}

bool IsPlaceFree (float x, float y,
                  bool checkAll,
                  SLevelObject** obstacleLevelObject,
                  SPhysObject** obstaclePhysObject)
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
        if (obstacleLevelObject != NULL)
            *obstacleLevelObject = levelObject;
        return false;
    }

    /* check another physObjects... */
    if (checkAll)
    {
        SPhysObject* physObject;
        register unsigned short int i;
        for (i = 0; i < MAX_PHYSOBJECTS_COUNT; i++)
        {
            physObject = physObjects[i];
            if (physObject != NULL)
            {
                if (physObject->collisionFlag < PHYSOBJ_COLLISION_WITH_ALL)
                    return true;

                if (PhysObjectContainsPoint(physObject, x, y))
                {
                    if (obstaclePhysObject != NULL)
                        *obstaclePhysObject = physObject;
                    return false;
                }
            }
        }
    }

    /* nobody here ;) */
    return true;
}

bool PhysObjectIsCollisionPhysObject (SPhysObject* o1, SPhysObject* o2)
{
    if (o1 == NULL || o2 == NULL)
        return false;

    /* минимальное возможное расстояние между центрами */
    float distX = o1->halfW + o2->halfW;
    float distY = o1->halfH + o2->halfH;

    /* есть столкновение по X или Y */
    bool isCollX = false;
    bool isCollY = false;

    /* если расстояние между центрами меньше возможного, то столкновение точно есть */
    if (abs(o1->center.x - o2->center.x) < distX)
        isCollX = true;
    if (abs(o1->center.y - o2->center.y) < distY)
        isCollY = true;

    return (isCollX && isCollY);
}

bool PhysObjectIsCollisionLevelObject (SPhysObject* o1, SLevelObject* l2)
{
    if (o1 == NULL || l2 == NULL)
        return false;

    /* минимальное возможное расстояние между центрами */
    float distX = o1->halfW + (BLOCK_SIZE >> 1);
    float distY = o1->halfH + (BLOCK_SIZE >> 1);

    /* есть столкновение по X или Y */
    bool isCollX = false;
    bool isCollY = false;

    /* если расстояние между центрами меньше возможного, то столкновение точно есть */
    if (abs (o1->center.x - l2->center.x) < distX)
        isCollX = true;
    if (abs (o1->center.y - l2->center.y) < distY)
        isCollY = true;

    return (isCollX && isCollY);
}

void PhysObjectsUpdate()
{
    register unsigned short i;
    SPhysObject* physObject;
    for (i = 0; i < MAX_PHYSOBJECTS_COUNT; i++)
    {
        physObject = physObjects[i];

        if (physObject != NULL)
        {
            PhysObjectUpdateState(physObject);
            PhysObjectUpdatePhysics(physObject);

            /* check edges of level */
            short xPos = (short)physObject->center.x / BLOCK_SIZE;
            short yPos = (short)physObject->center.y / BLOCK_SIZE;
            if (xPos < 0 || xPos > LEVEL_WIDTH ||
                yPos < 0 || yPos > LEVEL_HEIGHT)
            {
                PhysObjectDestroy (&(physObjects[i]));
            }
        }
    }
}
