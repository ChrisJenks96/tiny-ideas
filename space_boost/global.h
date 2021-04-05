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

//used to get a 0-255 to 0-SCR_WIDTH on boost x values
#define BOOST_EXTEND_X SCR_WIDTH / 255.0f

static uint8_t cBoostXOffsets[BOOST_NUM] = 
{
    118, 53, 100, 128, 129, 184, 193, 222,
    20, 53, 57, 68, 123, 156, 158, 164,
    16, 61, 83, 84, 182, 206, 213, 248,
    15, 60, 79, 98, 207, 217, 221, 229,
    22, 47, 97, 121, 125, 180, 235, 238,
    69, 112, 169, 180, 190, 218, 203, 239,
    72, 74, 75, 120, 137, 175, 187, 248,
    12, 88, 94, 95, 117, 127, 145, 185,
    24, 47, 67, 78, 103, 126, 166, 248,
    46, 81, 103, 125, 130, 175, 209, 236,
    26, 30, 49, 63, 74, 101, 211, 232,
    4, 89, 95, 98, 137, 155, 166, 230,
    34, 53, 122, 127, 142, 167, 209, 140,
    126, 132, 150, 186, 194, 208, 214, 243,
    32, 73, 110, 117, 187, 203, 17, 219,
    19, 32, 69, 82, 120, 189, 214, 227,
    6, 20, 60, 110, 154, 161, 208, 221,
    20, 27, 69, 78, 166, 228, 227, 213,
    99, 138, 147, 157, 180, 185, 213, 239,
    35, 51, 112, 142, 161, 213, 245, 249,
    3, 22, 120, 171, 181, 187, 201, 254,
    19, 67, 113, 115, 151, 174, 196, 220,
    66, 87, 93, 132, 143, 148, 149, 213,
    16, 17, 57, 62, 63, 71, 114, 232,
    41, 133, 141, 177, 190, 222, 226, 144,
    41, 43, 74, 101, 148, 203, 221, 217,
    38, 79, 114, 137, 159, 195, 199, 152,
    33, 71, 110, 171, 175, 242, 219, 230,
    97, 111, 130, 158, 182, 205, 209, 212,
    1, 51, 78, 86, 123, 131, 156, 218,
    3, 10, 17, 27, 41, 75, 177, 238,
    69, 92, 120, 135, 198, 222, 238, 229
};

#define STAR_NUM 40

static uint16_t cStarOffsets[STAR_NUM] = 
{
    4, 34, 94, 37, 111, 475, 298, 294,
    618, 240, 285, 110, 90, 4, 390, 216,
    582, 482, 409, 285, 120, 546, 127, 502,
    274, 275, 600, 173, 104, 305, 278, 0,
    600, 304, 532, 289, 580, 284, 284, 193
};