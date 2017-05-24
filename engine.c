#include <time.h>

#include "engine.h"
#include "list.h"
#include "sound.h"
#include "texture.h"
#include "level.h"
#include "physObj.h"
#include "creature.h"
#include "corpse.h"
#include "lump.h"
#include "surprise.h"
#include "player.h"

/* время в начале и в конце кадра */
unsigned long int startTick, endTick;
/* счетчик кадров и FPS */
unsigned short int fpsCounter;
float delayTime = 0.0f;

SDL_Window* sdlWindow;
SDL_Renderer* sdlRenderer;

bool EngineStart()
{
    startTick = endTick = SDL_GetPerformanceCounter();
    deltaTime = delayTime = 0.0f;
    fps = fpsCounter = 0;

    //if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
    if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
        printf ("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    /* create lists */
    physObjects = ListCreate();
    lumps       = ListCreate();
    surprises   = ListCreate();
    corpses     = ListCreate();
    creatures   = ListCreate();

    return true;
}

void EngineStop()
{
    if (sdlWindow)
        SDL_DestroyWindow (sdlWindow);
    if (sdlRenderer)
        SDL_DestroyRenderer (sdlRenderer);

    SDL_Quit();

    /* free lists */
    ListDestroy (&physObjects);
    ListDestroy (&lumps);
    ListDestroy (&surprises);
    ListDestroy (&corpses);
    ListDestroy (&creatures);

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

    sdlRenderer = SDL_CreateRenderer (sdlWindow, index, flags);
    return (sdlRenderer != NULL);
}

bool EngineInitAudio()
{
    audioSystemLoaded = false;
    if (SDL_Init (SDL_INIT_AUDIO) != 0)
        return false;

    audioSystemLoaded = (Mix_OpenAudio (22050, MIX_DEFAULT_FORMAT, 2, 1024) >= 0);
    return audioSystemLoaded;
}

SDL_Texture* EngineLoadTexture (const char* fileName)
{
    return IMG_LoadTexture(sdlRenderer, fileName);
}

/*SDL_Texture* EngineLoadTextureFromStrip (SDL_Texture* srcTexture, const SDL_Rect* srcRect)
{
    if (srcTexture == NULL || srcRect == NULL)
        return NULL;

    // prepare new SDL_Texture and rect
    SDL_Texture* dstTexture = SDL_CreateTexture (sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, srcRect->w, srcRect->h);
    SDL_Rect dstRect;
    dstRect.x = dstRect.y = 0;
    dstRect.w = srcRect->w;
    dstRect.h = srcRect->h;

    // render to new texture
    SDL_SetRenderTarget (sdlRenderer, dstTexture);
    //SDL_SetRenderDrawColor (sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_SetRenderDrawColor (sdlRenderer, 0, 0, 0, 0);
    SDL_RenderClear (sdlRenderer);
    SDL_RenderCopy (sdlRenderer, srcTexture, srcRect, &dstRect);
    SDL_RenderPresent (sdlRenderer);

    SDL_RenderClear (sdlRenderer);
    SDL_SetRenderTarget (sdlRenderer, NULL);

    return dstTexture;
}*/


void EngineRenderClear()
{
    SDL_RenderClear(sdlRenderer);
}

void EngineRenderImage (SDL_Texture* texture, const SDL_Rect* rect, bool flipX)
{
    if (texture == NULL || rect == NULL)
        return;

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

void EngineRenderImageEx (SDL_Texture* texture, const SDL_Rect* rect,
                          const double angle,
                          const SVector2f* center,
                          const SDL_RendererFlip flip)
{
    if (texture == NULL || rect == NULL || center == NULL)
        return;

    SDL_Point point;
    point.x = (int)(center->x);
    point.y = (int)(center->y);

    SDL_RenderCopyEx (sdlRenderer, texture, NULL, rect, angle, &point, flip);
}

void EngineRenderTile (SDL_Texture* tiles, const SDL_Rect* srcRect, const SDL_Rect* dstRect)
{
    if (tiles == NULL || srcRect == NULL || dstRect == NULL)
        return;

    SDL_RenderCopy (sdlRenderer, tiles, srcRect, dstRect);
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
    delayTime += deltaTime;
    if (delayTime >= 1.0f)
    {
        delayTime -= 1.0f;
        fps = fpsCounter;
        fpsCounter = 0;
    }
}

void EngineClearAllInstances()
{
    CreatureClearAll();
    CorpseClearAll();
    LumpClearAll();
    SurpriseClearAll();
    PhysObjectClearAll();

    player = NULL;
}
