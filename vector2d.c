#include <math.h>

#include "defines.h"
#include "vector2d.h"


/* float vector */
SVector2f AddVector2f (SVector2f v1, SVector2f v2)
{
    SVector2f ret;
    ret.x = v1.x + v2.x;
    ret.y = v1.y + v2.y;

    return ret;
}

SVector2f SubtractVector2f (SVector2f v1, SVector2f v2)
{
    SVector2f ret;
    ret.x = v1.x - v2.x;
    ret.y = v1.y - v2.y;

    return ret;
}

float DotVector2f (SVector2f v1, SVector2f v2)
{
    return v1.x*v2.x + v1.y*v2.y;
}

float MagnitudeVector2f (SVector2f v1, SVector2f v2)
{
    return sqrt (SQR(v2.x - v1.x) + SQR(v2.y - v1.y));
}

SVector2f MultVector2f (SVector2f v1, float s)
{
    SVector2f ret;
    ret.x = v1.x * s;
    ret.y = v1.y * s;

    return ret;
}

SVector2f DivVector2f (SVector2f v1, float s)
{
    float reverse;
    if (s != 0.0f)
        reverse = 1.0f / s;
    else
        reverse = 0.0f;

    return MultVector2f (v1, reverse);
}



/* int vector */
SVector2i AddVector2i (SVector2i v1, SVector2i v2)
{
    SVector2i ret;
    ret.x = v1.x + v2.x;
    ret.y = v1.y + v2.y;

    return ret;
}

SVector2i SubtractVector2i (SVector2i v1, SVector2i v2)
{
    SVector2i ret;
    ret.x = v1.x - v2.x;
    ret.y = v1.y - v2.y;

    return ret;
}

int DotVector2i (SVector2i v1, SVector2i v2)
{
    return v1.x*v2.x + v1.y*v2.y;
}

float MagnitudeVector2i (SVector2i v1, SVector2i v2)
{
    return sqrt (SQR(v2.x - v1.x) + SQR(v2.y - v1.y));
}

SVector2i MultVector2i (SVector2i v1, float s)
{
    SVector2i ret;
    ret.x = (int)(v1.x * s);
    ret.y = (int)(v1.y * s);

    return ret;
}

SVector2i DivVector2i (SVector2i v1, float s)
{
    float reverse;
    if (s != 0.0f)
        reverse = 1.0f / s;
    else
        reverse = 0.0f;

    return MultVector2i (v1, reverse);
}
