#include "draw.h"

int main(int argc, char** argv)
{
	bool bErr = drawInit();
	if (bErr == false)
	{
		return -1;
	}

	while (!bQuit)
	{
		bQuit = glfwWindowShouldClose(pWindow);
		drawClear();

		drawQuad(20, 20, 10, 10, 20, 20);

		drawSwapBuffers();		
	}

	drawFree();
	return 0;
}