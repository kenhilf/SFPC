// This was replacement BMP loading code I found on the gamedev.net
// forums to stand in for the NeHe OpenGL code that relies on GLaux.
// Looking into better code to replace this.  Image class can easily
// be set up to load from multiple formats in the future.
// KJH 01/28/10
////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
#include <windows.h>		// Header File For Windows - has structures for BMP format
#include <stdio.h>	    	// Header File For Standard Input/Output
#include <stdlib.h>
#include "BMP.h"

/*------------------------------------------------------------------
 BMP Loader - a quick and dirty substitute for GLaux 
 if you only use GLaux to load BMP files will load any format of a 
 windows DIB BMP format graphics file Only works on a windows box   
 Caution! memory for the data is allocated using 'new'.  
 In the NeHe tutorials the memory is reclaimed using 'free'.   
 For the small tutorials its not a big deal but not a good practice in 
 larger projects (heap trashing not good). J.M. Doyle : 12 Jan 2003
------------------------------------------------------------------*/

AUX_RGBImageRec *auxDIBImageLoad(const char *FileName)
{ 
	 return new AUX_RGBImageRec(FileName);
}


void AUX_RGBImageRec::convertBGRtoRGB()
{
	const DWORD BitmapLength = sizeX * sizeY * 3;
	byte Temp;  // not quick but it works  
	for(DWORD i=0; i< BitmapLength; i += 3) 
	{
	    Temp = data[i];
	    data[i] = data[i+2];
	    data[i+2] = Temp;
    }
}

AUX_RGBImageRec::AUX_RGBImageRec(const char *FileName): data(NULL), NoErrors(false)
{ 
 loadFile(FileName);
}

AUX_RGBImageRec::~AUX_RGBImageRec()
{
  if (data != NULL) delete data;
  data = NULL;
}

bool AUX_RGBImageRec::loadFile(const char* Filename)
{
	BITMAPINFO BMInfo;								// need the current OpenGL device contexts in order to make use of windows DIB utilities  
	const HDC gldc = wglGetCurrentDC();   			// a handle for the current OpenGL Device Contexts
	NoErrors = false;  								// assume there are errors until file is loaded successfully into memory  

	if(data != NULL) delete data;					// release old data since this object could be used to load multiple Textures  
	
	BMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);   // windows needs this info to determine what header info we are looking for  
	BMInfo.bmiHeader.biBitCount = 0;					  // Get windows to determine color bit depth in the file for us

	// Get windows to open and load the BMP file and handle the messy decompression if the file is compressed
	// assume perfect world and no errors in reading file, Ha Ha
	HANDLE DIBHandle = LoadImage(0,Filename, IMAGE_BITMAP, 0, 0,LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);   

	if (DIBHandle == NULL)
		DWORD foo = GetLastError();

	GetDIBits(gldc, (HBITMAP)DIBHandle, 0,0, NULL, &BMInfo, DIB_RGB_COLORS); // use windows to get header info of bitmap - assume no errors in header format
	sizeX = BMInfo.bmiHeader.biWidth;
	sizeY = BMInfo.bmiHeader.biHeight;				  
	BMInfo.bmiHeader.biBitCount = 24;				// change color depth to 24 bits (3 bytes (BGR) / pixel)
	BMInfo.bmiHeader.biCompression = BI_RGB;		// don't want the data compressed 
	const DWORD BitmapLength = sizeX * sizeY * 3;	// 3 bytes (BGR) per pixel (24bp)  
													  
	data = new byte[BitmapLength];					// allocate enough memory to hold the pixel data in client memory
	
	// Get windows to do the dirty work of converting the BMP into the format needed by OpenGL  
	// if file is already 24 bit color then this is a waste of time but makes for short code  
	// Get the actual Texel data from the BMP object  
	if (GetDIBits(gldc, (HBITMAP)DIBHandle, 0, sizeY, data, &BMInfo, DIB_RGB_COLORS)) 
	{
		NoErrors = true;
		convertBGRtoRGB();							// NOTE: BMP is in BGR format but OpenGL needs RGB unless you use GL_BGR_EXT
	}

	DeleteObject(DIBHandle);						// don't need the BMP Object anymore  
	return NoErrors;
}        

bool AUX_RGBImageRec::Flip()
{
	// This function is used to flip bottom-up .BMP images
	byte *pSrcBuffer = data; // We will free this one
	int width = sizeX;
	int height = sizeY;
	int bpp = 24;

	// Calculate pitch (end of row must align on 4-byte address)
	int srcPitchBytes = width * (bpp/8);
	while (srcPitchBytes % 4 != 0)
		++srcPitchBytes; // Add padding bytes

	int srcBytesPerRow = srcPitchBytes;
	int dstBytesPerRow = width * bpp/8;

	// Allocate new destination buffer
	UCHAR *pDstBuffer = (UCHAR*)malloc(dstBytesPerRow * height);
	data = pDstBuffer; // Replace old one

	// Flip vertically
	for (int index=0; index < height; index++)
	{
		memcpy(
		&pDstBuffer[((height-1) - index)*dstBytesPerRow],
		&pSrcBuffer[index*srcBytesPerRow],
		dstBytesPerRow
        );
	}

	// Release the old memory buffer
	free(pSrcBuffer);

	// Return success
	return true;
}