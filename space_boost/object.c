#include "object.h"

float hitExpiryTimer = 0.0f;

sMoveableObject moveableObjectCreate(int iSize, float fX, float fY, float fSpeed, float fMaxVal)
{
	sMoveableObject obj;
	obj.mSize = iSize;
	obj.mX = fX;
	obj.mY = fY;
	obj.mVelX = 0.0f;
	obj.mVelY = 0.0f;
	obj.mVelSpeed = fSpeed;
	obj.mVelMax = fMaxVal;
	obj.mIsCollidable = true;
	obj.mRot = 0.0f;
	return obj;
}

bool moveableObjectCollisionUpdate(sMoveableObject* pMO, float fOtherPosX, float fOtherPosY, int iOtherSize)
{
	if (pMO->mY < (fOtherPosY + iOtherSize) && pMO->mY > (fOtherPosY - pMO->mSize) && 
		pMO->mX < (fOtherPosX + iOtherSize) && pMO->mX > (fOtherPosX - pMO->mSize))
	{
		return true;
	}

	return false;
}

sMainShipObject mainShipObjectCreate()
{
	sMainShipObject obj;
	obj.mMovObj = moveableObjectCreate(SHIP_SIZE, HALF_SCR_WIDTH - SHIP_SIZE, 
		SCR_HEIGHT - SHIP_SIZE, 4.0f, 20.0f);
	obj.mFuelRemaining = FUEL_START_VALUE;
	obj.mHitObject = false;
	return obj;
}

void mainShipObjectUpdate(sMainShipObject* pMSO, bool* pKeys, float fDt)
{
	if (pKeys[GLFW_KEY_W] && !(pMSO->mFuelRemaining < 0.1f))
	{
		pMSO->mMovObj.mVelY += pMSO->mMovObj.mVelSpeed * fDt;
		pMSO->mFuelRemaining -= FUEL_CONSUMPTION_RATE * fDt;
	}

	else
	{
		pMSO->mMovObj.mVelY -= GRAVITY_Y_VAL * fDt;
	}

	if (pMSO->mMovObj.mVelY > pMSO->mMovObj.mVelMax)
	{
		pMSO->mMovObj.mVelY = pMSO->mMovObj.mVelMax;
	}

	if (pKeys[GLFW_KEY_A])
	{
		pMSO->mMovObj.mVelX += pMSO->mMovObj.mVelSpeed * fDt;
	}

	if (pKeys[GLFW_KEY_D])
	{
		pMSO->mMovObj.mVelX -= pMSO->mMovObj.mVelSpeed * fDt;
	}

	pMSO->mMovObj.mX -= pMSO->mMovObj.mVelX * fDt;
	pMSO->mMovObj.mY -= pMSO->mMovObj.mVelY * fDt;

	if (pMSO->mMovObj.mY > (SCR_HEIGHT - pMSO->mMovObj.mSize))
	{
		pMSO->mMovObj.mY = SCR_HEIGHT - pMSO->mMovObj.mSize;
		pMSO->mMovObj.mVelY = 0.0f;
		pMSO->mMovObj.mVelX = 0.0f;
	}

	//make sure to take the boost hit object collision off
	if (pMSO->mHitObject)
	{
		hitExpiryTimer += 1.0f * fDt;
		if (hitExpiryTimer > 4.0f)
		{
			pMSO->mHitObject = false;
			hitExpiryTimer = 0.0f;
		}
	}
}