#include <math.h>
#include <stdlib.h>


#include "defines.h"
#include "additions.h"
#include "physObj.h"

SPhysObject* PhysObjectCreate (float x, float y, ubyte w, ubyte h, ubyte collisionFlag)
{
    SPhysObject* physObject = (SPhysObject*) malloc (sizeof(SPhysObject));
    physObject->pos.x = x;
    physObject->pos.y = y;
    physObject->impulse.x = 0.0f;
    physObject->impulse.y = 0.0f;
    physObject->w = w;
    physObject->h = h;
    physObject->halfW = w >> 1;
    physObject->halfH = h >> 1;
    physObject->center.x = x + physObject->halfW;
    physObject->center.y = y + physObject->halfH;
    physObject->collisionFlag = collisionFlag;
    physObject->isGrounded = false;
    physObject->friction = BLOCK_SIZE / 6.0f;

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
    ListClear (physObjects);
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
    /* ���� ������ ����� � ���-�� ������������ */
    if (physObject->collisionFlag > PHYSOBJ_NO_COLLISION)
    {
        SLevelObject* levelObject;

        /* ���������, ����� ����� ����� ������ ������ */
        short xStart = ((short)(physObject->pos.x)) / BLOCK_SIZE;
        short xEnd   = ((short)(physObject->pos.x + physObject->w)) / BLOCK_SIZE;
        short yStart = ((short)(physObject->pos.y)) / BLOCK_SIZE;
        short yEnd   = ((short)(physObject->pos.y + physObject->h)) / BLOCK_SIZE;

        /* ������������ ������ ����� ��������� ������ */
        LimitShort (&xStart, 0, LEVEL_WIDTH - 1);
        LimitShort (&xEnd,   0, LEVEL_WIDTH - 1);
        LimitShort (&yStart, 0, LEVEL_HEIGHT - 1);
        LimitShort (&yEnd,   0, LEVEL_HEIGHT - 1);

        /* �������� */
        float halfW = BLOCK_SIZE >> 1;
        float halfH = halfW;

        /* ����������� ���������� ����� �������� � ������ = ����� �������� �� �������� */
        float minDistX = physObject->halfW + halfW;
        float minDistY = physObject->halfH + halfH;

        uint16 i, j;
        for (j = yStart; j <= yEnd; j++)
            for (i = xStart; i <= xEnd; i++)
            {
                levelObject = level[j][i];
                if (levelObject != NULL && levelObject->isSolid)
                {
                    /* ����������� ���������� ������� � ���� */
                    float xOffset = abs(physObject->center.x - levelObject->center.x);
                    float yOffset = abs(physObject->center.y - levelObject->center.y);

                    /* ���� ���������� ����� �������� ������ ���������� ���������� - ���� �����������! */
                    if ((xOffset <= minDistX) &&
                        (yOffset <= minDistY))
                    {
                        /* ���� ���������� �� ����������� ������, ��� �� ��������� */
                        if (xOffset > yOffset)
                        {
                            /* ���� ������ */
                            if (physObject->center.x < levelObject->center.x)
                            if (yOffset <= halfH + (physObject->halfH >> 1))
                                physObject->pos.x = levelObject->pos.x - physObject->w;

                            /* ���� ����� */
                            if (physObject->center.x > levelObject->center.x)
                            if (yOffset <= halfH + (physObject->halfH >> 1))
                                physObject->pos.x = levelObject->pos.x + BLOCK_SIZE;

                            physObject->impulse.x = 0.0f;
                        }
                        /* ���������� �� ��������� ������, ��� �� ����������� */
                        else
                        {
                            /* ���� ������ */
                            if (physObject->center.y > levelObject->center.y)
                            if (xOffset <= halfW + (physObject->halfW >> 1))
                            {
                                physObject->pos.y = levelObject->pos.y + BLOCK_SIZE;
                                physObject->impulse.y = 0.0f;
                            }

                            /* ���� ����� */
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

        /* �������� ������������ � ������� ����������� ��������� */
        if (physObject->collisionFlag == PHYSOBJ_COLLISION_WITH_ALL)
        {
            SPhysObject* anotherPhysObject;
            for (SListElement* element = physObjects->first; element; element = element->next)
            {
                anotherPhysObject = (SPhysObject*) element->value;
                if ((anotherPhysObject != NULL) &&
                    (anotherPhysObject != physObject) &&
                    (anotherPhysObject->collisionFlag == PHYSOBJ_COLLISION_WITH_ALL))
                {
                    /* ����������� ���������� ����� ������a�� = ����� �������� �� �������� */
                    minDistX = physObject->halfW + anotherPhysObject->halfW;
                    minDistY = physObject->halfH + anotherPhysObject->halfH;

                    /* ����������� ���������� ������� � ������ */
                    float xOffset = abs(physObject->center.x - anotherPhysObject->center.x);
                    float yOffset = abs(physObject->center.y - anotherPhysObject->center.y);

                    /* ���� ���������� ����� �������� ������ ���������� ���������� - ���� �����������! */
                    if ((xOffset <= minDistX) &&
                        (yOffset <= minDistY))
                    {
                        /* ���� ���������� �� ����������� ������, ��� �� ��������� */
                        if (xOffset > yOffset)
                        {
                            if (physObject->impulse.x == 0.0f)
                                continue;

                            /* ������ ������ */
                            if (physObject->center.x < anotherPhysObject->center.x)
                            {
                                physObject->pos.x = anotherPhysObject->pos.x - physObject->w - 1.0f;
                                physObject->pos.x -= physObject->impulse.x;
                            }

                            /* ������ ����� */
                            if (physObject->center.x > anotherPhysObject->center.x)
                            {
                                physObject->pos.x = anotherPhysObject->pos.x + anotherPhysObject->w + 1.0f;
                                physObject->pos.x -= physObject->impulse.x;
                            }

                            physObject->impulse.x = 0.0f;
                        }
                        /* ���������� �� ��������� ������, ��� �� ����������� */
                        else
                        {
                            if (physObject->impulse.y == 0.0f)
                                continue;

                            /* ������ ����� */
                            if (physObject->center.y < anotherPhysObject->center.y)
                            {
                                physObject->pos.y = anotherPhysObject->pos.y - physObject->h - 1.0f;
                                physObject->pos.y -= physObject->impulse.y;
                            }

                            /* ������ ������ */
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
    if (xPos < 0 || xPos > (LEVEL_WIDTH - 1) ||
        yPos < 0 || yPos > (LEVEL_HEIGHT - 1))
        return false;

    SLevelObject* levelObject = level[yPos][xPos];
    if ((levelObject != NULL) && (levelObject->isSolid))
    {
        if (obstacleLevelObject != NULL)
            *obstacleLevelObject = levelObject;
        return false;
    }

    if (checkAll)
    {
        SPhysObject* physObject;
        for (SListElement* element = physObjects->first; element; element = element->next)
        {
            physObject = (SPhysObject*) element->value;

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

    /* ����������� ��������� ���������� ����� �������� */
    float distX = o1->halfW + o2->halfW;
    float distY = o1->halfH + o2->halfH;

    /* ���� ������������ �� X ��� Y */
    bool isCollX = false;
    bool isCollY = false;

    /* ���� ���������� ����� �������� ������ ����������, �� ������������ ����� ���� */
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

    /* ����������� ��������� ���������� ����� �������� */
    float distX = o1->halfW + (BLOCK_SIZE >> 1);
    float distY = o1->halfH + (BLOCK_SIZE >> 1);

    /* ���� ������������ �� X ��� Y */
    bool isCollX = false;
    bool isCollY = false;

    /* ���� ���������� ����� �������� ������ ����������, �� ������������ ����� ���� */
    if (abs (o1->center.x - l2->center.x) < distX)
        isCollX = true;
    if (abs (o1->center.y - l2->center.y) < distY)
        isCollY = true;

    return (isCollX && isCollY);
}

void PhysObjectsUpdate()
{
    SPhysObject* physObject;
    SListElement* element = physObjects->first;
    while (element != NULL)
    {
        physObject = (SPhysObject*) element->value;

        if (physObject != NULL)
        {
            // check edges of level
            short xPos = (short)physObject->center.x / BLOCK_SIZE;
            short yPos = (short)physObject->center.y / BLOCK_SIZE;
            if (xPos < 0 || xPos > (LEVEL_WIDTH - 1) ||
                yPos < 0 || yPos > (LEVEL_HEIGHT - 1))
            {
                SListElement* nextElement = element->next;
                ListDeleteElementByValue (physObjects, physObject);
                element = nextElement;

                continue;
            }

            PhysObjectUpdateState(physObject);
            PhysObjectUpdatePhysics(physObject);
        }

        element = element->next;
    }
}
