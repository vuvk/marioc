#include "engine.h"
#include "level.h"
#include "texture.h"
#include "corpse.h"
#include "lump.h"
#include "player.h"

/* временный массив уровня */
char levelData [LEVEL_HEIGHT][LEVEL_WIDTH] =
{
    "####################",
    "#                  #",
    "#                  #",
    "#      g           #",
    "#                  #",
    "#          g   g   #",
    "#   p              #",
    "#        #         #",
    "#     b            #",
    "#        b     #   #",
    "#              #   #",
    "# c        bbb     #",
    "#     b##b         #",
    "#            #     #",
    "##############   ###"
};

SLevelObject* LevelObjectCreate (ELevelObjectType levelObjectType,
                                 float x, float y,
                                 bool isSolid,
                                 bool isStatic,
                                 int texIndex)
{
    SLevelObject* levelObject = (SLevelObject*) malloc(sizeof(SLevelObject));
    levelObject->startPos.x = x;
    levelObject->startPos.y = y;
    levelObject->pos.x = x;
    levelObject->pos.y = y;
    levelObject->center.x = levelObject->pos.x + BLOCK_SIZE / 2;
    levelObject->center.y = levelObject->pos.y + BLOCK_SIZE / 2;
    levelObject->levelObjectType = levelObjectType;
    levelObject->isSolid = isSolid;
    levelObject->isStatic = isStatic;
    levelObject->texIndex = texIndex;

    return levelObject;
}

void LevelObjectDestroy (SLevelObject** levelObject)
{
    if ((levelObject != NULL) && (*levelObject != NULL))
    {
        free (*levelObject);
        *levelObject = NULL;
    }
}

void LevelClear ()
{
    int r, c;
    for (r = 0; r < LEVEL_HEIGHT; r++)
    {
        for (c = 0; c < LEVEL_WIDTH; c++)
        {
            LevelObjectDestroy (&(level[r][c]));
        }
    }
}

void LevelLoad ()
{
    register unsigned short r, c;

    CreatureClearAll();
    CorpseClearAll();
    LumpClearAll();
    PhysObjectClearAll();
    LevelClear();
    char symbol;

    for (r = 0; r < LEVEL_HEIGHT; r++)
    {
        for (c = 0; c < LEVEL_WIDTH; c++)
        {
            symbol = levelData [r][c];

            switch (symbol)
            {
                /* anybody */
                case 'p' :
                case 'g' :
                {
                    /* creature creating */
                    switch (symbol)
                    {
                        /* player */
                        case 'p' :
                        {
                            creatures[creaturesCount] = CreatureCreate (ctPlayer,                        /* type */
                                                                        1,                               /* health */
                                                                        c * BLOCK_SIZE, r * BLOCK_SIZE,  /* position */
                                                                        BLOCK_SIZE, BLOCK_SIZE,          /* size */
                                                                        5.0,                             /* movement speed */
                                                                        &playerTextures[0], 6,           /* textures and number of textures */
                                                                        0.1f);                           /* speed of animation */
                            /* link for player */
                            player = creatures [creaturesCount];
                            break;
                        }

                        /* goomba */
                        case 'g' :
                        {
                            creatures[creaturesCount] = CreatureCreate (ctGoomba,                        /* type */
                                                                        1,                               /* health */
                                                                        c * BLOCK_SIZE, r * BLOCK_SIZE,  /* position */
                                                                        BLOCK_SIZE - 2, BLOCK_SIZE - 2,  /* size */
                                                                        2.5,                             /* movement speed */
                                                                        &goombaTextures[0], 2,           /* textures and number of textures */
                                                                        0.1f);                           /* speed of animation */
                            break;
                        }
                    }

                    creaturesCount++;
                    break;
                }

                /* solid block */
                case '#' :
                {
                    level [r][c] = LevelObjectCreate (lotBlock, c * BLOCK_SIZE, r * BLOCK_SIZE, true, true, 1);
                    break;
                }

                /* red brick */
                case 'b' :
                {
                    level [r][c] = LevelObjectCreate (lotBrick, c * BLOCK_SIZE, r * BLOCK_SIZE, true, false, 2);
                    break;
                }

                /* surprise block */
                case 'c' :
                {
                    level [r][c] = LevelObjectCreate (lotCoinBox, c * BLOCK_SIZE, r * BLOCK_SIZE, true, false, 3);
                    break;
                }

                default :
                {
                    if (level [r][c] != NULL)
                    {
                        free (level[r][c]);
                        level [r][c] = NULL;
                    }

                    break;
                }
            }
        }
    }
}

void LevelUpdateAndRender ()
{
    int r, c;
    SDL_Rect rect;
    SLevelObject* levelObject;
    rect.x = 0;
    rect.y = 0;
    rect.w = BLOCK_SIZE;
    rect.h = BLOCK_SIZE;

    for (r = 0; r < LEVEL_HEIGHT; r ++)
    {
        for (c = 0; c < LEVEL_WIDTH; c ++)
        {
            levelObject = level[r][c];

            if (levelObject != NULL)
            {
                // update levelObject position...
                levelObject->center.x = levelObject->pos.x + (BLOCK_SIZE >> 1);
                levelObject->center.y = levelObject->pos.y + (BLOCK_SIZE >> 1);

                float distY = levelObject->pos.y - levelObject->startPos.y;
                if (abs (distY) > EPSILON)
                {
                    if (levelObject->pos.y < levelObject->startPos.y)
                        levelObject->pos.y += deltaTime*BLOCK_SIZE;

                    if (levelObject->pos.y > levelObject->startPos.y)
                        levelObject->pos.y -= deltaTime*BLOCK_SIZE;
                }
                else
                    if (distY != 0.0f)
                        levelObject->pos.y = levelObject->startPos.y;

                rect.x = (int)(levelObject->pos.x);
                rect.y = (int)(levelObject->pos.y);

                //SDL_RenderCopy (sdlRenderer, levelTextures [levelObject->texIndex], NULL, &rect);
                EngineRenderImage (levelTextures [levelObject->texIndex], &rect, false);
            }
        }
    }
}
