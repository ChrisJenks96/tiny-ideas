#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	//[filename] [width] [height] [export name]
	//divisable by 8 only
	//255 0 255 is transparency
	if (argc == 5)
	{
		const char* pImportName = argv[1];
		int iWidth = strtol(argv[2], NULL, 10);
		int iHeight = strtol(argv[3], NULL, 10);
		const char* pExportName = argv[4];

		FILE* pFile = fopen(pImportName, "rb");

		if (pFile != NULL)
		{
			//get the filesize of the bmp
			fseek(pFile, 0, SEEK_END);
			int iFileSize = ftell(pFile);
			//54 byte offset is where all the pixels are
			fseek(pFile, 54, SEEK_SET);

			//create the RGBA buffer we're going to write the 24 bit bmp too
			unsigned char* pBMP = (unsigned char*)malloc(iFileSize-54);
			unsigned char* pRGBABuffer = (unsigned char*)malloc(iWidth * iHeight * 4);

			fread(&pBMP[0], iFileSize, 1, pFile);
			fclose(pFile);

			//write the bmp to our rgba buffer
			int iCount = 0, iRGBACount = 0;
			while (iRGBACount < (iWidth * iHeight * 4))
			{
				//flip the BGR to RGB
				pRGBABuffer[iRGBACount] = pBMP[iCount+2];
				pRGBABuffer[iRGBACount+1] = pBMP[iCount+1];
				pRGBABuffer[iRGBACount+2] = pBMP[iCount];
				//if the transparency key is used, then make the alpha component 100%
				pRGBABuffer[iRGBACount+3] = ((pBMP[iCount] == 255) && (pBMP[iCount+1] == 0) && (pBMP[iCount+2] == 255)) ? 0 : 255;
				iCount += 3;
				iRGBACount += 4;
			}

			FILE* pOut = fopen(pExportName, "wb");
			if (pOut != NULL)
			{
				fwrite(&pRGBABuffer[0], iWidth * iHeight * 4, 1, pOut);
				fclose(pOut);
			}

			free(pBMP);
			free(pRGBABuffer);	
		}
	}

	else
	{
		printf("bmpstrip [bmp w/ extension] [width] [height] [export bin name]");
	}

	return 0;
}