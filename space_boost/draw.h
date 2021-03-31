#pragma once

#include <stdio.h>
#include <stdbool.h>

#include <GL/gl.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#define SCR_WIDTH 640
#define SCR_HEIGHT 480

extern GLFWwindow* pWindow;
extern bool bQuit;

//init all main systems and gl
bool drawInit();
//clear colour and color bit buffer
void drawClear();
void drawSwapBuffers();
void drawQuad(int iX, int iY, int sX, int sY, int sX2, int sY2);
//unload all main systems
void drawFree();
GLuint drawTextureInit(unsigned char* pBuffer, int iW, int iH);
void drawTextureBind(GLuint uiTID);
void drawTextureFree(GLuint uiTID);