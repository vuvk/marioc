#pragma once

#include <stdlib.h>

//#include "creature.h"
#include "defines.h"
#include "vector2d.h"

/* типы объектов уровня */
typedef enum
{
    lotBrick,
    lotMetalBrick,
    lotBlock,
    lotCoinBox,
    lotMushroomBox
} ELevelObjectType;

/* объект уровня */
typedef struct
{
    SVector2f pos;
    SVector2f center;
    ELevelObjectType levelObjectType;
    bool solid;
    int texIndex;
} SLevelObject;


/* массив из объектов */
SLevelObject* level [LEVEL_HEIGHT][LEVEL_WIDTH];


/* имитация конструктора */
SLevelObject* LevelObjectCreate (ELevelObjectType levelObjectType,
                                 float x, float y,
                                 bool solid,
                                 int texIndex);

/* имитация деструктора */
void LevelObjectDestroy (SLevelObject* levelObject);

/* загрузка уровня (наполнение массива level) */
void LevelLoad ();
void LevelClear ();

/* check place */
//bool IsPlaceFree (float x, float y, bool checkAll, void** obstacle);
//bool IsPlaceFree (float x, float y, bool checkAll, SLevelObject** obstacleLevelObject, SCreature** obstacleCreature);
