#include "draw.h"

bool bKeys[256];
char cLastKey = -1;

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

bool cDraw::Init()
{
	mQuit = false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	if (!glfwInit())
	{
		return false;
	}
	
	glfwSetErrorCallback(errorCallback);

	mWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Space Boost", NULL, NULL);
	if (!mWindow)
	{
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(mWindow, keyCallback);
	glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(0);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void cDraw::Clear(float fR, float fG, float fB, float fCamY)
{
	glClearColor(fR, fG, fB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (double)SCR_WIDTH, 0.0, (double)SCR_HEIGHT, 0.0, 1.0);
	glTranslatef(0.0f, fCamY, 0.0f);
}

void cDraw::SwapBuffers()
{
	glfwSwapBuffers(mWindow);
	glfwPollEvents();
}

void cDraw::PushMatrix()
{
	glPushMatrix();
}

void cDraw::PopMatrix()
{
	glPopMatrix();
}

void cDraw::Color3f(float fR, float fG, float fB)
{
	glColor3f(fR, fG, fB);
}

void cDraw::Line(float fX, float fY, float fX2, float fY2, double fThick)
{
	//flip y
	fY = (SCR_HEIGHT - fY) - fY2;

	glLineWidth(fThick);
	
	glBegin(GL_LINES);
	glVertex2f(fX, fY);
	glVertex2f(fX + fX2, fY + fY2);
	glEnd();
}

void cDraw::Box(float fX, float fY, float fX2, float fY2, float fThick)
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

void cDraw::TransformQuad(float fX, float fY, float fX2, float fY2, float* fRotVal)
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

void cDraw::TransformPoint(float fX, float fY)
{
	//flip y
	fY = (SCR_HEIGHT - fY);
	glTranslatef(fX, fY, 0.0f);
}

void cDraw::Quad()
{
	glBegin(GL_QUADS);
	glTexCoord2i(0,0); glVertex2f(0.0f, 0.0f);
	glTexCoord2i(1,0); glVertex2f(1.0f, 0.0f);
	glTexCoord2i(1,1); glVertex2f(1.0f, 1.0f);
	glTexCoord2i(0,1); glVertex2f(0.0f, 1.0f);
	glEnd();
}

void cDraw::Point()
{
	glBegin(GL_POINTS);
	glVertex2f(0.0f, 0.0f);
	glEnd();
}

void cDraw::QuadSection(float fX, float fY, float fWidth, float fHeight,
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

void cDraw::Text(float fX, float fY, float fQuadSize, float fTextSepSize, const char* cStr, int iLen, bool bCentre)
{
	//if a length is submitted but the string is null, check the first elem to make sure
	if (*cStr == 0)
	{
		return;
	}

	if (bCentre)
	{
		fX -= (fTextSepSize * iLen) / 2.0f;
	}

	float fOriginalfX = fX;

	//the size of each quad in GL
	float fMaxCharInRow = fX + (FONT_MAX_CHARS_PER_ROW * fQuadSize);

	//width and height are the same, will always be multiple of 8
	float fMax = (1.0f / FONT_SIZE) * FONT_CHAR_SIZE;

	int iCount = 0;
	while (iCount < iLen)
	{
		uint16_t ucStrCharVal = cStr[iCount] - 32; //ascii val to bitmap index
		float fUVX = (ucStrCharVal % FONT_CHARS_PER_ROW) * fMax;
		float fUVY = (ucStrCharVal / FONT_CHARS_PER_ROW) * fMax;

		QuadSection(fX, fY, FONT_SIZE, FONT_SIZE, //pos and font details
			fQuadSize, fQuadSize, //the quad size
			fUVX, fUVY, fUVX+fMax, fUVY+fMax); //the tex coords offset

		fX += fTextSepSize;
		if (fX >= fMaxCharInRow)
		{
			fX = fOriginalfX;
			fY += fQuadSize;
		}

		iCount++;
	}
}

void cDraw::Thrust(float fX, float fY, float* fThrustY, float fLength, float fSize, float fSpeed)
{
	PushMatrix();
	TransformQuad(fX, fY + *fThrustY, fSize, SMOKE_SPRITE_SIZE, NULL);
	Quad();
	PopMatrix();
}

void cDraw::Stars(float fXOffset, float* pCamY)
{
	int iCount = 0;
	int shipPos = (int)*pCamY + SCR_HEIGHT;
	TextureUnbind();
	Color3f(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < STAR_NUM; i+=2)
	{
		//if the star falls below the rocket, is out of scope, move it to the next frame
		if (shipPos < -cStarOffsets[i+1])
		{
			cStarOffsets[i+1] += (SCR_HEIGHT + 20);
		}

		//only draw if the star coords are within the screen frame
		else if (shipPos <= (-cStarOffsets[i+1]+SCR_HEIGHT))
		{
			glPointSize(cBoostXOffsets[iCount++] * 0.01f);
			PushMatrix();
			TransformPoint(cStarOffsets[i] + fXOffset, -cStarOffsets[i+1]);
			Point();
			PopMatrix();
		}
	}
}

void cDraw::Free()
{
	if (mWindow != NULL)
	{
		glfwDestroyWindow(mWindow);
	}
	
	glfwTerminate();
}

GLuint cDraw::TextureInit(const unsigned char* pBuffer, int iW, int iH)
{
	GLuint uiTID;
	glGenTextures(1, &uiTID);
	glBindTexture(GL_TEXTURE_2D, uiTID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iW, iH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
	return uiTID;
}

void cDraw::TextureBind(GLuint uiTID)
{
	glBindTexture(GL_TEXTURE_2D, uiTID);
}

void cDraw::TextureUnbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void cDraw::TextureFree(GLuint uiTID)
{
	glDeleteTextures(1, &uiTID);
}