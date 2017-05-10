#pragma once

#include <stdio.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_image.h"

#include "defines.h"

extern float deltaTime;
extern int fps;

bool EngineStart();
void EngineStop();

bool EngineCreateWindow (const char* title, int x, int y, int w, int h, unsigned long flags);
void EngineSetWindowTitle (const char* title);
bool EngineCreateRenderer (int index, unsigned long flags);
bool EngineInitAudio();
SDL_Texture* EngineLoadTexture (const char* fileName);


void EngineRenderClear();
void EngineUpdateAndRenderLevel();
void EngineRenderImage (SDL_Texture* texture, SDL_Rect* rect, bool flipX);
void EngineRenderPresent();
void EngineUpdateTime();

