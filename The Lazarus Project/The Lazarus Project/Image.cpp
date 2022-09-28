//-----------------------------------------------------------------------------------------|
// PROJECT TITLE:		The Lazarus Project
//
// FILE NAME:			Image.cpp
// EXE NAME:			Optical Geez
// 
// PROGRAM DESC:		The following file contains the defintion of sets of prototypes 
//						that are used in the processing of images for various formats used 
//						in the project OCR.
//
// DATE CREATED:		5th of Oct 2017, Thursday (aka the hand of fate)
// LAST UPDATE:
//
// PROGRAM AUTHOR:		Dr. Rediet Worku aka Code Red
//-----------------------------------------------------------------------------------------|
#define WIN32_LEAN_AND_MEAN			// no MFC thank-you.

//-----------------------------------------------------------------------------------------|
// DEFINES
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// TYPEDEFS
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// INCLUDES
//-----------------------------------------------------------------------------------------|
#include <Windows.h>
#include <stdio.h>
#include <malloc.h>

#include "Image.h"

//-----------------------------------------------------------------------------------------|
// GLOBALS/EXTERNS
//-----------------------------------------------------------------------------------------|
// these external's are used in relation to bitmap file's
BITMAP_FILE	bitmap;						// this hold's our bitmap data in memory
UCHAR *binary_buffer			= NULL;	// this store's the bitmap pixels in a much more convienient
										//	manner; i.e. 0 for black, 1 for white.
int buffer_height;						// the height of the bitmap buffer above
int buffer_width;						// the corresponding width

//-----------------------------------------------------------------------------------------|
// PROTOTYPES
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// FUNCTIONS
//-----------------------------------------------------------------------------------------|

int Load_Bitmap_File(BITMAP_FILE_PTR pBitmap, char *filename)
{
// this function open's up a bitmap file using the Win32 API functions and intializes
//	the contents of the structure recieved by the first parameter

	HANDLE file_handle = NULL;		// handle to the file object in Win32
	DWORD bytes_read;

	// open up the file
	file_handle = CreateFile(filename,				// the filename
							 GENERIC_READ,			// The access mode
							 0, NULL,				// SHARING MODES AND SECURITY
							 OPEN_EXISTING,			// FILE must exist
							 FILE_ATTRIBUTE_NORMAL,	// normal attribured file
							 NULL);					// templates

	// test for a valid-handle
	if (file_handle == INVALID_HANDLE_VALUE)
		return(0);		// no such file

	// now read-in the contents of the file header
	ReadFile(file_handle, &pBitmap->bfh, sizeof(BITMAPFILEHEADER), &bytes_read, NULL);

	// now test if this is a bitmap file
	if (pBitmap->bfh.bfType != BITMAP_ID)
	{
		// close the file
		CloseHandle(file_handle);

		// return not bitmap-file code error
		return(0);		// not a bitmap file
	} // end if

	// so we've got a bitmap file, read in the info header
	//	for more information about the bitmap
	ReadFile(file_handle, &pBitmap->bih, sizeof(BITMAPINFOHEADER), &bytes_read, NULL);

	// set the globals buffer_width and buffer_height
	buffer_height	= pBitmap->bih.biHeight;
	buffer_width	= pBitmap->bih.biWidth;

	// now test for the color depth, and read in the palette structure
	//	if there is one.
	if (pBitmap->bih.biBitCount <= 8)
	{
		// yes, there exists a color table, allocate memory for it
		//	and read-it in

		// test for a pre-allocated memory and
		//	free it in
		if (pBitmap->palette)
		{
			free(pBitmap->palette);
			pBitmap->palette = NULL;		// the double-tap rule
		} // end if pre-allocated

		pBitmap->palette = (RGBQUAD *)malloc(sizeof(RGBQUAD) << (pBitmap->bih.biBitCount));

		// test for a valid mem
		if (!pBitmap->palette)
		{
			// close the file
			CloseHandle(file_handle);

			// return fail code
			return(0);			// General malloc fail
		} // end if no pointer to palette

		// now read in the palette data
		ReadFile(file_handle, pBitmap->palette, sizeof(RGBQUAD) << pBitmap->bih.biBitCount, &bytes_read, NULL);

	} // end if there is palette

	// test for a pre-allocated memory on the pixel
	//	and free it
	if (pBitmap->pixels)
	{
		free(pBitmap->pixels);
		pBitmap->pixels	= NULL;
	} // end if pre-allocated pixel buffer

	// at this point we're left with the pixel data, allocate 
	//	memory for it
	pBitmap->pixels = (UCHAR *)malloc(pBitmap->bih.biSizeImage);	// the image size

	// now test for a valid memory
	if (!pBitmap->pixels)
	{
		// close the file
		CloseHandle(file_handle);

		// return fail code
		return(0);
	} // end if

	// now at this point, read-in the pixel data
	ReadFile(file_handle, pBitmap->pixels, pBitmap->bih.biSizeImage, &bytes_read, NULL);

	// now filp the bitmap if it needs be
	// test for flipped codes
	if (buffer_height < 0 && pBitmap->bih.biBitCount > 1)
	{
		// bitmap is flipped
		buffer_height *= -1;

		// do flip
		Flip_Bitmap(pBitmap->pixels, (buffer_width * pBitmap->bih.biBitCount) >> 3,
			buffer_height);

	} // end if

	// we're done close, the file
	CloseHandle(file_handle);

	// return success
	return(1);

} // end Load_Bitmap_File

//-----------------------------------------------------------------------------------------|

void Unload_Bitmap(BITMAP_FILE_PTR pBitmap)
{
// this function unload's (frees) up the memory sent by
//	the parameter

	// test for allocated mem
	if (pBitmap->pixels)
	{
		free(pBitmap->pixels);
		pBitmap->pixels = NULL;		// double-tap rule
	} // end if pixels

	// free the palatte if used
	if (pBitmap->palette)
	{
		free(pBitmap->palette);
		pBitmap->palette = NULL;	// double-tap
	} // end if palette

} // end Unload_Bitmap

//-----------------------------------------------------------------------------------------|

void Flip_Bitmap(UCHAR *buffer, int width, int height)
{
// flip's a bottom-up DIB into its normal up-right orientation. The algorithim
//	flips the image by copying one line at a time.

	UCHAR *alias = buffer;		// alias pointer to navigate
	UCHAR *temp = (UCHAR *)malloc(width*height);

	// copy mem
	memcpy(temp, buffer, width*height);

	// starting from the bottom copy the bitmap
	//	line by line
	for (int i = height-1; i >= 0; i--)
	{
		memcpy(alias, &temp[i*width], width);

		// advance pointer
		alias += width;
	} // end for

} // end Flip

//-----------------------------------------------------------------------------------------|

int Bitmap_Monochrome(BITMAP_FILE_PTR pBitmap)
{
// this function decodes the contents of a monochrome bitmap file and
//	initalizes the data in the more convienent linear array buffer.

	int i, j, k;		// looping vars; two forward one backwards (for flipping)
	int scan_line;		// store's the width of the buffer according to the file storage
		// format

	// allocate memory for the global buffer
	if ( !(binary_buffer = (UCHAR *)malloc(buffer_width * buffer_height)) )
		return(0);

	// clear the buffer
	memset(binary_buffer, 0, buffer_width * buffer_height);

	// all good now calculate the width of the image
	//	interms of scanline interms of monochrome encoding.
	if (buffer_width % 8)
		scan_line = ( 8 + (buffer_width - (buffer_width % 8)) ) >> 3;
	else
		scan_line = ( (buffer_width - (buffer_width % 8)) ) >> 3;

	// the scan-line must be evenly divisible by four
	//	or it must be padded with 0s in the file, account for that
	if (scan_line % 4)
		scan_line += (4 - (scan_line % 4));

	// now we're set to rock, read and expand the byte packed
	//	image stored in pixels.
	for (i = 0, k = buffer_height - 1; i < buffer_height; i++, k--)
		for (j = 0; j < buffer_width; j++)
		{
			binary_buffer[j+i*buffer_width] = ( (pBitmap->pixels[(j>>3)+k*scan_line]) & (0x80 >> (j%8)) );

			// test for value
			if (binary_buffer[j+i*buffer_width])		// could be positives other than 1
				binary_buffer[j+i*buffer_width] = 1;	// make sure all of 'em is one
		} // end for j buffer_width

	// return success
	return(1);

} // end Bitmap_Monochrome
	
//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|