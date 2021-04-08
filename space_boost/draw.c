#include "draw.h"

GLFWwindow* pWindow = 0;
bool bQuit = true;
bool bKeys[256];
char cLastKey = -1;
bool bLastKeyPressed = false;

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//false if GLFW_RELEASE
	bKeys[key] = (action == GLFW_PRESS || action == GLFW_REPEAT) ? true : false;

	if (action == GLFW_PRESS)
	{
		cLastKey = (char)key;
		//make it upper case for the font
		if (cLastKey >= 97)
		{
			cLastKey -= 32;
		}

		bLastKeyPressed = false;
	}	
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void errorCallback(int error, const char* description)
{
	//error printing functionality here...
	printf("%s\n", description);
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
	glEnable(GL_POINT_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void drawClear(float fR, float fG, float fB, float fCamY)
{
	glClearColor(fR, fG, fB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (double)SCR_WIDTH, 0.0, (double)SCR_HEIGHT, 0.0, 1.0);
	glTranslatef(0.0f, fCamY, 0.0f);
}

void drawSwapBuffers()
{
	glfwSwapBuffers(pWindow);
	glfwPollEvents();
}

void drawPushMatrix()
{
	glPushMatrix();
}

void drawPopMatrix()
{
	glPopMatrix();
}

void drawColor3f(float fR, float fG, float fB)
{
	glColor3f(fR, fG, fB);
}

void drawLine(float fX, float fY, float fX2, float fY2, double fThick)
{
	//flip y
	fY = (SCR_HEIGHT - fY) - fY2;

	glLineWidth(fThick);
	
	glBegin(GL_LINES);
	glVertex2f(fX, fY);
	glVertex2f(fX + fX2, fY + fY2);
	glEnd();
}

void drawBox(float fX, float fY, float fX2, float fY2, float fThick)
{
	fY = (SCR_HEIGHT - fY) - fY2;

	glLineWidth(fThick);
	
	glBegin(GL_LINES);
	glVertex2f(fX, fY);
	glVertex2f(fX + fX2, fY);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(fX + fX2, fY);
	glVertex2f(fX + fX2, fY + fY2);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(fX + fX2, fY + fY2);
	glVertex2f(fX, fY + fY2);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(fX, fY + fY2);
	glVertex2f(fX, fY);
	glEnd();
}

void drawTransformQuad(float fX, float fY, float fX2, float fY2, float* fRotVal)
{
	//flip y
	fY = (SCR_HEIGHT - fY) - fY2;

	float fHalfSpriteWidth = fX2 / 2.0f;
	float fHalfSpriteHeight = fY2 / 2.0f;

	glTranslatef(fX + fHalfSpriteWidth, fY + fHalfSpriteHeight, 0.0f); //translate to original point
	if (fRotVal)
	{
		glRotatef(fRotVal[0], fRotVal[1], fRotVal[2], fRotVal[3]); 
	}

	else
	{
		glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	glTranslatef(-fHalfSpriteWidth, -fHalfSpriteHeight, 0.0f); //translate to the centre of the object
	glScalef(fX2, fY2, 1.0f);
}

void drawQuad()
{
	glBegin(GL_QUADS);
	glTexCoord2i(0,0); glVertex2f(0.0f, 0.0f);
	glTexCoord2i(1,0); glVertex2f(1.0f, 0.0f);
	glTexCoord2i(1,1); glVertex2f(1.0f, 1.0f);
	glTexCoord2i(0,1); glVertex2f(0.0f, 1.0f);
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

void drawText(float fX, float fY, float fQuadSize, float fTextSepSize, const char* cStr, bool bCentre)
{
	if (bCentre)
	{
		fX -= (fTextSepSize * strlen(cStr)) / 2.0f;
	}

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

		fX += fTextSepSize;
		if (fX >= sMaxCharInRow)
		{
			fX = fOriginalfX;
			fY += fQuadSize;
		}

		cStr++;
	}
}

void drawThrust(float fX, float fY, float* fThrustY, float fLength, float fSize, float fSpeed)
{
	drawPushMatrix();
	drawTransformQuad(fX, fY + *fThrustY, fSize, SMOKE_SPRITE_SIZE, NULL);
	drawQuad();
	drawPopMatrix();
}

void drawStars(float fXOffset, float* pCamY)
{
	int iCount = 0;
	drawTextureUnbind();

	for (int i = 0; i < STAR_NUM; i+=2)
	{
		//if the star is out of scope, move it to the next frame
		if ((*pCamY+SCR_HEIGHT) < -cStarOffsets[i+1])
		{
			cStarOffsets[i+1] += (SCR_HEIGHT + 20);
		}

		float fSize = cBoostXOffsets[iCount++] * 0.01f;
		drawColor3f(fSize, fSize, fSize);
		drawPushMatrix();
		drawTransformQuad(cStarOffsets[i] + fXOffset, -cStarOffsets[i+1], fSize, fSize, NULL);
		drawQuad();
		drawPopMatrix();
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

void drawTextureUnbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawTextureFree(GLuint uiTID)
{
	glDeleteTextures(1, &uiTID);
}