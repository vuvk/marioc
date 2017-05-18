#pragma once

#include "SDL2/SDL_mixer.h"

#include "defines.h"

bool audioSystemLoaded;

Mix_Chunk* sndCoin;
Mix_Chunk* sndPowerUp;

Mix_Chunk* sndKick;
Mix_Chunk* sndJump;

Mix_Chunk* sndMarioDie;

Mix_Chunk* sndBump;
Mix_Chunk* sndBreakBlock;

void LoadSounds();
void FreeSounds();
void SoundPlay (Mix_Chunk* snd, uint16 loops);
void StopAllSounds();
