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
