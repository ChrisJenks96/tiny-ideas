#pragma once

#include "global.h"

extern GLFWwindow* pWindow;
extern bool bQuit;
extern bool bKeys[256];

//init all main systems and gl
bool drawInit();
//clear colour and color bit buffer
void drawClear(float fR, float fG, float fB, float fCamX, float fCamY);
void drawSwapBuffers();
void drawQuad(float iX, float iY, float sX, float sY, float sX2, float sY2);
//unload all main systems
void drawFree();
GLuint drawTextureInit(unsigned char* pBuffer, int iW, int iH);
void drawTextureBind(GLuint uiTID);
void drawTextureFree(GLuint uiTID);