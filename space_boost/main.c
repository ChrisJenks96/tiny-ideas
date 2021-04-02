#include "draw.h"
#include "object.h"

//images in header format
#include "ship.h"
#include "fuelbar.h"
#include "font.h"
#include "boost.h"

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

//get the start and end arr id's for rendering the boost sprites
static void boostGetStartEndID(float* pCamY, int* iStart, int* iEnd)
{
	bool bFoundFirst = false;
	int iBoostCounter = *iStart;
	float fCurrentBoost;
	float fBoundaryY1;
	float fBoundaryY2;

	while (iBoostCounter < BOOST_NUM)
	{
		fCurrentBoost = -BOOST_Y_INC * iBoostCounter;
		fBoundaryY1 = *pCamY - BOOST_SIZE; //top
		fBoundaryY2 = fBoundaryY1 + SCR_HEIGHT + BOOST_SIZE; //bottom

		if (fCurrentBoost >= fBoundaryY1 && fCurrentBoost <= fBoundaryY2)
		{
			if (!bFoundFirst)
			{
				*iStart = iBoostCounter;
				bFoundFirst = true;
			}

			*iEnd = iBoostCounter+1;
		}

		else
		{
			//printf("Start:%i End:%i\n", *iStart, *iEnd);
			//if a boost sprite is out of viewport bounds and beyond end rendering boost id, then kill the loop
			if (iBoostCounter > *iEnd)
			{
				break;
			}
		}
		
		iBoostCounter++;
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
	
	char cFontAltitudeText[128], cFontHSText[128];

	//load the high score for the game in here...
	int iCurrentHighScore = 0;
	int iCurrentAltitude = 0;
	int iBoostStartID = 0, iBoostEndID = 0;

	double dNowTime, dLastTime;
	float fDeltaTime;

	//load statically held textures
	GLuint uiT1 = drawTextureInit(ship_bmp, SHIP_SIZE, SHIP_SIZE);
	GLuint uiT2 = drawTextureInit(fuelbar_bmp, FUELBAR_SIZE_X, FUELBAR_SIZE_Y);
	GLuint uiT3 = drawTextureInit(font_bmp, FONT_SIZE, FONT_SIZE);
	GLuint uiT4 = drawTextureInit(boost_bmp, BOOST_SIZE, BOOST_SIZE);
	
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
		
		//get the boost sprite to render in the current viewport
		boostGetStartEndID(&fCamY, &iBoostStartID, &iBoostEndID);

		//collision checks against the existing boost sprites
		for (int i = iBoostStartID; i < iBoostEndID; i++)
		{
			if (moveableObjectCollisionUpdate(&Ship1.mMovObj, cBoostXOffsets[i] * BOOST_EXTEND_X, -BOOST_Y_INC * i, BOOST_SIZE))
			{
				//do collisiony stuff here
			}
		}

		backgroundUpdate(&fBkgFadeFactor, &Ship1, fDeltaTime);

		//update the camera relative to the ships x/y
		cameraUpdate(&fCamX, &fCamY, &Ship1, fDeltaTime);

		iFuelCellsRemaining = (int)((Ship1.mFuelRemaining - 0.1f) * 0.1f);

		iCurrentAltitude = -(int)(Ship1.mMovObj.mY - SCR_HEIGHT + SHIP_SIZE);
		if (iCurrentAltitude > iCurrentHighScore)
		{
			iCurrentHighScore = iCurrentAltitude;
		}

		memset(&cFontAltitudeText[0], 0, 128);
		memset(&cFontHSText[0], 0, 128);
		sprintf(&cFontAltitudeText[0], "ALTITUDE: %i", iCurrentAltitude);
		sprintf(&cFontHSText[0], "HIGH SCORE: %i", iCurrentHighScore);

		//rendering

		drawClear(0.33f * fBkgFadeFactor, 0.56f * fBkgFadeFactor, 
			0.80f * fBkgFadeFactor, fCamX, fCamY);

		drawTextureBind(uiT4);
		for (int i = iBoostStartID; i < iBoostEndID; i++)
		{
			drawQuad(cBoostXOffsets[i] * BOOST_EXTEND_X, -BOOST_Y_INC * i, BOOST_SIZE, BOOST_SIZE);
		}

		drawTextureBind(uiT1);
		drawQuad(Ship1.mMovObj.mX, Ship1.mMovObj.mY, SHIP_SIZE, SHIP_SIZE);

		drawTextureBind(uiT2);
		for (int i = 0; i < iFuelCellsRemaining; i++)
		{
			drawQuad(10 + ((FUELBAR_SIZE_X + FUEL_CELL_SPRITE_SEPERATION) * i), 10 + fCamY, FUELBAR_SIZE_X, FUELBAR_SIZE_Y);
		}

		drawTextureBind(uiT3);
		drawText(10, 30 + fCamY, 16, cFontAltitudeText);
		drawText(10, 50 + fCamY, 16, cFontHSText);
		
		drawSwapBuffers();

		//printf("dt: %f\n", fDeltaTime);
		dLastTime = dNowTime;
	}

	drawTextureFree(uiT1);
	drawTextureFree(uiT2);
	drawTextureFree(uiT3);
	drawTextureFree(uiT4);
	drawFree();
	return 0;
}