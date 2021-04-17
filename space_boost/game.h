#pragma once

#include "global.h"
#include "object.h"
#include <unistd.h>

typedef struct sTime
{
	int mSeconds;
	int mMinutes;
	int mHours;
} sTime;

typedef struct sFrameTime
{
	//frame timing
	double mNowTime;
	double mLastTime;
	float mDeltaTime;
	float mFPSCounter;
	int mFrames;
} sFrameTime;

class cGame
{
	public:
		bool Init();
		void BackgroundUpdate(cMainShip& rShip);
		bool SaveOrLoad(bool bSave);
		void CameraUpdate(cMainShip& rShip);
		void BoostGetStartEndID(int* pStart, int* pEnd);
		void TimeUpdate();
		void ShowFPS();
		void UpdateScores(cMainShip& rShip);
		float& GetCamY(){return mCamY;}
		const float GetGameTimer(){return mGameTimer;}
		sFrameTime& GetFrameTime(){return mFrameTime;}
		sTime& GetTime(){return mTime;}
		float& GetBackgroundFadeFactor(){return mBkgFadeFactor;}
		void TimerUpdate(char* pGameTimerText);
		const int& GetCurrentHighScore(){return mCurrentHighScore;}
		const int& GetCurrentAltitude(){return mCurrentAltitude;}
	private:
		int mCurrentHighScore;
		int mCurrentAltitude;
		float mBkgFadeFactor;
		float mCamY;
		sTime mTime;
		sFrameTime mFrameTime;
		float mGameTimer;
};