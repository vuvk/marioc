#include <stdlib.h>
#include <string.h>

#include "libxml/parser.h"

#include "additions.h"
#include "engine.h"
#include "level.h"
#include "texture.h"
#include "corpse.h"
#include "lump.h"
#include "surprise.h"
#include "player.h"


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
            if ((level[r][c]) != NULL)
                LevelObjectDestroy (&(level[r][c]));
        }
    }
}

/* парсинг и поиск объектов */
void XmlParsing (xmlNode* start_node, int lvl)
{
    if (start_node)
    {
        xmlNode* node;
        for (node = start_node; node; node = node->next)
        {
            if (strcmp ((char*) node->name, "text") != 0)
            {
                // level objects
                if (strcmp ((char*) node->name, "data") == 0)
                {
                    // номера тайлов представленные в формате csv
                    char* csv = (char*) xmlNodeGetContent(node);
                    //printf("%s", csv);

                    uint16 numberOfBlock = 0;
                    char* pch = strtok (csv, ",.-; ");
                    int textureIndex;
                    while (pch != NULL)
                    {
                        textureIndex = atoi (pch) - 1;
                        if (textureIndex > 0)
                        {
                            float x = (numberOfBlock % LEVEL_WIDTH) * BLOCK_SIZE;
                            float y = (numberOfBlock / LEVEL_WIDTH) * BLOCK_SIZE;

                            SLevelObject* levelObject;

                            switch (textureIndex)
                            {
                                // solid
                                case 1:
                                case 2:
                                case 3:

                                // труба
                                case 41:
                                case 42:
                                case 60:
                                case 61:
                                {
                                    levelObject = LevelObjectCreate (lotBlock, x, y, true, true, textureIndex);
                                    break;
                                }

                                // brick
                                case 48:
                                {
                                    levelObject = LevelObjectCreate (lotBrick, x, y, true, false, textureIndex);
                                    break;
                                }

                                // mushroom block
                                case 132:
                                {
                                    levelObject = LevelObjectCreate (lotMushroomBox, x, y, true, false, textureIndex);
                                    break;
                                }


                                default :
                                {
                                    levelObject = LevelObjectCreate (lotNone, x, y, false, true, textureIndex);
                                }
                            }

                            level[numberOfBlock / LEVEL_WIDTH][numberOfBlock % LEVEL_WIDTH] = levelObject;
                        }

                        pch = strtok (NULL, ",.-; ");
                        numberOfBlock++;
                    }

                    free (csv);
                    free (pch);
                    csv = NULL;
                    pch = NULL;
                }

                // creatures or another objects
                if (strcmp ((char*) node->name, "object") == 0)
                {
                    char* name = (char*) xmlGetProp (node, (xmlChar*) "name");
                    char* xStr = (char*) xmlGetProp (node, (xmlChar*) "x");
                    char* yStr = (char*) xmlGetProp (node, (xmlChar*) "y");

                    float x = atof (xStr) * (BLOCK_SIZE / TILE_SIZE);
                    float y = atof (yStr) * (BLOCK_SIZE / TILE_SIZE);

                    // PLAYER
                    if (strcmp (name, "player") == 0)
                    {
                        SCreature* creature = CreatureCreate (ctPlayer,                   // type
                                                              1,                          // health
                                                              x, y,                       // position
                                                              BLOCK_SIZE, BLOCK_SIZE,     // size
                                                              BLOCK_SIZE / 5.0f,          // movement speed
                                                              &playerTextures[0], 13,     // textures and number of textures
                                                              0.1f);                      // speed of animation
                        // link for player
                        player = creature;
                        ListAddElement (creatures, creature);

                        playerCanDamaged = true;
                        playerPrevHealth = 1;
                        TextureArraySetColor (player->textures, 255, 255, 255, player->texCount);
                    }

                    // GOOMBA
                    if (strcmp (name, "goomba") == 0)
                    {
                        SCreature* creature = CreatureCreate (ctGoomba,                   // type
                                                              1,                          // health
                                                              x, y,                       // position
                                                              BLOCK_SIZE, BLOCK_SIZE,     // size
                                                              2.5f,                       // movement speed
                                                              &goombaTextures[0], 2,      // textures and number of textures
                                                              0.1f);                      // speed of animation
                        ListAddElement (creatures, creature);
                    }

                    // free shit
                    free (name);
                    free (xStr);
                    free (yStr);
                    name = NULL;
                    xStr = NULL;
                    yStr = NULL;
                }

                // search all children node, if exists
                if (node->children)
                    XmlParsing(node->children, lvl + 1);
            }
        }

        xmlFreeNode (node);
        node = NULL;
    }
}

void LevelLoad (char* fileName)
{
    LevelClear();

    if (!FileExists (fileName))
    {
        printf ("File '%s' doesn't exists!\n", fileName);
        return;
    }

    // start parsing xml-level
    xmlDoc* document;
    xmlNode* root;

    document = xmlReadFile (fileName, NULL, 0);
    root = xmlDocGetRootElement (document);

    XmlParsing (root->children, 1);

    //xmlFreeNode (root);
    xmlFreeDoc (document);
    printf ("Level loaded!\n");

    root = NULL;
    document = NULL;
}

void LevelUpdateAndRender ()
{
    uint16 r, c;
    SDL_Rect srcRect, dstRect;
    SLevelObject* levelObject;

    /* draw background first */
    dstRect.x = dstRect.y = srcRect.x = srcRect.y = 0;
    srcRect.w = srcRect.h = TILE_SIZE;
    dstRect.w = WINDOW_WIDTH;
    dstRect.h = WINDOW_HEIGHT;
    EngineRenderTile (levelTextures, &srcRect, &dstRect);

    /* ограничиваем обрабатываемые блоки видимостью камеры */
    int16 viewColMin = (int16)(cameraPos.x) / BLOCK_SIZE;
    int16 viewColMax = viewColMin + WINDOW_WIDTH / BLOCK_SIZE;
    LimitShort (&viewColMin, 0, LEVEL_WIDTH - 1);
    LimitShort (&viewColMax, 0, LEVEL_WIDTH - 1);

    int16 viewRowMin = (int16)(cameraPos.y) / BLOCK_SIZE;
    int16 viewRowMax = viewRowMin + WINDOW_HEIGHT / BLOCK_SIZE;
    LimitShort (&viewRowMin, 0, LEVEL_HEIGHT - 1);
    LimitShort (&viewRowMax, 0, LEVEL_HEIGHT - 1);

    dstRect.w = BLOCK_SIZE;
    dstRect.h = BLOCK_SIZE;

    for (r = viewRowMin; r <= viewRowMax; r ++)
    {
        for (c = viewColMin; c <= viewColMax; c++)
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

                dstRect.x = (int)(levelObject->pos.x - cameraPos.x);
                dstRect.y = (int)(levelObject->pos.y - cameraPos.y);

                /* if rect in screen range */
                if ((dstRect.x + dstRect.w) > 0 &&
                    (dstRect.x <= WINDOW_WIDTH))
                {
                    srcRect.x = (levelObject->texIndex % COUNT_TILES_HORIZONTAL) * TILE_SIZE;
                    srcRect.y = (levelObject->texIndex / COUNT_TILES_HORIZONTAL) * TILE_SIZE;

                    EngineRenderTile (levelTextures, &srcRect, &dstRect);
                }
            }
        }
    }
}
