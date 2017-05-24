/*
*   ���������� ��������
*   ������ �������� ����, �� �������
*   ������������ ���������� ����
*/

#pragma once

#include "list.h"
#include "level.h"

#define PHYSOBJ_NO_COLLISION         0
#define PHYSOBJ_COLLISION_WITH_LEVEL 1
#define PHYSOBJ_COLLISION_WITH_ALL   2

typedef struct
{
    /* ������� � ������� ������ */
    SVector2f pos;
    SVector2f center;
    /* ������� */
    ubyte w, h;
    ubyte halfW, halfH;

    /* �������� */
    SVector2f impulse;

    /* � ��� ����� ������������ */
    ubyte collisionFlag;

    float friction;
    bool isGrounded;
} SPhysObject;

/* ����� ���� ���������� ��� � ���� */
SList* physObjects;

SPhysObject* PhysObjectCreate (float x, float y, ubyte w, ubyte h, ubyte collisionFlag);
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
