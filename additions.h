#pragma once

#include <stdio.h>

#include "defines.h"

bool FileExists (const char* fileName);

void Swapc (char** a, char** b);
void Swaps (short* a, short* b);
void Swapi (int* a, int* b);
void Swapf (float* a, float* b);

void LimitShort (short* a, short min, short max);
void LimitInt (int* a, int min, int max);
void LimitFloat (float* a, float min, float max);
