/*
*   ���������� ��������
*   ������ �������� ����, �� �������
*   ������������ ���������� ����
*/

#pragma once

#include "defines.h"
#include "level.h"


#define PHYSOBJ_NO_COLLISION 0
#define PHYSOBJ_COLLISION_WITH_LEVEL 1
#define PHYSOBJ_COLLISION_WITH_ALL 2

typedef struct
{
    /* ������� � ������� ������ */
    SVector2f pos;
    SVector2f center;
    /* ������� */
    byte w, h;
    byte halfW, halfH;

    /* �������� */
    SVector2f impulse;

    /* � ��� ����� ������������ */
    byte collisionFlag;

    float friction;
    bool isGrounded;
} SPhysObject;


SPhysObject* physObjects[MAX_PHYSOBJECTS_COUNT];


SPhysObject* PhysObjectCreate (float x, float y, byte w, byte h, byte collisionFlag);
void PhysObjectDestroy (SPhysObject** physObject);
void PhysObjectClearAll ();

void PhysObjectUpdateState (SPhysObject* physObject);

/* ��������� ������ � �������� */
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
