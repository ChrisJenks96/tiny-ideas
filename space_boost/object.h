#pragma once

#include "global.h"

extern float hitExpiryTimer;

class cMoveable
{
	public:
		void Create(int iSize, float fX, float fY, float fSpeed, float fMaxVal);
		void Reset(float fX, float fY);
		bool CollisionUpdate(float fOtherPosX, float fOtherPosY, int iOtherSize);
		int& GetSize(){return mSize;}
		void SetXY(float fX, float fY){mX = fX; mY = fY;}
		std::pair<float, float> GetXY(){return std::make_pair(mX, mY);}
		std::pair<float, float> GetVelXY(){return std::make_pair(mVelX, mVelY);}
	protected:
		int mSize;
		float mX, mY;
		float mVelX, mVelY;
		float mVelSpeed;
		float mRot;
		float mVelMax;
		bool mIsCollidable;
};

class cMainShip : public cMoveable
{
	public:
		void Create();
		void Reset();
		void BoostTrajectoryUpdate(float& rBoostRot);
		void Update(bool* pKeys, float fDt);
		bool& GetHitObject(){return mHitObject;}
		float& GetFuelRemaining(){return mFuelRemaining;}
		bool& GetThrottle(){return mThrottle;}
	private:
		float mFuelRemaining;
		bool mHitObject;
		bool mThrottle;
};