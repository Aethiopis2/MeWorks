//-----------------------------------------------------------------------------------------|
// PROJECT TITLE:		The Lazarus Project
//
// FILE NAME:			Image.h
// EXE NAME:			Optical Geez
// 
// PROGRAM DESC:		The following file contains the declearation of sets of prototypes 
//						that are used in the processing of images for various formats used 
//						in the project OCR.
//
// DATE CREATED:		5th of Oct 2017, Thursday (aka the hand of fate)
// LAST UPDATE:
//
// PROGRAM AUTHOR:		Dr. Rediet Worku aka Code Red
//-----------------------------------------------------------------------------------------|
#ifndef IMAGE_H
#define IMAGE_H

//-----------------------------------------------------------------------------------------|
// DEFINES
//-----------------------------------------------------------------------------------------|

// this is a standard identifer for bitmap files
#define BITMAP_ID		0x4D42		// 19778 in Decimal

//-----------------------------------------------------------------------------------------|
// TYPEDEFS
//-----------------------------------------------------------------------------------------|

// this structure represents a bitmap ("*.bmp") file format
typedef struct
{
	BITMAPFILEHEADER bfh;		// the file header containing info on file type and size
	BITMAPINFOHEADER bih;		// the info header containing info on bitmap; size and bit depth
	RGBQUAD	*palette;			// the palette info used for 8-bits and below
	UCHAR *pixels;				// the actual pixels indicating the conditions

} BITMAP_FILE, *BITMAP_FILE_PTR;

//-----------------------------------------------------------------------------------------|
// INCLUDES
//-----------------------------------------------------------------------------------------|
#include <Windows.h>

//-----------------------------------------------------------------------------------------|
// GLOBALS/EXTERNS
//-----------------------------------------------------------------------------------------|

// these external's are used in relation to bitmap file's
extern BITMAP_FILE	bitmap;		// this hold's our bitmap data in memory
extern UCHAR *binary_buffer;	// this store's the bitmap pixels in a much more convienient
								//	manner; i.e. 0 for black, 1 for white.
extern int buffer_height;		// the height of the bitmap buffer above
extern int buffer_width;		// the corresponding width

//-----------------------------------------------------------------------------------------|
// PROTOTYPES
//-----------------------------------------------------------------------------------------|

int Load_Bitmap_File(BITMAP_FILE_PTR pBitmap, char *filename);
void Flip_Bitmap(UCHAR *buffer, int width, int height);
void Unload_Bitmap(BITMAP_FILE_PTR pBitmap);
int Bitmap_Monochrome(BITMAP_FILE_PTR pBitmap);

//-----------------------------------------------------------------------------------------|
// FUNCTIONS
//-----------------------------------------------------------------------------------------|

#endif
//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|