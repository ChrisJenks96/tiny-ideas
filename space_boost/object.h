#pragma once

#include "global.h"

typedef struct sMoveableObject
{
	int mSize;
	float mX, mY;
	float mVel;
	float mVelSpeed;
} sMoveableObject;

sMoveableObject moveableObjectCreate(int iSize, float fX, float fY, float fSpeed);
void moveableObjectUpdate(sMoveableObject* pMO, bool bCanAcc, bool* pKeys, float fDt);

typedef struct sMainShipObject
{
	sMoveableObject mMovObj;
	float mFuelRemaining;
} sMainShipObject;

sMainShipObject mainShipObjectCreate();
void mainShipObjectUpdate(sMainShipObject* pMSO, bool* pKeys, float fDt);