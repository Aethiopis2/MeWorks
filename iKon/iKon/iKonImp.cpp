//-----------------------------------------------------------------------------------------|
// iKon.h -- Defintion of class iKon used for converting bitmaps to icons.
//
// By: Code Red
//
// Date created:	1st of April 2018, Sunday (Fool's day).
// Last update:		
//-----------------------------------------------------------------------------------------|
#define WIN32_LEAN_AND_MEAN

//-----------------------------------------------------------------------------------------|
// DEFINES
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// INCLUDES
//-----------------------------------------------------------------------------------------|
#include <Windows.h>
#include <stdio.h>
#include <malloc.h>

#include "iKon.h"

//-----------------------------------------------------------------------------------------|
// TYPES
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// CLASS
//-----------------------------------------------------------------------------------------|

iKon::iKon(void)
{
// constructor intializes all members and pointers to NULL

	// make calls to local function
	Init_Members();

} // end constructor

//-----------------------------------------------------------------------------------------|

iKon::~iKon(void)
{
// let's all the memories and objects used by this object go

	// unload any bitmaps
	Unload_Bitmap_File();

	// delete any handles
	if (hbitmap)
		DeleteObject(hbitmap);

} // end destructor

//-----------------------------------------------------------------------------------------|

void iKon::Init_Members(void)
{
// intializes all members to NULL and clears mem's

	// win32's
	hbitmap = NULL;

	// test and clear bitmap structure pointers
	bitmap.rgb_palette	= NULL;
	bitmap.buffer		= NULL;
	 
	// clear sub-structures too
	ZeroMemory( &bitmap.bfh, sizeof(BITMAPFILEHEADER) );
	ZeroMemory( &bitmap.bih, sizeof(BITMAPINFOHEADER) );

	// also icons
	iconimage.icColors	= NULL;
	iconimage.icAND		= NULL;
	iconimage.icXOR		= NULL;

	ZeroMemory( &icondir, sizeof(ICONDIR) );
	ZeroMemory( &icondirentry, sizeof(ICONDIRENTRY) );

} // end Init_Members

//-----------------------------------------------------------------------------------------|

int iKon::Load_Bitmap_File(char *file_path)
{
// this opens up the bitmap given in the file_path and reads its contents
//	into memory.

	HANDLE hfile = NULL;		// win32 file handle
	DWORD bytes_read;

	// open file
	hfile = CreateFile(file_path, GENERIC_READ, 
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	// test
	if (!hfile)
		return(0);		// unable to open file

	// now read-in the bitmap file header
	ReadFile(hfile, &bitmap.bfh, sizeof(BITMAPFILEHEADER), &bytes_read, NULL);

	// test if this baby is a bitmap file
	if (bitmap.bfh.bfType != 19778)
	{
		CloseHandle(hfile);		// close file
		return(-1);		// No bitmap
	} // end if

	// next read-in the bitmap info header
	ReadFile(hfile, &bitmap.bih, sizeof(BITMAPINFOHEADER), &bytes_read, NULL);

	// test the bit-depth
	if (bitmap.bih.biBitCount <= 8)
	{
		// this requires a palette, test if the palette has
		//	been used before
		if (bitmap.rgb_palette)
		{
			free(bitmap.rgb_palette);
			bitmap.rgb_palette = NULL;		// double-tap
		} // end if

		// now allocate memory for palette
		bitmap.rgb_palette = (RGBQUAD *)malloc(bitmap.bih.biSizeImage);

		// test memory
		if (!bitmap.rgb_palette)
		{
			CloseHandle(hfile);
			return(0);	
		} // end if

		// read-it in
		ReadFile(hfile, bitmap.rgb_palette, bitmap.bih.biSizeImage, &bytes_read, NULL);

	} // end if there is palette

	// finally test if buffer is full
	if (bitmap.buffer)
	{
		free(bitmap.buffer);
		bitmap.buffer = NULL;
	} // end if buffer full

	// allocate memory for buffer
	if (!(bitmap.buffer = (UCHAR *)malloc(bitmap.bih.biSizeImage)))
	{
		CloseHandle(hfile);
		return(0);
	} // end if

	// read-in the color data
	// do a single blow
	ReadFile(hfile, bitmap.buffer, bitmap.bih.biSizeImage, &bytes_read, NULL);

	// image needs to be flipped, flip it.
	if (bitmap.bih.biHeight < 0)
	{
		// flip it
		Flip_Bitmap(bitmap.buffer, (bitmap.bih.biWidth * bitmap.bih.biBitCount) >> 3, 
			bitmap.bih.biHeight);
	} // end if

	// close file
	CloseHandle(hfile);

	// success
	return(1);

} // end Load_Bitmap_File

//-----------------------------------------------------------------------------------------|

int iKon::Unload_Bitmap_File(void)
{
// let's all the memory go used by the custom bitmap structure

	if (bitmap.buffer)
	{
		free(bitmap.buffer);
		bitmap.buffer = NULL;
	} // end if bitmap buffer

	// success
	return(1);

} // end Unlod_bitmap_file

//-----------------------------------------------------------------------------------------|

void iKon::Flip_Bitmap(UCHAR *buffer, int width, int height)
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

int iKon::Bitmap_To_Icon(char *file_path, HWND hwnd, HDC hdc)
{
// this converts a bitmap file into an icon.

	UCHAR *buffer = NULL;		// temp buffer

	// load-up the bitmap file
	if (Load_Bitmap_File(file_path) != 1)
		return(0);		// error

	// draw the loaded bitmap
	Draw_Bitmap(file_path, hwnd, hdc);

	// fill in the icon structures
	icondir.idReserved	= 0;		// alwayz null
	icondir.idCount		= 1;		// only one of a kind
	icondir.idType		= 1;		// off-course this is an icon

	icondirentry.bColorCount	= 0;
	icondirentry.bHeight		= bitmap.bih.biHeight;
	icondirentry.bWidth			= bitmap.bih.biWidth;
	icondirentry.wBitCount		= bitmap.bih.biBitCount;
	icondirentry.wPlanes		= bitmap.bih.biPlanes;
	icondirentry.dwBytesInRes	= sizeof(icondir) + sizeof(icondirentry) + bitmap.bih.biSizeImage;
	icondirentry.dwImageOffset	= sizeof(icondir) + sizeof(icondirentry);

	// allocate memory for the icon image
	if (!(iconimage.icAND = (BYTE *)malloc(bitmap.bih.biSizeImage)) )
		return(0);

	if (!(iconimage.icXOR = (BYTE *)malloc(bitmap.bih.biSizeImage)) )
		return(0);

	// copy info header
	memcpy(&iconimage.bih, &bitmap.bih, sizeof(bitmap.bih));
	memcpy(iconimage.icAND, bitmap.buffer, bitmap.bih.biSizeImage);
	memcpy(iconimage.icXOR, bitmap.buffer, bitmap.bih.biSizeImage);

	// now write this stuff into file
	HANDLE hfile = CreateFile("icon.ico", 
		GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hfile)
		return(0);

	DWORD bytes_written = 0;
	WriteFile(hfile, &icondir, sizeof(icondir), &bytes_written, NULL);
	WriteFile(hfile, &icondirentry, sizeof(icondirentry), &bytes_written, NULL);

	// write the pixels beginig with infoheader
	WriteFile(hfile, &iconimage.bih, sizeof(iconimage.bih), &bytes_written, NULL);
	WriteFile(hfile, iconimage.icXOR, bitmap.bih.biSizeImage, &bytes_written, NULL);
	WriteFile(hfile, iconimage.icAND, bitmap.bih.biSizeImage, &bytes_written, NULL);

	// all done close file
	CloseHandle(hfile);

	// success
	return(1);

} // end Bitmp_To_Icon

//-----------------------------------------------------------------------------------------|

int iKon::Draw_Bitmap(char *file_path, HWND hwnd, HDC hdc)
{
// this draws the bitmap pixels on the screen using blitter

	RECT rc;
	int x, y;					// starting position during drawing

	// test if bitmap contains pixels
	if (!bitmap.buffer)
		return(0);		// empty buffer

	// load bitmap using win32 calls
	hbitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), file_path, 
		IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);

	if (!hbitmap)
	{
		char text[256];
		sprintf(text, "LoadImage returned: %d.", GetLastError());
		MessageBox(hwnd, text, "ERROR BOX", MB_ICONERROR);
		return(0);
	} // end if

	// all set create a comaptible DC to draw stuff
	//	but first calculate the current client size
	GetClientRect(hwnd, &rc);
	FillRect(hdc, &rc, CreateSolidBrush(RGB(0,0,0)));

	HDC hdcmem = CreateCompatibleDC(hdc);

	// select bitmap into mem
	HBITMAP old = (HBITMAP)SelectObject(hdcmem, hbitmap);

	// now draw using win32 blitter
	// but first calculate the starting pos
	x = (rc.right >> 1) - (bitmap.bih.biWidth >> 1);
	y = (rc.bottom >> 1) - (bitmap.bih.biHeight >> 1);

	// blit
	BitBlt(hdc, x, y, bitmap.bih.biWidth, bitmap.bih.biHeight, hdcmem, 0, 0, SRCCOPY);

	// unslect object
	SelectObject(hdcmem, old);

	// delete the DC
	DeleteDC(hdcmem);

	// success
	return(1);

} // end draw_bitmap

//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|