#pragma once

/* booleans */
#define bool unsigned char
#define false 0
#define true 1

/* main window size */
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

/* level size */
#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 15

/* ERRORS */
#define ERR_NONE 0
#define ERR_SDL_NOT_INITIALIZED 1
#define ERR_SDL_WINDOW_NOT_CREATED 2
#define ERR_SDL_RENDERER_NOT_CREATED 3

/* globals */
float deltaTime;
int fps;
#define LIMIT_FPS 1.0/30.0

#define SQR(X) (X)*(X)
#define EPSILON 0.01

/* physics */
#define GRAVITY 10
#define MIN_VERTICAL_IMPULSE -GRAVITY
#define MAX_VERTICAL_IMPULSE GRAVITY
#define MIN_HORIZONTAL_IMPULSE -10
#define MAX_HORIZONTAL_IMPULSE 10

/* game logics*/
#define BLOCK_SIZE 32
#define MAX_CREATURES_COUNT 64

