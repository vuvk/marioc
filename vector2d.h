#pragma once

typedef struct
{
    int x;
    int y;
} SVector2i;

typedef struct
{
    float x;
    float y;
} SVector2f;

/* операции с векторами */
/* сложение/вычитание векторов */
SVector2f AddVector2f (SVector2f v1, SVector2f v2);
SVector2f SubtractVector2f (SVector2f v1, SVector2f v2);
/* скалярное произведение */
float DotVector2f (SVector2f v1, SVector2f v2);
/* длина вектора */
float MagnitudeVector2f (SVector2f v1, SVector2f v2);
/* умножение/деление на скаляр */
SVector2f MultVector2f (SVector2f v1, float s);
SVector2f DivVector2f (SVector2f v1, float s);

SVector2i AddVector2i (SVector2i v1, SVector2i v2);
SVector2i SubtractVector2i (SVector2i v1, SVector2i v2);
int DotVector2i (SVector2i v1, SVector2i v2);
float MagnitudeVector2i (SVector2i v1, SVector2i v2);
SVector2i MultVector2i (SVector2i v1, float s);
SVector2i DivVector2i (SVector2i v1, float s);
