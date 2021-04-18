#pragma once

#include "global.h"

extern bool bKeys[256];
extern char cLastKey;

class cDraw
{
	public:
		bool Init();
		void Clear(float fR, float fG, float fB, float fCamY);
		void SwapBuffers();
		void PushMatrix();
		void PopMatrix();
		void TransformQuad(float fX, float fY, float fX2, float fY2, float* fRotVal);
		void TransformPoint(float fX, float fY);
		void Quad();
		void Color3f(float fR, float fG, float fB);
		void Box(float fX, float fY, float fX2, float fY2, float fThick);
		void Line(float fX, float fY, float fX2, float fY2, double fThick);
		void QuadSection(float fX, float fY, float fWidth, float fHeight,
		float fX2, float fY2, float fTCX, float fTCY, float fTCX2, float fTCY2);
		void Text(float fX, float fY, float fQuadSize, float fTextSepSize, const char* cStr, int iLen, bool bCentre);
		void Thrust(float fX, float fY, float* fThrustY, float fLength, float fSize, float fSpeed);
		void Stars(float fXOffset, float* pCamY);
		void Point();
		GLuint TextureInit(const unsigned char* pBuffer, int iW, int iH);
		void TextureBind(GLuint uiTID);
		void TextureUnbind();
		void TextureFree(GLuint uiTID);
		void Free();
		void SetQuit(bool bQ){mQuit=bQ;}
		bool& IsQuit(){return mQuit;}
		GLFWwindow* GetWindow(){return mWindow;}
	private:
		GLFWwindow* mWindow;
		bool mQuit;
};