#pragma once

#include "global.h"

typedef struct sMoveableObject
{
	int mSize;
	float mX, mY;
	float mVel;
	float mVelSpeed;
	float mVelMax;
	bool mIsCollidable;
} sMoveableObject;

sMoveableObject moveableObjectCreate(int iSize, float fX, float fY, float fSpeed, float fMaxVal);
void moveableObjectUpdate(sMoveableObject* pMO, bool bCanAcc, bool* pKeys, float fDt);
bool moveableObjectCollisionUpdate(sMoveableObject* pMO, float fOtherPosX, float fOtherPosY, int iOtherSize);

typedef struct sMainShipObject
{
	sMoveableObject mMovObj;
	float mFuelRemaining;
} sMainShipObject;

sMainShipObject mainShipObjectCreate();
void mainShipObjectUpdate(sMainShipObject* pMSO, bool* pKeys, float fDt);