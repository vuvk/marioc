#pragma once

#include <stdlib.h>

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
    SVector2f startPos;                 /* позиция, в которой объект был создан */
    SVector2f pos;                      /* текущая позиция */
    SVector2f center;                   /* центр блока */
    ELevelObjectType levelObjectType;
    bool isSolid;                       /* является ли блок твёрдым */
    bool isStatic;                      /* является ли блок подвижным/разбиваемым */
    int texIndex;
} SLevelObject;

/* массив из объектов */
SLevelObject* level [LEVEL_HEIGHT][LEVEL_WIDTH];


/* имитация конструктора */
SLevelObject* LevelObjectCreate (ELevelObjectType levelObjectType,
                                 float x, float y,
                                 bool isSolid,
                                 bool isStatic,
                                 int texIndex);

/* имитация деструктора */
void LevelObjectDestroy (SLevelObject** levelObject);

/* загрузка уровня (наполнение массива level) */
void LevelLoad ();
void LevelClear ();

/*void LevelObjectsUpdate ();*/

/* check place */
/*bool IsPlaceFree (float x, float y, bool checkAll, void** obstacle);
bool IsPlaceFree (float x, float y, bool checkAll, SLevelObject** obstacleLevelObject, SCreature** obstacleCreature);*/
