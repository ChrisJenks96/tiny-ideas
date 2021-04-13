#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

//SYSTEM RELATED...

#define SCR_WIDTH 640
#define HALF_SCR_WIDTH (SCR_WIDTH / 2)
#define SCR_HEIGHT 480
#define HALF_SCR_HEIGHT (SCR_HEIGHT / 2)

#define TARGET_FPS 1.0 / 30.0

#define STATE_MENU 0
#define STATE_GAME 1

//TEXTURE RELATED....

#define MAX_TEXTURES 5

#define SHIP_SIZE 32
#define BOOST_SIZE 32

#define SMOKE_SIZE 32
#define SMOKE_PARTICLES 4
#define SMOKE_SPRITE_SIZE 14.0f
#define SMOKE_SHRINKING_VALUE (SMOKE_SPRITE_SIZE / SMOKE_PARTICLES)
#define THRUST_LENGTH 20.0f

#define FUELBAR_SIZE_X 48
#define FUELBAR_SIZE_Y 16

#define FONT_SIZE 128.0f
#define FONT_CHARS_PER_ROW 8
#define FONT_CHAR_SIZE (FONT_SIZE / FONT_CHARS_PER_ROW) //128px wide with 8 chars per row
#define FONT_MAX_CHARS_PER_ROW 30

//NETWORK RELATED...

#define SERVER_MAX_CLIENTS 4

//GAME RELATED...

#define IP_MAX_TEXT 20

#define BKG_FADE_SPEED 0.1f
#define FUEL_CELL_SPRITE_SEPERATION 5
#define FUEL_CONSUMPTION_RATE 1.2f
#define MAX_FUEL_CELLS 12
#define FUEL_SINGLE_VALUE 10.0f
#define FUEL_START_VALUE (5 * FUEL_SINGLE_VALUE)
#define MAX_VEL 20.0f
#define GRAVITY_Y_VAL 5.0f

#define BOOST_NUM 256
#define HALF_BOOST_NUM (BOOST_NUM / 2)
#define BOOST_Y_INC 200
#define BOOST_MIN_ROT 35
#define BOOST_MAX_ROT 155

#define PI 3.14159265
#define DEG_TO_RAD(x) (x * (PI / 180))
#define RAD_TO_DEG(x) (x * (180 / PI))

#define BOOST_REACTIVATION_WAIT_TIME 5.0f

#define ALTITUDE_TEXT_SIZE 32
#define HIGH_SCORE_TEXT_SIZE 32
#define GAME_TIMER_TEXT_SIZE 32

//used to get a 0-255 to 0-SCR_WIDTH on boost x values
#define BOOST_EXTEND_X SCR_WIDTH / 255.0f

extern uint8_t cBoostXOffsets[BOOST_NUM];

#define STAR_NUM 40

extern uint16_t cStarOffsets[STAR_NUM];