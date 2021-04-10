#pragma once

#include "global.h"

extern GLFWwindow* pWindow;
extern bool bQuit;
extern bool bKeys[256];
extern char cLastKey;

//init all main systems and gl
bool drawInit();
//clear colour and color bit buffer
void drawClear(float fR, float fG, float fB, float fCamY);
void drawSwapBuffers();
void drawPushMatrix();
void drawPopMatrix();
void drawTransformQuad(float fX, float fY, float fX2, float fY2, float* fRotVal);
void drawQuad();
void drawColor3f(float fR, float fG, float fB);
void drawBox(float fX, float fY, float fX2, float fY2, float fThick);
void drawLine(float fX, float fY, float fX2, float fY2, double fThick);
void drawQuadSection(float fX, float fY, float fWidth, float fHeight,
	float fX2, float fY2, float fTCX, float fTCY, float fTCX2, float fTCY2);
void drawText(float fX, float fY, float fQuadSize, float fTextSepSize, const char* cStr, int iLen, bool bCentre);
void drawThrust(float fX, float fY, float* fThrustY, float fLength, float fSize, float fSpeed);
void drawStars(float fXOffset, float* pCamY);
//unload all main systems
void drawFree();
GLuint drawTextureInit(const unsigned char* pBuffer, int iW, int iH);
void drawTextureBind(GLuint uiTID);
void drawTextureUnbind();
void drawTextureFree(GLuint uiTID);