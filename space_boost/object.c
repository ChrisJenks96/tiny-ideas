#include "object.h"

sMoveableObject moveableObjectCreate(int iSize, float fX, float fY, float fSpeed, float fMaxVal)
{
	sMoveableObject obj;
	obj.mSize = iSize;
	obj.mX = fX;
	obj.mY = fY;
	obj.mVel = 0.0f;
	obj.mVelSpeed = fSpeed;
	obj.mVelMax = fMaxVal;
	obj.mIsCollidable = true;
	return obj;
}

void moveableObjectUpdate(sMoveableObject* pMO, bool bCanAcc, bool* pKeys, float fDt)
{
	if (pKeys[GLFW_KEY_W] && bCanAcc)
	{
		pMO->mVel += pMO->mVelSpeed * fDt;
	}

	else
	{
		pMO->mVel -= GRAVITY_Y_VAL * fDt;
	}

	if (pMO->mVel > pMO->mVelMax)
	{
		pMO->mVel = pMO->mVelMax;
	}
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
	return obj;
}

void mainShipObjectUpdate(sMainShipObject* pMSO, bool* pKeys, float fDt)
{
	if (pKeys[GLFW_KEY_W])
	{
		pMSO->mFuelRemaining -= FUEL_CONSUMPTION_RATE * fDt;
	}

	moveableObjectUpdate(&pMSO->mMovObj, !(pMSO->mFuelRemaining < 0.1f), pKeys, fDt);

	pMSO->mMovObj.mY -= pMSO->mMovObj.mVel * fDt;

	if (pMSO->mMovObj.mY > (SCR_HEIGHT - pMSO->mMovObj.mSize))
	{
		pMSO->mMovObj.mY = SCR_HEIGHT - pMSO->mMovObj.mSize;
		pMSO->mMovObj.mVel = 0.0f;
	}
}