#include "draw.h"

GLFWwindow* pWindow = 0;
bool bQuit = true;

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		bQuit = true;
	}
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void errorCallback(int error, const char* description)
{
	//error printing functionality here...
}

bool drawInit()
{
	bQuit = false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	if (!glfwInit())
	{
		return false;
	}

	pWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Space Boost", NULL, NULL);
	if (!pWindow)
	{
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(pWindow, keyCallback);
	glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);

	glfwMakeContextCurrent(pWindow);
	glfwSwapInterval(0);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glEnable(GL_TEXTURE);

	glOrtho(0.0, (double)SCR_WIDTH, 0.0, (double)SCR_HEIGHT, 0.0, 1.0);

	return true;
}

void drawClear()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void drawSwapBuffers()
{
	glfwSwapBuffers(pWindow);
	glfwPollEvents();
}

void drawQuad(int iX, int iY, int sX, int sY, int sX2, int sY2)
{
	//flip y
	iY = (SCR_HEIGHT - iY) - sY2;

	glBegin(GL_QUADS);
	glTexCoord2i(0,0); glVertex2i(iX + sX,iY + sY);
	glTexCoord2i(1,0); glVertex2i(iX + sX2,iY +sY);
	glTexCoord2i(1,1); glVertex2i(iX + sX2,iY +sY2);
	glTexCoord2i(0,1); glVertex2i(iX + sX,iY +sY2);
	glEnd();
}

void drawFree()
{
	if (pWindow != NULL)
	{
		glfwDestroyWindow(pWindow);
	}
	
	glfwTerminate();
}