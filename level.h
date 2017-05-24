#pragma once


#include "defines.h"
#include "vector2d.h"


/* типы объектов уровня */
typedef enum
{
    lotNone              = 0,
    lotBrick             = 1,
    lotMetalBrick        = 2,
    lotBlock             = 3,
    lotCoinBox           = 4,
    lotMushroomBox       = 5,
    lotSurpriseBlockUsed = 6
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
void LevelLoad (char* fileName);
void LevelClear ();

void LevelUpdateAndRender ();
