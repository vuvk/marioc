#pragma once

#include "SDL2/SDL_mixer.h"

Mix_Chunk* sndJump;
Mix_Chunk* sndMarioDie;
Mix_Chunk* sndKick;


void LoadSounds();
void FreeSounds();
void StopAllSounds();
