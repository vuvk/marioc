#pragma once

#include "vector2d.h"

/* main window size */
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

/* level size */
#define LEVEL_WIDTH  200
#define LEVEL_HEIGHT 15

/* ERRORS */
#define ERR_NONE                      0
#define ERR_SDL_NOT_INITIALIZED       1
#define ERR_SDL_WINDOW_NOT_CREATED    2
#define ERR_SDL_RENDERER_NOT_CREATED  3
#define ERR_SDL_AUDIO_NOT_INITIALIZED 4


#define SQR(X) (X)*(X)
#define EPSILON 0.01


/* game logics*/
#define BLOCK_SIZE 32
#define TILE_SIZE 32
#define COUNT_TILES_HORIZONTAL 19
#define COUNT_TILES_VERTICAL 12

/* physics */
#define GRAVITY BLOCK_SIZE/3.0f
#define MIN_VERTICAL_IMPULSE -GRAVITY
#define MAX_VERTICAL_IMPULSE  GRAVITY
#define MIN_HORIZONTAL_IMPULSE -GRAVITY
#define MAX_HORIZONTAL_IMPULSE  GRAVITY

/* 60 frames per second */
#define LIMIT_FPS 60


/* === NEW TYPES === */

/* booleans */
#define false 0
#define true  1
typedef unsigned char bool;

/* byte */
typedef char byte;
typedef unsigned char ubyte;

/* int16 */
typedef signed short int int16;
typedef unsigned short int uint16;

/* int32 */
typedef signed int int32;
typedef unsigned int uint32;

/* int64 */
typedef signed long int int64;
typedef unsigned long int uint64;



/* === GLOBALS === */
float deltaTime;
uint16 fps;
SVector2f cameraPos;

