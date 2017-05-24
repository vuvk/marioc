/*
*   физическая сущность
*   объект игрового мира, на который
*   воздействуют физические силы
*/

#pragma once

#include "list.h"
#include "level.h"

#define PHYSOBJ_NO_COLLISION         0
#define PHYSOBJ_COLLISION_WITH_LEVEL 1
#define PHYSOBJ_COLLISION_WITH_ALL   2

typedef struct
{
    /* позиция и позиция центра */
    SVector2f pos;
    SVector2f center;
    /* размеры */
    ubyte w, h;
    ubyte halfW, halfH;

    /* движение */
    SVector2f impulse;

    /* с чем может сталкиваться */
    ubyte collisionFlag;

    float friction;
    bool isGrounded;
} SPhysObject;

/* набор всех физических тел в игре */
SList* physObjects;

SPhysObject* PhysObjectCreate (float x, float y, ubyte w, ubyte h, ubyte collisionFlag);
void PhysObjectDestroy (SPhysObject** physObject);
void PhysObjectClearAll ();

void PhysObjectUpdateState (SPhysObject* physObject);

/* обработка физики и движения */
void PhysObjectAddImpulse (SPhysObject* physObject, float x, float y);
void PhysObjectUpdatePhysics (SPhysObject* physObject);
bool PhysObjectContainsPoint (SPhysObject* physObject, float x, float y);
bool PhysObjectIsCollisionPhysObject (SPhysObject* o1, SPhysObject* o2);
bool PhysObjectIsCollisionLevelObject (SPhysObject* o1, SLevelObject* l2);


void PhysObjectsUpdate();

bool IsPlaceFree (float x, float y,
                  bool checkAll,
                  SLevelObject** obstacleLevelObject,
                  SPhysObject** obstaclePhysObject);
