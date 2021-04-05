#pragma once

#include "global.h"

typedef struct sMoveableObject
{
	int mSize;
	float mX, mY;
	float mVelX, mVelY;
	float mVelSpeed;
	float mRot;
	float mVelMax;
	bool mIsCollidable;
} sMoveableObject;

sMoveableObject moveableObjectCreate(int iSize, float fX, float fY, float fSpeed, float fMaxVal);
bool moveableObjectCollisionUpdate(sMoveableObject* pMO, float fOtherPosX, float fOtherPosY, int iOtherSize);

typedef struct sMainShipObject
{
	sMoveableObject mMovObj;
	float mFuelRemaining;
	bool mHitObject;
} sMainShipObject;

extern float hitExpiryTimer;

sMainShipObject mainShipObjectCreate();
void mainShipObjectUpdate(sMainShipObject* pMSO, bool* pKeys, float fDt);