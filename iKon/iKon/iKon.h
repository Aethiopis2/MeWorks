//-----------------------------------------------------------------------------------------|
// iKon.h -- Decleration of class iKon used for converting bitmaps to icons.
//
// By: Code Red
//
// Date created:	1st of April 2018, Sunday (Fool's day).
// Last update:		
//-----------------------------------------------------------------------------------------|
#ifndef IKON_H
#define IKON_H

//-----------------------------------------------------------------------------------------|
// DEFINES
//-----------------------------------------------------------------------------------------|
// this bulids a 32-bit color mode with a.r.g.b having 8.8.8.8 bits
#define _RGB32BIT(a,r,g,b) ( (b) + (g << 8) + (r << 16) + (a << 24) )

//-----------------------------------------------------------------------------------------|
// INCLUDES
//-----------------------------------------------------------------------------------------|
#include <Windows.h>

//-----------------------------------------------------------------------------------------|
// TYPES
//-----------------------------------------------------------------------------------------|
// this represents a bitmap file structure
typedef struct
{
	BITMAPFILEHEADER	bfh;
	BITMAPINFOHEADER	bih;
	RGBQUAD				*rgb_palette;
	UCHAR				*buffer;

} BITMAP_FILE,  *BITMAP_FILE_PTR;

// these represent an icon file structure
typedef struct
{
	WORD			idReserved;		// must be 0
	WORD			idType;			// 1 for icon 2 for Cursor's
	WORD			idCount;		// No images present in the Icon
} ICONDIR, *LPICONDIR;

typedef struct
{
	BYTE	bWidth;			// width in pixels
	BYTE	bHeight;		// hight in pixels
	BYTE	bColorCount;	// number of colors (0 if >= 8bpp)
	BYTE	bReserved;		// must be 0
	WORD	wPlanes;		// color planes
	WORD	wBitCount;		// bits per pixel
	DWORD	dwBytesInRes;	// Number of bytes in this resource
	DWORD	dwImageOffset;	// where in the file is this image
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct
{
	BITMAPINFOHEADER bih;	// DIB header
	RGBQUAD	*icColors;		// color table
	BYTE *icXOR;			// DIB bits for XOR mask
	BYTE *icAND;			// DIB bits for AND mask
} ICONIMAGE, *LPICONIMAGE;

//-----------------------------------------------------------------------------------------|
// CLASS
//-----------------------------------------------------------------------------------------|
class iKon
{
public:

	iKon();
	~iKon();

	void Init_Members(void);
	int Bitmap_To_Icon(char *file_path, HWND hwnd, HDC hdc);
	int Draw_Bitmap(char *file_path, HWND hwnd, HDC hdc);

private:

	// Win32's
	HBITMAP hbitmap;			// the currently loaded bitmap

	// compositions
	// bitmaps
	BITMAP_FILE bitmap;

	// icons
	ICONDIR icondir;
	ICONDIRENTRY icondirentry;
	ICONIMAGE iconimage;

	// utility functions
	int Load_Bitmap_File(char *file_path);
	int Unload_Bitmap_File(void);
	void Flip_Bitmap(UCHAR *buffer, int width, int height);

}; // end class

#endif
//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|