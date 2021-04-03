#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
#define BOOST_SIZE 32

#define FUELBAR_SIZE_X 48
#define FUELBAR_SIZE_Y 16

#define FONT_SIZE 128.0f
#define FONT_CHARS_PER_ROW 8
#define FONT_CHAR_SIZE (FONT_SIZE / FONT_CHARS_PER_ROW) //128px wide with 8 chars per row
#define FONT_MAX_CHARS_PER_ROW 30

//GAME RELATED...

#define BKG_FADE_SPEED 0.1f
#define FUEL_CELL_SPRITE_SEPERATION 5
#define FUEL_CONSUMPTION_RATE 1.2f
#define MAX_FUEL_CELLS 5
#define FUEL_START_VALUE (MAX_FUEL_CELLS * 10.0f)
#define MAX_VEL 20.0f
#define GRAVITY_Y_VAL 5.0f

#define BOOST_NUM 256
#define BOOST_Y_INC 200
#define BOOST_MIN_ROT 35
#define BOOST_MAX_ROT 155

//used to get a 0-255 to 0-SCR_WIDTH on boost x values
#define BOOST_EXTEND_X SCR_WIDTH / 255.0f

static uint8_t cBoostXOffsets[BOOST_NUM] = 
{
	115, 130, 23, 201, 30, 75, 103, 32
};