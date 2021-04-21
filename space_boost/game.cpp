#include "game.h"

bool cGame::Init()
{
	mBkgFadeFactor = 1.0f;
	mGameTimer = 0.0f;
	mTime.mSeconds = -5;
	mTime.mMinutes = 0;
	mTime.mHours = 0;
	memset(&mFrameTime, 0, sizeof(sFrameTime));
	return true;
}

void cGame::ShowFPS()
{	
	mFrameTime.mFrames++;
	mFrameTime.mFPSCounter += mFrameTime.mDeltaTime;

	if (mFrameTime.mFPSCounter >= 1.0f)
	{
		printf("FPS: %i\n", mFrameTime.mFrames);
		mFrameTime.mFrames = 0;
		mFrameTime.mFPSCounter = 0.0f;
	}
}

//fading from light into dark when the shuttle climbs
void cGame::BackgroundUpdate(cMainShip& rShip)
{
	float fCurrY = std::get<1>(rShip.GetXY());
	float fCurrVelY = std::get<1>(rShip.GetVelXY());

	//if the camera has centralised on the ship, then start fading into the stars
	if (fCurrY < HALF_SCR_HEIGHT)
	{
		if (mBkgFadeFactor > 0.01f)
		{
			mBkgFadeFactor -= (BKG_FADE_SPEED * (fCurrVelY / MAX_VEL)) * mFrameTime.mDeltaTime;
		}	
	}

	//if we're dropping back down to the surface, bring the colour back
	else if (fCurrY > HALF_SCR_HEIGHT)
	{
		if (mBkgFadeFactor < 1.0f)
		{
			mBkgFadeFactor += (BKG_FADE_SPEED * (-fCurrVelY / MAX_VEL)) * mFrameTime.mDeltaTime;
		}
	}
}

//update the camera relative to the ships x/y
void cGame::CameraUpdate(cMainShip& rShip)
{
	float fCurrY = std::get<1>(rShip.GetXY());
	if (fCurrY <= HALF_SCR_HEIGHT)
	{
		mCamY = fCurrY - HALF_SCR_HEIGHT;
	}
}

bool cGame::SaveOrLoad(bool bSave)
{
	//if the file does not exist
	bool saveExists = access("GAME_SAVE.bin", F_OK) != 0 ? false : true;
	FILE* pFile;

	//save the game
	if (bSave)
	{
		pFile = fopen("GAME_SAVE.bin", "wb");
		fwrite(&mCurrentHighScore, sizeof(int), 1, pFile);
		fclose(pFile);
		return true;
	}

	//load the game
	else
	{
		if (saveExists)
		{
			pFile = fopen("GAME_SAVE.bin", "rb");
			//03 compiler flags want fread to have a return type
			fread(&mCurrentHighScore, sizeof(int), 1, pFile);
			fclose(pFile);
			return true;
		}

		else
		{
			//return false but having set the high score to 0
			mCurrentHighScore = 0;
			return false;
		}
	}

	return false;
}

//get the start and end arr id's for rendering the boost sprites
void cGame::BoostGetStartEndID(int* pStart, int* pEnd)
{
	bool bFoundFirst = false;
	int iBoostCounter = *pStart;
	float fCurrentBoost;
	float fBoundaryY1;
	float fBoundaryY2;

	while (iBoostCounter < BOOST_NUM)
	{
		fCurrentBoost = -BOOST_Y_INC * iBoostCounter;
		fBoundaryY1 = mCamY - BOOST_SIZE; //top
		fBoundaryY2 = fBoundaryY1 + SCR_HEIGHT + BOOST_SIZE; //bottom

		if (fCurrentBoost >= fBoundaryY1 && fCurrentBoost <= fBoundaryY2)
		{
			if (!bFoundFirst)
			{
				*pStart = iBoostCounter;
				bFoundFirst = true;
			}

			*pEnd = iBoostCounter+1;
		}

		else
		{
			//printf("Start:%i End:%i\n", *iStart, *iEnd);
			//if a boost sprite is out of viewport bounds and beyond end rendering boost id, then kill the loop
			if (iBoostCounter > *pEnd)
			{
				break;
			}
		}
		
		iBoostCounter++;
	}
}

void cGame::UpdateScores(cMainShip& rShip)
{
	mCurrentAltitude = -(int)(std::get<1>(rShip.GetXY()) - SCR_HEIGHT + SHIP_SIZE);
	if (mCurrentAltitude > mCurrentHighScore)
	{
		mCurrentHighScore = mCurrentAltitude;
	}
}

void cGame::TimerUpdate(char* pGameTimerText)
{
	mGameTimer += 1.0f * mFrameTime.mDeltaTime;
	if (mGameTimer > 1.0f)
	{
		mGameTimer = 0.0f;
		mTime.mSeconds += 1;

		if (mTime.mSeconds >= 60)
		{
			mTime.mSeconds = 0;
			mTime.mMinutes += 1;
		}

		else if (mTime.mMinutes >= 60)
		{
			mTime.mMinutes = 0;
			mTime.mHours += 1;
		}

		if (mTime.mHours < 10 && mTime.mMinutes < 10 && mTime.mSeconds < 10)
		{
			if (mTime.mSeconds < 0)
			{
				sprintf(&pGameTimerText[0], "TIME: -0%i:0%i:0%i", mTime.mHours, mTime.mMinutes, -mTime.mSeconds);
			}
			else
			{
				sprintf(&pGameTimerText[0], "TIME: 0%i:0%i:0%i", mTime.mHours, mTime.mMinutes, mTime.mSeconds);
			}
		}

		else if (mTime.mHours < 10 && mTime.mMinutes < 10 && mTime.mSeconds >= 10)
		{
			sprintf(&pGameTimerText[0], "TIME: 0%i:0%i:%i", mTime.mHours, mTime.mMinutes, mTime.mSeconds);
		}

		else if (mTime.mHours < 10 && mTime.mMinutes >= 10 && mTime.mSeconds < 10)
		{
			sprintf(&pGameTimerText[0], "TIME: 0%i:%i:0%i", mTime.mHours, mTime.mMinutes, mTime.mSeconds);
		}

		else if (mTime.mHours < 10 && mTime.mMinutes >= 10 && mTime.mSeconds >= 10)
		{
			sprintf(&pGameTimerText[0], "TIME: 0%i:%i:%i", mTime.mHours, mTime.mMinutes, mTime.mSeconds);
		}

		else if (mTime.mHours >= 10 && mTime.mMinutes < 10 && mTime.mSeconds < 10)
		{
			sprintf(&pGameTimerText[0], "TIME: %i:0%i:0%i", mTime.mHours, mTime.mMinutes, mTime.mSeconds);
		}

		else if (mTime.mHours >= 10 && mTime.mMinutes < 10 && mTime.mSeconds >= 10)
		{
			sprintf(&pGameTimerText[0], "TIME: %i:0%i:%i", mTime.mHours, mTime.mMinutes, mTime.mSeconds);
		}

		else if (mTime.mHours >= 10 && mTime.mMinutes >= 10 && mTime.mSeconds < 10)
		{
			sprintf(&pGameTimerText[0], "TIME: %i:%i:0%i", mTime.mHours, mTime.mMinutes, mTime.mSeconds);
		}

		else if (mTime.mHours >= 10 && mTime.mMinutes >= 10 && mTime.mSeconds >= 10)
		{
			sprintf(&pGameTimerText[0], "TIME: %i:%i:%i", mTime.mHours, mTime.mMinutes, mTime.mSeconds);
		}
	}
}