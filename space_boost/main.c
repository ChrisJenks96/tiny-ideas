#include "draw.h"
#include "ship.h"

int main(int argc, char** argv)
{
	bool bErr = drawInit();
	if (bErr == false)
	{
		return -1;
	}

	//load statically held textures
	GLuint t1 = drawTextureInit(ship_bmp, 32, 32);
	
	while (!bQuit)
	{
		bQuit = glfwWindowShouldClose(pWindow);
		drawClear();

		drawTextureBind(t1);
		drawQuad(20, 20, 10, 10, 42, 42);
		
		drawSwapBuffers();		
	}

	drawTextureFree(t1);
	drawFree();
	return 0;
}