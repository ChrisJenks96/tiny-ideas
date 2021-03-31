#include "draw.h"
#include "ship.h"

#define SHIP_SIZE 32
#define SKY_TRANSITION_Y 200
#define BKG_FADE_SPEED 0.1f

int main(int argc, char** argv)
{
	bool bErr = drawInit();
	if (bErr == false)
	{
		return -1;
	}

	int iShipPosX = (SCR_WIDTH >> 1) - SHIP_SIZE; 
	int iShipPosY = SCR_HEIGHT - SHIP_SIZE;
	float fBkgFadeFactor = 1.0f;

	//load statically held textures
	GLuint uiT1 = drawTextureInit(ship_bmp, SHIP_SIZE, SHIP_SIZE);
	
	while (!bQuit)
	{
		bQuit = glfwWindowShouldClose(pWindow);
		drawClear(0.33f * fBkgFadeFactor, 0.56f * fBkgFadeFactor, 
			0.80f * fBkgFadeFactor);

		drawTextureBind(uiT1);
		drawQuad(iShipPosX, iShipPosY, 10, 10, 10 + SHIP_SIZE, 10 + SHIP_SIZE);

		fBkgFadeFactor -= (!(iShipPosY % SKY_TRANSITION_Y)) ? BKG_FADE_SPEED : 0.0f;
		
		drawSwapBuffers();
	}

	drawTextureFree(uiT1);
	drawFree();
	return 0;
}