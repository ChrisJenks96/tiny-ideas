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
void drawQuad(float fX, float fY, float fX2, float fY2);
void drawQuadSection(float fX, float fY, float fWidth, float fHeight,
	float fX2, float fY2, float fTCX, float fTCY, float fTCX2, float fTCY2);
void drawText(float fX, float fY, float fQuadSize, const char* cStr);
//unload all main systems
void drawFree();
GLuint drawTextureInit(unsigned char* pBuffer, int iW, int iH);
void drawTextureBind(GLuint uiTID);
void drawTextureFree(GLuint uiTID);