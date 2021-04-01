#include "object.h"

sMoveableObject moveableObjectCreate(int iSize, float fX, float fY, float fSpeed)
{
	sMoveableObject obj;
	obj.mSize = iSize;
	obj.mX = fX;
	obj.mY = fY;
	obj.mVel = 0.0f;
	obj.mVelSpeed = fSpeed;
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

	if (pMO->mVel > MAX_VEL)
	{
		pMO->mVel = MAX_VEL;
	}
	
	pMO->mY -= pMO->mVel * fDt;

	if (pMO->mY > (SCR_HEIGHT - pMO->mSize))
	{
		pMO->mY = SCR_HEIGHT - pMO->mSize;
		pMO->mVel = 0.0f;
	}
}

sMainShipObject mainShipObjectCreate()
{
	sMainShipObject obj;
	obj.mMovObj.mSize = SHIP_SIZE;
	obj.mMovObj.mX = HALF_SCR_HEIGHT + SHIP_SIZE;
	obj.mMovObj.mY = SCR_HEIGHT - SHIP_SIZE;
	obj.mMovObj.mVel = 0.0f;
	obj.mMovObj.mVelSpeed = 4.0f;
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
}