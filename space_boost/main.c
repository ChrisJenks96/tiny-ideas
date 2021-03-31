#include "draw.h"
#include "object.h"
#include "ship.h"

#define SHIP_SIZE 32
#define SKY_TRANSITION_Y 200
#define BKG_FADE_SPEED 0.1f

double dNowTime, dDeltaTime, dLastTime;

int main(int argc, char** argv)
{
	bool bErr = drawInit();
	if (bErr == false)
	{
		return -1;
	}

	sMoveableObject Ship1 = moveableObjectCreate(SHIP_SIZE, (SCR_WIDTH >> 1) - SHIP_SIZE, 
		SCR_HEIGHT - SHIP_SIZE, 4.0f);
	float fBkgFadeFactor = 1.0f;

	//load statically held textures
	GLuint uiT1 = drawTextureInit(ship_bmp, SHIP_SIZE, SHIP_SIZE);
	
	dLastTime = 0;

	while (!bQuit)
	{
		dNowTime = glfwGetTime();
		dDeltaTime = dNowTime - dLastTime;

		//updates

		//quit if the escape button is pressed or the window close event is triggered
		bQuit = glfwWindowShouldClose(pWindow) | bKeys[GLFW_KEY_ESCAPE];

		//fBkgFadeFactor -= (!((int)Ship1.mY % SKY_TRANSITION_Y)) ? BKG_FADE_SPEED : 0.0f;

		moveableObjectUpdate(&Ship1, &bKeys[0], (float)dDeltaTime);

		//rendering
		drawClear(0.33f * fBkgFadeFactor, 0.56f * fBkgFadeFactor, 
			0.80f * fBkgFadeFactor);

		drawTextureBind(uiT1);
		drawQuad(Ship1.mX, Ship1.mY, 10, 10, 10 + SHIP_SIZE, 10 + SHIP_SIZE);

		drawSwapBuffers();

		//printf("dt: %f\n", dDeltaTime);
		dLastTime = dNowTime;
	}

	drawTextureFree(uiT1);
	drawFree();
	return 0;
}