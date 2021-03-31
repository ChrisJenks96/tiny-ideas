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

void moveableObjectUpdate(sMoveableObject* pMO, bool* pKeys, float fDt)
{
	if (pKeys[GLFW_KEY_W])
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

	printf("%f\n", pMO->mY);
}