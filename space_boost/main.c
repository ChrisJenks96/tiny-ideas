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
			*pBkgFadeFactor -= (BKG_FADE_SPEED * (pShip->mMovObj.mVelY / MAX_VEL)) * fDt;
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

static void boostShipTrajectoryUpdate(sMainShipObject* pShip, float* fBoostRot)
{
	if (!pShip->mHitObject)
	{
		pShip->mHitObject = true;

		//get normalised dir of the boost we hit 	
		//float fMagnitude = sqrt((pShip->mMovObj.mVelX*pShip->mMovObj.mVelX) + 
			//(pShip->mMovObj.mVelY*pShip->mMovObj.mVelY));
		//printf("pfMagnitude : %f\n", fMagnitude);

		pShip->mMovObj.mVelX += -sinf(DEG_TO_RAD(*fBoostRot)) * MAX_VEL;
		pShip->mMovObj.mVelY += cosf(DEG_TO_RAD(*fBoostRot)) * MAX_VEL;

		//add some fuel to the repo
		pShip->mFuelRemaining += FUEL_SINGLE_VALUE;
		if (pShip->mFuelRemaining > (FUEL_SINGLE_VALUE * MAX_FUEL_CELLS))
		{
			pShip->mFuelRemaining = FUEL_SINGLE_VALUE * MAX_FUEL_CELLS;
		}
	}
}

static void drawStars(float fXOffset, float* pCamY)
{
	int iCount = 0;
	drawTextureUnbind();

	for (int i = 0; i < STAR_NUM; i+=2)
	{
		//if the star is out of scope, move it to the next frame
		if ((*pCamY+SCR_HEIGHT) < -cStarOffsets[i+1])
		{
			cStarOffsets[i+1] += (SCR_HEIGHT + 20);
		}

		float fSize = cBoostXOffsets[iCount++] * 0.01f;
		drawColor3f(fSize, fSize, fSize);
		drawPushMatrix();
		drawTransformQuad(cStarOffsets[i] + fXOffset, -cStarOffsets[i+1], fSize, fSize, NULL);
		drawQuad();
		drawPopMatrix();
	}
}

static void gameTimerUpdate(char* pGameTimer, int* pSeconds, int* pMinutes, int* pHours)
{
	memset(&pGameTimer[0], 0, 32);
	*pSeconds += 1;

	if (*pSeconds >= 60)
	{
		*pSeconds = 0;
		*pMinutes += 1;
	}

	else if (*pMinutes >= 60)
	{
		*pMinutes = 0;
		*pHours += 1;
	}

	if (*pHours < 10 && *pMinutes < 10 && *pSeconds < 10)
	{
		if (*pSeconds < 0)
		{
			sprintf(&pGameTimer[0], "TIME: -0%i:0%i:0%i", *pHours, *pMinutes, -*pSeconds);
		}
		else
		{
			sprintf(&pGameTimer[0], "TIME: 0%i:0%i:0%i", *pHours, *pMinutes, *pSeconds);
		}
	}

	else if (*pHours < 10 && *pMinutes < 10 && *pSeconds >= 10)
	{
		sprintf(&pGameTimer[0], "TIME: 0%i:0%i:%i", *pHours, *pMinutes, *pSeconds);
	}

	else if (*pHours < 10 && *pMinutes >= 10 && *pSeconds < 10)
	{
		sprintf(&pGameTimer[0], "TIME: 0%i:%i:0%i", *pHours, *pMinutes, *pSeconds);
	}

	else if (*pHours < 10 && *pMinutes >= 10 && *pSeconds >= 10)
	{
		sprintf(&pGameTimer[0], "TIME: 0%i:%i:%i", *pHours, *pMinutes, *pSeconds);
	}

	else if (*pHours >= 10 && *pMinutes < 10 && *pSeconds < 10)
	{
		sprintf(&pGameTimer[0], "TIME: %i:0%i:0%i", *pHours, *pMinutes, *pSeconds);
	}

	else if (*pHours >= 10 && *pMinutes < 10 && *pSeconds >= 10)
	{
		sprintf(&pGameTimer[0], "TIME: %i:0%i:%i", *pHours, *pMinutes, *pSeconds);
	}

	else if (*pHours >= 10 && *pMinutes >= 10 && *pSeconds < 10)
	{
		sprintf(&pGameTimer[0], "TIME: %i:%i:0%i", *pHours, *pMinutes, *pSeconds);
	}

	else if (*pHours >= 10 && *pMinutes >= 10 && *pSeconds >= 10)
	{
		sprintf(&pGameTimer[0], "TIME: %i:%i:%i", *pHours, *pMinutes, *pSeconds);
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
	//for the glRotatef function
	float fBoostRotation = 0.0f;
	float fRotationArr[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	
	char cFontAltitudeText[128], cFontHSText[128], cGameTimerText[32];

	//load the high score for the game in here...
	int iCurrentHighScore = 0;
	int iCurrentAltitude = 0;
	int iBoostStartID = 0, iBoostEndID = 0;

	double dNowTime, dLastTime;
	float fDeltaTime;
	
	//game countdown and t+
	float gameTimerCounter = 0.0f;
	int gameTimerSeconds = -5, gameTimerMinutes = 0, gameTimerHours = 0;
	sprintf(&cGameTimerText[0], "TIME: -00:00:0%i", -gameTimerSeconds);

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
		if (gameTimerSeconds >= 0)
		{
			mainShipObjectUpdate(&Ship1, &bKeys[0], fDeltaTime);
		}
		
		//get the boost sprite to render in the current viewport
		boostGetStartEndID(&fCamY, &iBoostStartID, &iBoostEndID);

		//collision checks against the existing boost sprites
		for (int i = iBoostStartID; i < iBoostEndID; i++)
		{
			if (moveableObjectCollisionUpdate(&Ship1.mMovObj, cBoostXOffsets[i] * BOOST_EXTEND_X, -BOOST_Y_INC * i, BOOST_SIZE))
			{
				//if we hit a boost sprite, then propell the ship in the direction of facing
				boostShipTrajectoryUpdate(&Ship1, &fBoostRotation);
			}
		}

		backgroundUpdate(&fBkgFadeFactor, &Ship1, fDeltaTime);

		//update the camera relative to the ships x/y
		cameraUpdate(&fCamX, &fCamY, &Ship1, fDeltaTime);

		iFuelCellsRemaining = (int)(Ship1.mFuelRemaining * 0.1f);

		iCurrentAltitude = -(int)(Ship1.mMovObj.mY - SCR_HEIGHT + SHIP_SIZE);
		if (iCurrentAltitude > iCurrentHighScore)
		{
			iCurrentHighScore = iCurrentAltitude;
		}

		memset(&cFontAltitudeText[0], 0, 128);
		memset(&cFontHSText[0], 0, 128);
		sprintf(&cFontAltitudeText[0], "ALTITUDE: %i", iCurrentAltitude);
		sprintf(&cFontHSText[0], "HIGH SCORE: %i", iCurrentHighScore);

		gameTimerCounter += 1.0f * fDeltaTime;
		if (gameTimerCounter > 1.0f)
		{
			gameTimerCounter = 0.0f;
			gameTimerUpdate(&cGameTimerText[0], 
				&gameTimerSeconds, &gameTimerMinutes, &gameTimerHours);
		}
		
		//rendering

		drawClear(0.33f * fBkgFadeFactor, 0.56f * fBkgFadeFactor, 
			0.80f * fBkgFadeFactor, fCamX, fCamY);

		drawStars(0.0f, &fCamY);

		drawTextureBind(uiT4);

		//get the boost sprites spinning
		fBoostRotation += 20.0f * fDeltaTime;
		if (fBoostRotation >= 360)
			fBoostRotation = 0;

		fRotationArr[0] = fBoostRotation;

		for (int i = iBoostStartID; i < iBoostEndID; i++)
		{
			drawPushMatrix();
			drawTransformQuad(cBoostXOffsets[i] * BOOST_EXTEND_X, -BOOST_Y_INC * i, BOOST_SIZE, BOOST_SIZE,
				fRotationArr);
			drawQuad();
			drawPopMatrix();
		}

		//reset the rotation back to the original value for rendering the rest of the objects
		fRotationArr[0] = 0.0f;

		drawTextureBind(uiT1);
		drawPushMatrix();
		drawTransformQuad(Ship1.mMovObj.mX, Ship1.mMovObj.mY, SHIP_SIZE, SHIP_SIZE,
				fRotationArr);
		drawQuad();
		drawPopMatrix();

		drawTextureBind(uiT2);
		for (int i = 0; i < iFuelCellsRemaining; i++)
		{
			drawPushMatrix();
			drawTransformQuad(10 + ((FUELBAR_SIZE_X + FUEL_CELL_SPRITE_SEPERATION) * i), 10 + fCamY, FUELBAR_SIZE_X, FUELBAR_SIZE_Y,
				fRotationArr);
			drawQuad();
			drawPopMatrix();
		}

		drawTextureBind(uiT3);
		drawText(10, 30 + fCamY, 16, 9, cFontAltitudeText);
		drawText(10, 50 + fCamY, 16, 9, cFontHSText);
		drawText(10, 70 + fCamY, 16, 9, cGameTimerText);
		
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