#pragma once

#include "global.h"

#define MAX_VEL 20.0f
#define GRAVITY_Y_VAL 5.0f

typedef struct sMoveableObject
{
	int mSize;
	float mX, mY;
	float mVel;
	float mVelSpeed;
} sMoveableObject;

sMoveableObject moveableObjectCreate(int iSize, float fX, float fY, float fSpeed);
void moveableObjectUpdate(sMoveableObject* pMO, bool* pKeys, float fDt);