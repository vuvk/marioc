#include <time.h>

#include "engine.h"
#include "texture.h"
#include "level.h"


/* время в начале и в конце кадра */
unsigned long int startTick, endTick;
/* счетчик кадров и FPS */
int fpsCounter = 0;
float timeDelay = 0.0f;

SDL_Window* sdlWindow;
SDL_Renderer* sdlRenderer;

bool EngineStart()
{
    startTick = endTick = SDL_GetPerformanceCounter();
    deltaTime = 0.0f;
    fps = 0;

    if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
    {
        printf ("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void EngineStop()
{
    if (sdlWindow)
        SDL_DestroyWindow (sdlWindow);
    if (sdlRenderer)
        SDL_DestroyRenderer (sdlRenderer);

    SDL_Quit();

    printf ("SDL stopped.\n");
}

bool EngineCreateWindow (const char* title, int x, int y, int w, int h, unsigned long flags)
{
    sdlWindow = SDL_CreateWindow (title, x, y, w, h, flags);
    return (sdlWindow != NULL);
}

void EngineSetWindowTitle (const char* title)
{
    SDL_SetWindowTitle (sdlWindow, title);
}

bool EngineCreateRenderer (int index, unsigned long flags)
{
    if (sdlWindow == NULL)
        return false;

    sdlRenderer = SDL_CreateRenderer(sdlWindow, index, flags);
    return (sdlRenderer != NULL);
}

void EngineRenderClear()
{
    SDL_RenderClear(sdlRenderer);
}


void EngineRenderLevel()
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
                rect.x = c * BLOCK_SIZE;
                rect.y = r * BLOCK_SIZE;

                SDL_RenderCopy (sdlRenderer, levelTextures [levelObject->texIndex], NULL, &rect);
            }
        }
    }
}

void EngineRenderImage(SDL_Texture* texture, SDL_Rect* rect, bool flipX)
{
    if (!flipX)
    {
        SDL_RenderCopy(sdlRenderer, texture, NULL, rect);
    }
    else
    {
        SDL_Point point;
        point.x = point.y = 0;
        SDL_RenderCopyEx(sdlRenderer, texture, NULL, rect, 0.0, &point, SDL_FLIP_HORIZONTAL);
    }
}

void EngineRenderPresent()
{
    SDL_RenderPresent(sdlRenderer);
}

void EngineUpdateTime()
{
    startTick = SDL_GetPerformanceCounter();
    deltaTime = (float)(startTick - endTick) / SDL_GetPerformanceFrequency();
    endTick = startTick;

    fpsCounter++;
    timeDelay += deltaTime;
    if (timeDelay >= 1.0f)
    {
        timeDelay -= 1.0f;
        fps = fpsCounter;
        fpsCounter = 0;
    }
}

SDL_Texture* EngineLoadTexture (const char* fileName)
{
    return IMG_LoadTexture(sdlRenderer, fileName);
}
