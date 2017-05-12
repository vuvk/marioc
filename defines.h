#pragma once

/* main window size */
#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

/* level size */
#define LEVEL_WIDTH  20
#define LEVEL_HEIGHT 15

/* ERRORS */
#define ERR_NONE                      0
#define ERR_SDL_NOT_INITIALIZED       1
#define ERR_SDL_WINDOW_NOT_CREATED    2
#define ERR_SDL_RENDERER_NOT_CREATED  3
#define ERR_SDL_AUDIO_NOT_INITIALIZED 4

/* 60 frames per second */
#define LIMIT_FPS 0.01666666667

#define SQR(X) (X)*(X)
#define EPSILON 0.01

/* physics */
#define GRAVITY 10.0f
#define MIN_VERTICAL_IMPULSE -GRAVITY
#define MAX_VERTICAL_IMPULSE  GRAVITY
#define MIN_HORIZONTAL_IMPULSE -10.0f
#define MAX_HORIZONTAL_IMPULSE  10.0f

/* game logics*/
#define BLOCK_SIZE 32
#define MAX_CREATURES_COUNT 64
#define MAX_LUMPS_COUNT 128
#define MAX_PHYSOBJECTS_COUNT 128

/* booleans */
#define false 0
#define true  1
typedef unsigned char bool;

/* byte */
typedef char byte;
typedef unsigned char ubyte;

/* globals */
float deltaTime;
unsigned short int fps;
