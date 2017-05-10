#pragma once

#include <stdio.h>

#include "defines.h"

bool FileExists (const char* fileName)
{
    FILE* f = fopen (fileName, "r");

    if (f == NULL)
    {
        return false;
    }
    else
    {
        fclose (f);
        f = NULL;

        return true;
    }
}

void Swapi (int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void Swapf (float* a, float* b)
{
    float t = *a;
    *a = *b;
    *b = t;
}


void LimitShort (short* a, short min, short max)
{
    if (*a < min) *a = min;
    if (*a > max) *a = max;
}

void LimitInt (int* a, int min, int max)
{
    if (*a < min) *a = min;
    if (*a > max) *a = max;
}

void LimitFloat (float* a, float min, float max)
{
    if (*a < min) *a = min;
    if (*a > max) *a = max;
}
