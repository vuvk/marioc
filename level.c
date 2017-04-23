#include "level.h"
#include "texture.h"
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
    "#                  #",
    "#        #     #   #",
    "#              #   #",
    "#          ###     #",
    "#     ####         #",
    "#                  #",
    "####################"
};

SLevelObject* LevelObjectCreate (ELevelObjectType levelObjectType,
                                 float x, float y,
                                 bool solid,
                                 int texIndex)
{
    SLevelObject* levelObject = (SLevelObject*) malloc(sizeof(SLevelObject));
    levelObject->pos.x = x;
    levelObject->pos.y = y;
    levelObject->center.x = levelObject->pos.x + BLOCK_SIZE / 2;
    levelObject->center.y = levelObject->pos.y + BLOCK_SIZE / 2;
    levelObject->levelObjectType = levelObjectType;
    levelObject->solid = solid;
    levelObject->texIndex = texIndex;

    return levelObject;
}

void LevelObjectDestroy (SLevelObject* levelObject)
{
    if (levelObject != NULL)
    {
        free(levelObject);
        levelObject = NULL;
    }
}

void LevelClear ()
{
    int r, c;
    for (r = 0; r < LEVEL_HEIGHT; r++)
    {
        for (c = 0; c < LEVEL_WIDTH; c++)
        {
            LevelObjectDestroy (level[r][c]);
        }
    }
}

void LevelLoad ()
{
    int r, c;

    CreatureClearAll();
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
                            creatures[creaturesCount] = CreatureCreate (ctPlayer,           /* type */
                                                                        1,                  /* health */
                                                                        c * BLOCK_SIZE, r * BLOCK_SIZE,  /* position */
                                                                        BLOCK_SIZE, BLOCK_SIZE,     /* size */
                                                                        5.0,                    /* movement speed */
                                                                        &playerTextures[0], 6,  /* textures and number of textures */
                                                                        0.1f);              /* speed of animation */
                            /* link for player */
                            player = creatures [creaturesCount];
                            break;
                        }

                        /* goomba */
                        case 'g' :
                        {
                            creatures[creaturesCount] = CreatureCreate (ctGoomba,               /* type */
                                                                        1,                      /* health */
                                                                        c * BLOCK_SIZE, r * BLOCK_SIZE,  /* position */
                                                                        BLOCK_SIZE, BLOCK_SIZE,          /* size */
                                                                        2.5,                    /* movement speed */
                                                                        &goombaTextures[0], 2,  /* textures and number of textures */
                                                                        0.1f);                  /* speed of animation */
                            break;
                        }
                    }

                    creaturesCount++;
                }

                /* solid block */
                case '#' :
                {
                    level [r][c] = LevelObjectCreate (lotBlock, c * BLOCK_SIZE, r * BLOCK_SIZE, true, 1);
                    break;
                }

                default :
                {
                    if (level [r][c] != NULL)
                    {
                        free (level[r][c]);
                        level [r][c] = NULL;

                        break;
                    }
                }
            }
        }
    }
}
