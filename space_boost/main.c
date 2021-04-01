#include "draw.h"
#include "object.h"

//images in header format
#include "ship.h"
#include "fuelbar.h"

static void backgroundUpdate(float* pBkgFadeFactor, sMainShipObject* pShip, float fDt)
{
	//if the camera has centralised on the ship, then start fading into the stars
	if (pShip->mMovObj.mY < HALF_SCR_HEIGHT)
	{
		if (*pBkgFadeFactor > 0.01f)
		{
			*pBkgFadeFactor -= (BKG_FADE_SPEED * (pShip->mMovObj.mVel / MAX_VEL)) * fDt;
		}	
	}

	//if we're dropping back down to the surface, bring the colour back
	/*else
	{
		if (*pBkgFadeFactor < 1.0f)
		{
			*pBkgFadeFactor += BKG_FADE_SPEED * fDt;
		}
	}*/
}

static void cameraUpdate(float* pX, float* pY, sMainShipObject* pShip, float fDt)
{
	if (pShip->mMovObj.mY <= HALF_SCR_HEIGHT)
	{
		*pY = pShip->mMovObj.mY - HALF_SCR_HEIGHT;
	}
}

int main(int argc, char** argv)
{
	bool bErr = drawInit();
	if (bErr == false)
	{
		return -1;
	}

	int iFuelCellsRemaining = MAX_FUEL_CELLS;

	sMainShipObject Ship1 = mainShipObjectCreate();
	//fading from light into dark when the shuttle climbs
	float fBkgFadeFactor = 1.0f;
	//GL cam stuff...
	float fCamX = 0.0f, fCamY = 0.0f;
	
	double dNowTime, dLastTime;
	float fDeltaTime;

	//load statically held textures
	GLuint uiT1 = drawTextureInit(ship_bmp, SHIP_SIZE, SHIP_SIZE);
	GLuint uiT2 = drawTextureInit(fuelbar_bmp, FUELBAR_SIZE_X, FUELBAR_SIZE_Y);
	
	dLastTime = 0;

	while (!bQuit)
	{
		dNowTime = glfwGetTime();
		fDeltaTime = dNowTime - dLastTime;

		//updates

		//quit if the escape button is pressed or the window close event is triggered
		bQuit = glfwWindowShouldClose(pWindow) | bKeys[GLFW_KEY_ESCAPE];

		//fBkgFadeFactor -= (!((int)Ship1.mY % SKY_TRANSITION_Y)) ? BKG_FADE_SPEED : 0.0f;
		mainShipObjectUpdate(&Ship1, &bKeys[0], fDeltaTime);

		backgroundUpdate(&fBkgFadeFactor, &Ship1, fDeltaTime);

		//update the camera relative to the ships x/y
		cameraUpdate(&fCamX, &fCamY, &Ship1, fDeltaTime);

		//rendering
		drawClear(0.33f * fBkgFadeFactor, 0.56f * fBkgFadeFactor, 
			0.80f * fBkgFadeFactor, fCamX, fCamY);

		drawTextureBind(uiT1);
		drawQuad(Ship1.mMovObj.mX, Ship1.mMovObj.mY, 10, 10, 10 + SHIP_SIZE, 10 + SHIP_SIZE);

		drawTextureBind(uiT2);
		iFuelCellsRemaining = (int)((Ship1.mFuelRemaining - 0.1f) / 10.0f)+1;
		for (int i = 0; i < iFuelCellsRemaining; i++)
		{
			drawQuad(10 + ((FUELBAR_SIZE_X + FUEL_CELL_SPRITE_SEPERATION) * i), 10 + fCamY, 0, 0, FUELBAR_SIZE_X, FUELBAR_SIZE_Y);
		}

		drawSwapBuffers();

		//printf("dt: %f\n", fDeltaTime);
		dLastTime = dNowTime;
	}

	drawTextureFree(uiT1);
	drawFree();
	return 0;
}