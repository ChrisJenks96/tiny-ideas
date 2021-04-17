#include "object.h"

float hitExpiryTimer = 0.0f;

void cMoveable::Create(int iSize, float fX, float fY, float fSpeed, float fMaxVal)
{
	mSize = iSize;
	mX = fX;
	mY = fY;
	mVelX = 0.0f;
	mVelY = 0.0f;
	mVelSpeed = fSpeed;
	mVelMax = fMaxVal;
	mIsCollidable = true;
	mRot = 0.0f;
}

void cMoveable::Reset(float fX, float fY)
{
	mX = fX;
	mY = fY;
	mVelX = 0.0f;
	mVelY = 0.0f;
	mRot = 0.0f;
}

bool cMoveable::CollisionUpdate(float fOtherPosX, float fOtherPosY, int iOtherSize)
{
	if (mY < (fOtherPosY + iOtherSize) && mY > (fOtherPosY - mSize) && 
		mX < (fOtherPosX + iOtherSize) && mX > (fOtherPosX - mSize))
	{
		return true;
	}

	return false;
}

void cMainShip::Create()
{
	cMoveable::Create(SHIP_SIZE, HALF_SCR_WIDTH - SHIP_SIZE, 
		SCR_HEIGHT - SHIP_SIZE, 4.0f, 30.0f);
	mFuelRemaining = FUEL_START_VALUE;
	mHitObject = false;
}

void cMainShip::Reset()
{
	cMoveable::Reset(HALF_SCR_WIDTH - SHIP_SIZE, 
		SCR_HEIGHT - SHIP_SIZE);
	mFuelRemaining = FUEL_START_VALUE;
	mHitObject = false;
	mThrottle = false;
}

void cMainShip::BoostTrajectoryUpdate(float& rBoostRot)
{
	if (!mHitObject)
	{
		mHitObject = true;

		//get normalised dir of the boost we hit 	
		//float fMagnitude = sqrt((pShip->mMovObj.mVelX*pShip->mMovObj.mVelX) + 
			//(pShip->mMovObj.mVelY*pShip->mMovObj.mVelY));
		//printf("pfMagnitude : %f\n", fMagnitude);

		mVelX += sinf(DEG_TO_RAD(rBoostRot)) * MAX_VEL;
		mVelY += cosf(DEG_TO_RAD(rBoostRot)) * MAX_VEL;

		//add some fuel to the repo
		mFuelRemaining += FUEL_SINGLE_VALUE;
		if (mFuelRemaining > (FUEL_SINGLE_VALUE * MAX_FUEL_CELLS))
		{
			mFuelRemaining = FUEL_SINGLE_VALUE * MAX_FUEL_CELLS;
		}
	}
}

void cMainShip::Update(bool* pKeys, float fDt)
{
	mThrottle = false;
	if (pKeys[GLFW_KEY_W] && !(mFuelRemaining < 0.1f))
	{
		mThrottle = true;
		mVelY += mVelSpeed * fDt;
		mFuelRemaining -= FUEL_CONSUMPTION_RATE * fDt;
	}

	else
	{
		mVelY -= GRAVITY_Y_VAL * fDt;
	}

	if (mVelY > mVelMax)
	{
		mVelY = mVelMax;
	}

	if (pKeys[GLFW_KEY_A])
	{
		mVelX += mVelSpeed * fDt;
	}

	if (pKeys[GLFW_KEY_D])
	{
		mVelX -= mVelSpeed * fDt;
	}

	if (mVelX > mVelMax)
	{
		mVelX = mVelMax;
	}

	mX -= mVelX * fDt;
	mY -= mVelY * fDt;

	if (mY > (SCR_HEIGHT - mSize))
	{
		mY = SCR_HEIGHT - mSize;
		mVelY = 0.0f;
		mVelX = 0.0f;
	}

	//make sure to take the boost hit object collision off
	if (mHitObject)
	{
		hitExpiryTimer += 1.0f * fDt;
		if (hitExpiryTimer > BOOST_REACTIVATION_WAIT_TIME)
		{
			mHitObject = false;
			hitExpiryTimer = 0.0f;
		}
	}

	//prevent out of bounds
	if ((mX + SHIP_SIZE) > SCR_WIDTH)
	{
		mVelX = 0.0f;
		mX = SCR_WIDTH - SHIP_SIZE;
	}

	if (mX < 0)
	{
		mVelX = 0.0f;
		mX = 0.1f;
	}
}