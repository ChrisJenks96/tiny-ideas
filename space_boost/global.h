#pragma once

#include <stdio.h>
#include <stdbool.h>

#include <GL/gl.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

//SYSTEM RELATED...

#define SCR_WIDTH 640
#define HALF_SCR_WIDTH 320
#define SCR_HEIGHT 480
#define HALF_SCR_HEIGHT 240

//TEXTURE RELATED....

#define SHIP_SIZE 32

#define FUELBAR_SIZE_X 48
#define FUELBAR_SIZE_Y 16

//GAME RELATED...

#define BKG_FADE_SPEED 0.1f
#define FUEL_CELL_SPRITE_SEPERATION 5
#define FUEL_CONSUMPTION_RATE 1.2f
#define MAX_FUEL_CELLS 5
#define FUEL_START_VALUE (MAX_FUEL_CELLS * 10.0f)
#define MAX_VEL 20.0f
#define GRAVITY_Y_VAL 5.0f