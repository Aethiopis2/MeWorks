//-----------------------------------------------------------------------------------------|
// PROJECT TITLE:		The Lazarus Project
//
// FILE NAME:			Teaser.cpp
// EXE NAME:			Teaser.exe
// 
// PROGRAM DESC:		The following file contains the defintion of our entry point and
//						accessory funtions. The Program is designed for test purposes only.
//
// DATE CREATED:		5th of Oct 2017, Thursday (aka the hand of fate)
// LAST UPDATE:
//
// PROGRAM AUTHOR:		Dr. Rediet Worku aka Code Red
//-----------------------------------------------------------------------------------------|
#define WIN32_LEAN_AND_MEAN

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
#include <CommDlg.h>

#include <stdio.h>
#include <conio.h>
#include <memory.h>
#include <wchar.h>

#include "Lazarus.h"
#include "Geez_Unicode.h"
#include "Image.h"
#include "My_OCR.h"

//-----------------------------------------------------------------------------------------|
// GLOBALS/EXTERNS
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// PROTOTYPES
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// FUNCTIONS
//-----------------------------------------------------------------------------------------|
// this is the main entry point of our test application -- the Teaser.exe
int main()
{
	wchar_t test_text[2] = { 0 };		// used for printing UNICODE glyphys in Windows OS
	char *filenames[PATTERN_GROUP][NUM_PATTERNS] = { "train1.bmp" };

	int training_data[PATTERN_GROUP][NUM_PATTERNS][PATTERN_SPACE];
		// many per each pattern to learn form

	int test_data[PATTERN_SPACE];	// the test data

	// print heading's 
	printf("\n...and Jesus called out in a loud voice: \"Lazarus, come out!\"\n\n");
	
	printf("Loading the file \"train1.bmp\"\n");
	Load_Bitmap_File(&bitmap, "train1.bmp");				// load the image data

	// Assuming monochrome: intialize the extern buffer from image lib
	Bitmap_Monochrome(&bitmap);

	printf("File loaded.\nUloading the bitmap.\nInitalizing the OCR engine!\n");
	Unload_Bitmap(&bitmap);

	// initalze ocr
	Init_OCR(buffer_width, buffer_height);
	printf("%d -- %d\n\n", buffer_width, buffer_height);

	// apply segmentations
	printf("Now Appling Line Segment...\n");
	Line_Project(buffer, buffer_width, buffer_height);
	printf("%d\n\n", num_lines);

	// apply word seg
	printf("Now Applying word segmentation...\n");
	Extract_Fidel(buffer, buffer_width, buffer_height);

	// trim extra spaces
	printf("Now applying post segmentation to remove extra-spces...\n");
	Post_Segment(buffer, buffer_width, buffer_height);

	printf("%d -- %d\n\n", num_lines, num_chars[0]);
	// at this point we know the locations of our fidels in the bitmap
	//	loop thru each one and train the classifer...

	// begin training
	printf("Now begining training our classifer...\n");

	int p = 0;

	// loop thru all the lines in the bitmap
	for (int line = 0; line < num_lines; line++)
	{
		// go thru every character in each line
		for (int ch = 0; ch < num_chars[num_lines]; ch++)
		{
			Recon_Input(buffer, left[line][ch], high[line][ch], right[line][ch], low[line][ch]);
			
			// copy to the training data
			memcpy(training_data[0][p], matrix, PATTERN_SPACE);
			//Train_Weight(matrix, p);
			++p;
			printf("%d\n", p);
		} // end for c

	} // end for r

	// training complete
	printf("Training complete...\nNow you can test me sir!\nHit a 0 if you don't want to.\n");

	// now process app till i wanna quit
	int response;
	OPENFILENAME ofn;
	char filename[MAX_PATH] = { 0 };
	while ( (response = _getch()) != 48)	// a zero
	{
		// fill in the openfile strcture to open bitmaps
		ZeroMemory(&ofn, sizeof(ofn));

		ofn.lStructSize			= sizeof(ofn);
		ofn.hwndOwner			= NULL;	
		ofn.hInstance			= GetModuleHandle(NULL);
		ofn.lpstrFilter			= "Bitmaps\0*.bmp\0";
		ofn.lpstrFile			= filename;
		ofn.nMaxFile			= MAX_PATH;
		ofn.nFilterIndex		= 1;
		ofn.Flags				= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn))
		{
			Load_Bitmap_File(&bitmap, filename);
			Bitmap_Monochrome(&bitmap);
			//Set_Matrix(buffer);
			//memcpy(test_data, matrix, PATTERN_SPACE);
			
			printf("Now running the classifer...\n");
			int code = Run_Classifer(test_data);
			printf("Code value: %d\n", code);
			test_text[0] = amharic_unicodes[code];
			MessageBoxW(0, test_text, L"", 0);
		} // end if

		printf("Please hit 0 if you wanna stop...\n");

	} // end while

	// unload bitmaps if there are any
	Unload_Bitmap(&bitmap);

	// print exit message
	printf("\nPress any key to exit...\n");

	// back to the OS
	return(_getch());		// fake return code that we'll never branch off to

} // end main

//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|