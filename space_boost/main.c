#include "draw.h"
#include "object.h"
#include "network.h"

//images in header format
#include "ship.h"
#include "fuelbar.h"
#include "font.h"
#include "boost.h"
#include "smoke.h"

//variables

sMainShipObject Ship1;
//fading from light into dark when the shuttle climbs
float fBkgFadeFactor;
//GL cam stuff...
float fCamY;
//for the glRotatef function
float fBoostRotation;
float fRotationArr[4];
char cFontAltitudeText[128];
char cFontHSText[128];
char cGameTimerText[32];
char cIPAddrText[IP_MAX_TEXT];
int iIPTextLength;

int iCurrentHighScore;
int iCurrentAltitude;
int iBoostStartID;
int iBoostEndID;
//frame timing
double dNowTime, dLastTime;
float fDeltaTime;

float fThrustSmoke[SMOKE_PARTICLES];

//game related timing (used as secondary high score)
float gameTimerCounter;
int gameTimerSeconds;
int gameTimerMinutes;
int gameTimerHours;

bool gameFirstTimeUse = true;

int iMenuOptions = 0;

GLuint uiT[MAX_TEXTURES];

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
	else if (pShip->mMovObj.mY > HALF_SCR_HEIGHT)
	{
		if (*pBkgFadeFactor < 1.0f)
		{
			*pBkgFadeFactor += (BKG_FADE_SPEED * (-pShip->mMovObj.mVelY / MAX_VEL)) * fDt;
		}
	}
}

static void cameraUpdate(float* pY, sMainShipObject* pShip, float fDt)
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

		pShip->mMovObj.mVelX += sinf(DEG_TO_RAD(*fBoostRot)) * MAX_VEL;
		pShip->mMovObj.mVelY += cosf(DEG_TO_RAD(*fBoostRot)) * MAX_VEL;

		//add some fuel to the repo
		pShip->mFuelRemaining += FUEL_SINGLE_VALUE;
		if (pShip->mFuelRemaining > (FUEL_SINGLE_VALUE * MAX_FUEL_CELLS))
		{
			pShip->mFuelRemaining = FUEL_SINGLE_VALUE * MAX_FUEL_CELLS;
		}
	}
}

static bool gameSaveOrLoad(bool save)
{
	//if the file does not exist
	bool saveExists = access("GAME_SAVE.bin", F_OK) != 0 ? false : true;
	FILE* pFile;

	//save the game
	if (save)
	{
		pFile = fopen("GAME_SAVE.bin", "wb");
		fwrite(&iCurrentHighScore, sizeof(int), 1, pFile);
		fclose(pFile);
		return true;
	}

	//load the game
	else
	{
		if (saveExists)
		{
			pFile = fopen("GAME_SAVE.bin", "rb");
			fread(&iCurrentHighScore, sizeof(int), 1, pFile);
			fclose(pFile);
			return true;
		}

		else
		{
			//return false but having set the high score to 0
			iCurrentHighScore = 0;
			return false;
		}
	}

	return false;
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

static void menuInit()
{
	iIPTextLength = 0;
}

static void gameInit()
{
	if (gameFirstTimeUse)
	{
		Ship1 = mainShipObjectCreate();

		//load the game
		gameSaveOrLoad(false);

		//load statically held textures
		uiT[0] = drawTextureInit(ship_bmp, SHIP_SIZE, SHIP_SIZE);
		uiT[1] = drawTextureInit(fuelbar_bmp, FUELBAR_SIZE_X, FUELBAR_SIZE_Y);
		uiT[3] = drawTextureInit(boost_bmp, BOOST_SIZE, BOOST_SIZE);
		uiT[4] = drawTextureInit(smoke_bmp, SMOKE_SIZE, SMOKE_SIZE);

		gameFirstTimeUse = false;
	}

	else
	{
		//save the highscore that the player has
		gameSaveOrLoad(true);
	}

	mainShipObjectReset(&Ship1);
	//iCUrrentAltitude will resync itself based on the ship
	fBkgFadeFactor = 1.0f;
	fCamY = 0.0f;
	fBoostRotation = 0.0f;
	fRotationArr[0] = 0.0f;
	fRotationArr[1] = 0.0f;
	fRotationArr[2] = 0.0f;
	fRotationArr[3] = 1.0f;
	
	iBoostStartID = 0;
	iBoostEndID = 0;

	for (int i = 0; i < SMOKE_PARTICLES; i++)
	{
		fThrustSmoke[i] = (SMOKE_SPRITE_SIZE * 0.4f) * i;
	}
	
	//game countdown and t+
	gameTimerCounter = 0.0f;
	gameTimerSeconds = -5;
	gameTimerMinutes = 0;
	gameTimerHours = 0;
	sprintf(&cGameTimerText[0], "TIME: -00:00:0%i", -gameTimerSeconds);	
}

int main(int argc, char** argv)
{
	bool bErr = drawInit();
	if (bErr == false)
	{
		return -1;
	}

	//start the game out in the menu state
	int state = STATE_MENU;

	//load the font regardless of initial state
	uiT[2] = drawTextureInit(font_bmp, FONT_SIZE, FONT_SIZE);

	if (state == STATE_MENU)
	{
		menuInit();
	}

	else if (state == STATE_GAME)
	{
		gameInit();
	}
	
	
	dLastTime = 0;

	while (!bQuit)
	{
		dNowTime = glfwGetTime();
		fDeltaTime = dNowTime - dLastTime;

		//updates

		if (state == STATE_MENU)
		{
			//quit if the escape button is pressed or the window close event is triggered
			bQuit = glfwWindowShouldClose(pWindow) | bKeys[GLFW_KEY_ESCAPE];

			//press enter and proceed to the game
			if (cLastKey == 1)
			{
				//unload any menu items here...
				bool bProceed = true;

				if (iMenuOptions == 1)
				{
					//create a client
					if (!bClientInit)
					{
						bProceed = clientInit();
						if (iClientState == CLIENT_STATE_INIT_SUCCESS)
						{
							bClientInit = true;
						}
					}

					if (bProceed)
					{
						//if the client cannot connect and we've picked to do multiplayer,
						//reset out state back to menu
						bProceed = clientConnect(cIPAddrText, 7654, 5000);
						//remove and reset ip again after post connection (bad or good)
						//because bmp font is all upper case and it converts it to lowercase, we must remove 
						//the ip text (slower to revert it back)
						if (iClientState != CLIENT_STATE_CONNECTING_SUCCESS)
						{
							memset(&cIPAddrText[0], 0, strlen(cIPAddrText));
							iIPTextLength = 0;
						}

						//get a set id from the server
						else if (iClientState == CLIENT_STATE_CONNECTING_SUCCESS)
						{
							clientGetNewID();
						}

						bClientConnectedToHost = (iClientState == CLIENT_STATE_NEW_ID);
						
						cLastKey = -1;
					}
					
				}

				if (bClientConnectedToHost)
				{
					//init the game assets
					gameInit();
					state = STATE_GAME;
				}
				
			}

			//press up (multiplayer -> singleplayer)
			else if (cLastKey == 9)
			{
				iMenuOptions -= 1;
				if (iMenuOptions < 0)
				{
					iMenuOptions = 0;
				}
			}

			//press down (singleplayer -> multiplayer)
			else if (cLastKey == 8)
			{
				iMenuOptions += 1;
				if (iMenuOptions > 1)
				{
					iMenuOptions = 1;
				}
			}

			//only show the ip box if you have selected multiplayer
			if (iMenuOptions == 1 && !bLastKeyPressed && 
				cLastKey != -1 && cLastKey != 8 && cLastKey != 9 && 
				cLastKey != 7 && cLastKey != 6 && cLastKey != 1)
			{
				//backspace
				if (cLastKey == 3)
				{
					iIPTextLength -= 1;
					cIPAddrText[iIPTextLength] = 0;
					if (iIPTextLength < 0)
					{
						iIPTextLength = 0;
					}
				}

				else
				{
					cIPAddrText[iIPTextLength++] = cLastKey;
					if (iIPTextLength > IP_MAX_TEXT)
					{
						iIPTextLength = IP_MAX_TEXT;
					}
				}

				bLastKeyPressed = true;
			}
		}

		else if (state == STATE_GAME)
		{
			if (bKeys[GLFW_KEY_ESCAPE])
			{
				state = STATE_MENU;
				//force quit to disabled so we don't double quit (menu will quit the game...)
				bKeys[GLFW_KEY_ESCAPE] = false;
			}

			if (bClientConnectedToHost)
			{
				fClientTimer += 1.0f * fDeltaTime;
				if (fClientTimer > CLIENT_NETWORK_UPDATE)
				{
					clientUpdate();
					fClientTimer = 0.0f;
				}
				
			}

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
			cameraUpdate(&fCamY, &Ship1, fDeltaTime);

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
		}
		
		//rendering

		if (state == STATE_MENU)
		{
			drawClear(0.0f, 0.0f, 0.0f, fCamY);

			drawTextureBind(uiT[2]);
			if (iMenuOptions == 0)
			{
				drawColor3f(1.0f, 1.0f, 1.0f);
			}

			else
			{
				drawColor3f(0.3f, 0.3f, 0.3f);
			}
			
			drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 25.0f, 16.0f, 9.0f, "SINGLE PLAYER GAME", true);
			
			if (iMenuOptions == 1)
			{
				drawColor3f(1.0f, 1.0f, 1.0f);
			}

			else
			{
				drawColor3f(0.3f, 0.3f, 0.3f);
			}

			drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 50.0f, 16.0f, 9.0f, "MULTIPLAYER GAME", true);
			drawColor3f(1.0f, 1.0f, 1.0f);

			if (iMenuOptions == 1)
			{
				drawColor3f(1.0f, 1.0f, 1.0f);

				switch (iClientState)
				{
					case CLIENT_STATE_NULL:
					drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "SERVER ADDRESS", true);
					break; 
					case CLIENT_STATE_INIT_FAILED:
					drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: FAILED TO INIT", true);
					break; 
					case CLIENT_STATE_INIT_SUCCESS:
					drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: INIT SUCCESSFUL", true);
					break; 
					case CLIENT_STATE_CONNECTING:
					drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: CONNECTING...", true);
					break; 
					case CLIENT_STATE_CONNECTING_FAILED:
					drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: FAILED TO CONNECT", true);
					break; 
					case CLIENT_STATE_CONNECTING_SUCCESS:
					drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: CONNECTION SUCCESSFUL", true);
					break; 
					case CLIENT_STATE_NEW_ID:
					drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 120.0f, 16.0f, 9.0f, "CLIENT: NEW ID", true);
					break; 
				}
				
				drawColor3f(1.0f, 1.0f, 1.0f);
				drawText(HALF_SCR_WIDTH, HALF_SCR_HEIGHT + 85.0f, 16.0f, 9.0f, cIPAddrText, true);
				drawTextureUnbind();
				drawBox(HALF_SCR_WIDTH - (6.8f * 16.0f), HALF_SCR_HEIGHT + 80.0f, 240.0f, 27.0f, 1.25f);
			}
		}

		else if (state == STATE_GAME)
		{
			drawClear(0.33f * fBkgFadeFactor, 0.56f * fBkgFadeFactor, 
			0.80f * fBkgFadeFactor, fCamY);

			drawStars(0.0f, &fCamY);
			if (Ship1.mThrottle)
			{
				drawTextureBind(uiT[4]);
				for (int i = 0; i < SMOKE_PARTICLES; i++)
				{
					if (!bKeys[GLFW_KEY_A])
					{
						drawThrust(Ship1.mMovObj.mX - 2.0f, Ship1.mMovObj.mY + 25.0f, &fThrustSmoke[i], THRUST_LENGTH, SMOKE_SPRITE_SIZE, 10.0f * fDeltaTime);
					}

					if (!bKeys[GLFW_KEY_D])
					{
						drawThrust(Ship1.mMovObj.mX + 20.0f, Ship1.mMovObj.mY + 25.0f, &fThrustSmoke[i], THRUST_LENGTH, SMOKE_SPRITE_SIZE, 10.0f * fDeltaTime);
					}
				}
			}
			
			drawTextureBind(uiT[3]);

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

			drawTextureBind(uiT[0]);
			drawPushMatrix();
			drawTransformQuad(Ship1.mMovObj.mX, Ship1.mMovObj.mY, SHIP_SIZE, SHIP_SIZE,
					fRotationArr);
			drawQuad();
			drawPopMatrix();

			drawTextureBind(uiT[1]);
			int iFuelCellsRemaining = (int)(Ship1.mFuelRemaining * 0.1f);
			for (int i = 0; i < iFuelCellsRemaining; i++)
			{
				drawPushMatrix();
				drawTransformQuad(10 + ((FUELBAR_SIZE_X + FUEL_CELL_SPRITE_SEPERATION) * i), 10 + fCamY, FUELBAR_SIZE_X, FUELBAR_SIZE_Y,
					fRotationArr);
				drawQuad();
				drawPopMatrix();
			}

			drawTextureBind(uiT[2]);
			drawText(10, 30 + fCamY, 16, 9, cFontAltitudeText, false);
			drawText(10, 50 + fCamY, 16, 9, cFontHSText, false);
			drawText(10, 70 + fCamY, 16, 9, cGameTimerText, false);
		}

		drawSwapBuffers();

		//printf("dt: %f\n", fDeltaTime);
		dLastTime = dNowTime;
	}

	drawTextureFree(uiT[0]);
	drawTextureFree(uiT[1]);
	drawTextureFree(uiT[2]);
	drawTextureFree(uiT[3]);
	drawTextureFree(uiT[4]);
	drawFree();
	clientDestroy();
	return 0;
}