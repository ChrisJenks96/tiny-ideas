#include "draw.h"
#include "object.h"
#include "ship.h"

#define SHIP_SIZE 32
#define BKG_FADE_SPEED 0.1f

static void backgroundUpdate(float* pBkgFadeFactor, sMoveableObject* pShip, float fDt)
{
	if (pShip->mY < HALF_SCR_HEIGHT)
	{
		if (*pBkgFadeFactor > 0.01f)
		{
			*pBkgFadeFactor -= (BKG_FADE_SPEED * (pShip->mVel / MAX_VEL)) * fDt;
		}	
	}
}

static void cameraUpdate(float* pX, float* pY, sMoveableObject* pShip, float fDt)
{
	if (pShip->mY <= HALF_SCR_HEIGHT)
	{
		*pY = pShip->mY - HALF_SCR_HEIGHT;
	}
}

int main(int argc, char** argv)
{
	bool bErr = drawInit();
	if (bErr == false)
	{
		return -1;
	}

	sMoveableObject Ship1 = moveableObjectCreate(SHIP_SIZE, HALF_SCR_WIDTH - SHIP_SIZE, 
		SCR_HEIGHT - SHIP_SIZE, 4.0f);
	float fBkgFadeFactor = 1.0f;
	float fCamX = 0.0f, fCamY = 0.0f;
	double dNowTime, dLastTime;
	float fDeltaTime;

	//load statically held textures
	GLuint uiT1 = drawTextureInit(ship_bmp, SHIP_SIZE, SHIP_SIZE);
	
	dLastTime = 0;

	while (!bQuit)
	{
		dNowTime = glfwGetTime();
		fDeltaTime = dNowTime - dLastTime;

		//updates

		//quit if the escape button is pressed or the window close event is triggered
		bQuit = glfwWindowShouldClose(pWindow) | bKeys[GLFW_KEY_ESCAPE];

		//fBkgFadeFactor -= (!((int)Ship1.mY % SKY_TRANSITION_Y)) ? BKG_FADE_SPEED : 0.0f;
		moveableObjectUpdate(&Ship1, &bKeys[0], fDeltaTime);

		backgroundUpdate(&fBkgFadeFactor, &Ship1, fDeltaTime);

		//update the camera relative to the ships x/y
		cameraUpdate(&fCamX, &fCamY, &Ship1, fDeltaTime);

		//rendering
		drawClear(0.33f * fBkgFadeFactor, 0.56f * fBkgFadeFactor, 
			0.80f * fBkgFadeFactor, fCamX, fCamY);

		drawTextureBind(uiT1);
		drawQuad(Ship1.mX, Ship1.mY, 10, 10, 10 + SHIP_SIZE, 10 + SHIP_SIZE);

		drawSwapBuffers();

		//printf("dt: %f\n", fDeltaTime);
		dLastTime = dNowTime;
	}

	drawTextureFree(uiT1);
	drawFree();
	return 0;
}