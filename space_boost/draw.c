#include "draw.h"

GLFWwindow* pWindow = 0;
bool bQuit = true;
bool bKeys[256];

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//false if GLFW_RELEASE
	bKeys[key] = (action == GLFW_PRESS || action == GLFW_REPEAT) ? true : false;
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
	
	glfwSetErrorCallback(errorCallback);

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
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void drawClear(float fR, float fG, float fB, float fCamX, float fCamY)
{
	glClearColor(fR, fG, fB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (double)SCR_WIDTH, 0.0, (double)SCR_HEIGHT, 0.0, 1.0);
	glTranslatef(fCamX, fCamY, 0.0f);
}

void drawSwapBuffers()
{
	glfwSwapBuffers(pWindow);
	glfwPollEvents();
}

void drawQuad(float fX, float fY, float fX2, float fY2)
{
	//flip y
	fY = (SCR_HEIGHT - fY) - fY2;

	glBegin(GL_QUADS);
	glTexCoord2i(0,0); glVertex2f(fX, fY);
	glTexCoord2i(1,0); glVertex2f(fX + fX2, fY);
	glTexCoord2i(1,1); glVertex2f(fX + fX2, fY + fY2);
	glTexCoord2i(0,1); glVertex2f(fX, fY + fY2);
	glEnd();
}

void drawQuadSection(float fX, float fY, float fWidth, float fHeight,
	float fX2, float fY2, float fTCX, float fTCY, float fTCX2, float fTCY2)
{
	//correction to get y on the correct position
	fY = (SCR_HEIGHT - fY) - fY2;

	//correct the y axis on the tex coords also
	fTCY = fHeight - fTCY;
	fTCY2 = fHeight - fTCY2;

	glBegin(GL_QUADS);
	glTexCoord2f(fTCX,fTCY2); glVertex2f(fX, fY);
	glTexCoord2f(fTCX2,fTCY2); glVertex2f(fX + fX2, fY);
	glTexCoord2f(fTCX2,fTCY); glVertex2f(fX + fX2, fY + fY2);
	glTexCoord2f(fTCX,fTCY); glVertex2f(fX, fY + fY2);
	glEnd();
}

void drawText(float fX, float fY, float fQuadSize, const char* cStr)
{
	float fOriginalfX = fX;

	//the size of each quad in GL
	float sMaxCharInRow = fX + (FONT_MAX_CHARS_PER_ROW * fQuadSize);

	//width and height are the same, will always be multiple of 8
	float max = (1.0f / FONT_SIZE) * FONT_CHAR_SIZE;

	while (*cStr != '\0')
	{
		uint16_t ucStrCharVal = (*cStr) - 32; //ascii val to bitmap index
		float fUVX = (ucStrCharVal % FONT_CHARS_PER_ROW) * max;
		float fUVY = (ucStrCharVal / FONT_CHARS_PER_ROW) * max;

		drawQuadSection(fX, fY, FONT_SIZE, FONT_SIZE, //pos and font details
			fQuadSize, fQuadSize, //the quad size
			fUVX, fUVY, fUVX+max, fUVY+max); //the tex coords offset

		fX += fQuadSize;
		if (fX >= sMaxCharInRow)
		{
			fX = fOriginalfX;
			fY += fQuadSize;
		}

		cStr++;
	}
}

void drawFree()
{
	if (pWindow != NULL)
	{
		glfwDestroyWindow(pWindow);
	}
	
	glfwTerminate();
}

GLuint drawTextureInit(const unsigned char* pBuffer, int iW, int iH)
{
	GLuint uiTID;
	glGenTextures(1, &uiTID);
	glBindTexture(GL_TEXTURE_2D, uiTID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iW, iH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
	return uiTID;
}

void drawTextureBind(GLuint uiTID)
{
	glBindTexture(GL_TEXTURE_2D, uiTID);
}

void drawTextureFree(GLuint uiTID)
{
	glDeleteTextures(1, &uiTID);
}