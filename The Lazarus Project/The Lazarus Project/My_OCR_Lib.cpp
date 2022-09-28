//-------------------------------------------------------------------------------------------|
// FILE NAME:			My_OCR.h
// PROGRAM AUTHOR:		Dr. Rediet Worku, aka Code Red
// DATE CREATED:		07th of September 2017, Thursday
// LAST UPDATE:			
// COMPLIED WITH:		Microsoft Visual Studio 2010 Ultimate
// COMPLIED ON:			HP ProBook Intel Core i5, running on Windows 7 (Computer name: Butu)
// PROGRAM DESCRIPION:	Contains defintitions of routiens and globals used in OCR.
//-------------------------------------------------------------------------------------------|
#define WIN32_LEAN_AND_MEAN

//-------------------------------------------------------------------------------------------|
// DEFINES
//-------------------------------------------------------------------------------------------|

//-------------------------------------------------------------------------------------------|
// TYPEDEFS
//-------------------------------------------------------------------------------------------|

//-------------------------------------------------------------------------------------------|
// INCLUDES
//-------------------------------------------------------------------------------------------|
#include <Windows.h>

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>

#include "My_OCR.h"

//-------------------------------------------------------------------------------------------|
// GLOBALS
//-------------------------------------------------------------------------------------------|

// these buffer's point to the stright line that spearate's lines of words
int *bottom			= NULL;	
int *top			= NULL;

// these point to the minimum recangular dimension that bound
//	each fidel's
int **right			= NULL;
int **left			= NULL;
int **high			= NULL;
int **low			= NULL;

int num_lines		= 0;		// the number of lines present in the doc
int *num_chars		= NULL;		// the number of each character in every line
int current_line	= 0;		

//-------------------------------------------------------------------------------------------|
// FUNCTIONS
//-------------------------------------------------------------------------------------------|

int Init_OCR(int width, int height)
{
// intializes the global vectors for OCR segmentation

	// test and clear top and bottom buffers
	if (top)
		free(top);
	top = NULL;		// double tap rule
	
	if (bottom)
		free(bottom);
	bottom = NULL;

	// clear counter's of lines and all
	num_lines = 0;

	// test and clear char's
	if (num_chars)
		free(num_chars);
	num_chars = NULL;

	// allocate memory for the buffers top, bottom
	if ( !(top = (int *)malloc(sizeof(int) * height)) )
		return(0);

	if ( !(bottom = (int *)malloc(sizeof(int) * height)) )
		return(0);

	// clear the buffers
	memset(top, 0, sizeof(int) * height);
	memset(bottom, 0, sizeof(int) * height);

	// success
	return(1);

} // end Init

//-------------------------------------------------------------------------------------------|

void Shutdown_OCR(void)
{
// this function releases all the memories used by the OCR library

	// free the line indicator vector's

	// test if top vector not empty
	if (top)
	{
		free(top);
		top = NULL;	// double-tap
	} // end if

	// test for bottom
	if (bottom)
	{
		free(bottom);
		bottom = NULL;
	} // end if bottom

	// free character width vector's

	// free left
	if (left)
	{
		free(left);
		left = NULL;
	} // end if left

	// free right
	if (right)
	{
		free(right);
		right = NULL;
	} // end if right

	// free the high and low vectors of characters

	// free high
	if (high)
	{
		free(high);
		high = NULL;
	} // end if free high

	// test and free low
	if (low)
	{
		free(low);
		low = NULL;
	} // end if low

	// test and free the num_chars vector
	if (num_chars)
	{
		free(num_chars);
		num_chars = NULL;
	} // end if num_chars

} // end Shutdown_OCR

//-------------------------------------------------------------------------------------------|

int Line_Project(UCHAR *buffer, int width, int height)
{
// detects the presence of characters by horizontal projection

	int *counter = NULL;	// count's presnece of black pixels

	num_lines = 0;		// re-set to 0

	if ( !(counter = (int *)malloc(sizeof(int) * height)) )
		return(0);

	// clear mem
	memset(counter, 0, sizeof(int) * height);

	// go thru each pixel and count the number of black
	//	pixels in each line.
	for (int i = 0; i < height; i++)
	{
		// loop horiz
		for (int j = 0; j < width; j++)
		{
			if (!buffer[j+width*i])
				++counter[i];			// there is a black in this region
		} // end if
	} // end for

	// now loop thru every line in each pixel and determine
	//	those that have black pixels and set the boundaries
	//	top and bottom of each line.
	int i = 0;	// sart from the first line
	while (i < height)
	{
		// test for black pixels
		if (counter[i])
		{
			top[num_lines] = i;

			// now loop and find whity
			while (counter[++i] && i < height);

			bottom[num_lines++] = i - 1;
		} // end if blackie

		++i;
	} // end while

	free(counter);

	// success
	return(1);

} // end Get_Line

//-------------------------------------------------------------------------------------------|

int Extract_Fidel(UCHAR *buffer, int width, int height)
{
// looking at the lines that contain fidel's the algorithim extract's
//	the fidel glyphs using an orthogonal concept of Line_Project.

	// reject false calls
	if (!top || !bottom)
		return(0);

	int **counter = NULL;		// store's the count of black pixels in each
		// vertical line of every horizontal line

	// test for pre alloced mem
	if (num_chars)
		free(num_chars);

	if (right)
		free(right);

	if (left)
		free(left);

	// every line contains a variable number of characters
	//	use an array to account for such circumstances.
	if ( !(num_chars = (int *)malloc(sizeof(int) * num_lines)) )
		return(0);

	// clear buffer
	memset(num_chars, 0, sizeof(int) * num_lines);

	// as a consiquence every line needs and individual array of
	//	left and right buffers to pin point the exact boundaries 
	//	of each fidel. Impelment this function as double array.
	if ( !(left = (int **)malloc(sizeof(int) * num_lines)) )
		return(0);

	if ( !(right = (int **)malloc(sizeof(int) * num_lines)) )
		return(0);

	// alloc mem for counter
	if ( !(counter = (int **)malloc(sizeof(int) * num_lines)) )
		return(0);

	// now allocate memory for each line, which must be at most
	//	"width" long.
	for (int i = 0; i < num_lines; i++)
	{
		left[i] = (int *)malloc(sizeof(int) * width);
		right[i] = (int *)malloc(sizeof(int) * width);
		counter[i] = (int *)malloc(sizeof(int) * width);

		// test
		if (!left[i] || !right[i] || !counter[i])
			return(0);

		// clear the buffers
		memset(left[i], 0, sizeof(int) * width);
		memset(right[i], 0, sizeof(int) * width);
		memset(counter[i], 0, sizeof(int) * width);

	} // end for alloc

	// go thru each line and count the number of black pixels
	//	that exist in each column of line.
	for (int line = 0; line < num_lines; line++)
	{
		// go thru the width of this line
		for (int col = 0; col < width; col++)
		{
			// finally thru rows and test vertical lines
			for (int row = top[line]; row <= bottom[line]; row++)
				if (!buffer[col+row*width])
					++counter[line][col];

		} // end for col

	} // end for line

	// finally, go thru line by line and test
	//	each column that got a black pixel on the 
	//	vertical axis
	for (int line = 0; line < num_lines; line++)
	{
		// now do each column
		int col = 0;
		while (col < width)
		{
			// test if there is/are vertical pixels on this
			//	line
			if (counter[line][col])
			{
				// save this point as left
				left[line][num_chars[line]] = col;

				// find the first whity 44
				while (counter[line][++col])
				{
					// make sure it don't skip-off boundaries
					if (col > width)
					{
						col = width;
						break;
					} // end if
				} // end while

				// now is the right part 
				right[line][num_chars[line]++] = col - 1;

			} // end if counter
			++col;
		} // end while col

	} // end for line

	// release memories
	for (int i = 0; i < num_lines; i++)
		free(counter[i]);
	free(counter);

	// success
	return(1);

} // end Line_ProjectV

//-------------------------------------------------------------------------------------------|

int Post_Segment(UCHAR *buffer, int width, int height)
{
// adjusts the extra spaces that exist above and below each character.

	// trivially reject false calls
	if (!top || !bottom || !right || !left)
		return(0);

	//int i;						// looping var
	int ***counter = NULL;		// counts black pixels present in a row

	// test and free the high and lo buffers
	if (high)
	{
		// free columns
		//for (i = 0; i < num_lines; i++)
		//	free(high[i]);
		free(high);
	} // end if high

	if (low)
	{
		//for (i = 0; i < num_lines; i++)
			//free(low[i]);
		free(low);
	} // end if low

	// allocate memory for the counter, and high and low
	//	pointers. each is expected to be "num_lines" long since
	//	there are that much lines in the image document
	if ( !(counter = (int ***)malloc(sizeof(int) * num_lines)) )
		return(0);

	if ( !(high = (int **)malloc(sizeof(int) * num_lines)) )
		return(0);

	if ( !(low = (int **)malloc(sizeof(int) * num_lines)) )
		return(0);

	// now loop thru each line and allocate memories for each
	//	character in the line but the counter must store, the
	//	sum of black pixels in each row of every character
	for (int line = 0; line < num_lines; line++)
	{
		// alloc mem for counter
		counter[line] = (int **)malloc(sizeof(int) * num_chars[line]);

		// alloc mem for high and low vectors based on the
		//	the number of fidel's in each line
		high[line] = (int *)malloc(sizeof(int) * num_chars[line]);
		low[line] = (int *)malloc(sizeof(int) * num_chars[line]);

		// now test each memory
		if (!counter[line] || !high[line] || !low[line])
			return(0);


		// the counter contains a third dimension allocate memory for
		//	that, this must be at most equal to line height of every line
		for (int fidel = 0; fidel < num_chars[line]; fidel++)
		{
			counter[line][fidel] = (int *)malloc(sizeof(int) * (1 + (bottom[line] - top[line])));

			// test
			if (!counter[line][fidel])
				return(0);

			// now clear the counter
			for (int r = 0; r < (1 + (bottom[line] - top[line])); r++)
				counter[line][fidel][r] = 0;
		} // end for

	} // end for line alloc

	// now loop thru each line containing a supposed text, and..
	for (int line = 0; line < num_lines; line++)
	{
		// ... loop thru each supposed fidel, and...
		for (int fidel = 0; fidel < num_chars[line]; fidel++)
		{

			// ... begining with left most corner of fidel's line 
			//	or row 0, and begining with the first column of 
			//	fidel's matrix, count the number of blackish pixels present
			//	in each row
			int k = 0;
			for (int r = top[line]; r <= bottom[line]; r++)
			{
				for (int c = left[line][fidel]; c <= right[line][fidel]; c++)
				{
					if (!buffer[c+r*width])	// is black
					{
						++counter[line][fidel][k];
					}
				} // end for c
				++k;
			} // end for r

		} // end for fidel

	} // end for line

	// now for the finsishing touches; loop line by line once more
	//	and fidel by fidel, then row by row and adjust the height
	for (int line = 0; line < num_lines; line++)
	{
		// now fidel by fidel
		for (int fidel = 0; fidel < num_chars[line]; fidel++)
		{
			// finally go row by row
			int r = 0;
			int h = bottom[line] - top[line];	// the height

			while (r < h)
			{
				// test if blackish
				if (counter[line][fidel][r])
				{
					// set the high point
					high[line][fidel] = r+top[line];
					//r = 1 + (bottom[line] - top[line]);

					// find white point
					while (counter[line][fidel][r++])
					{
						// make sure r stays in bound
						if (r > h)
						{
							r = 1 + h;
							break;
						} // end if
					} // end while still whity 44

					low[line][fidel] = (r-1)+top[line];
					//r = 1 + h;
					break;
				} // end if

				++r;	// move to next row
			} // end while row


		} // end for fidel

	} // end for line

	// free the counter
	//for (i = 0; i <= num_lines; i++)
	//{
		//for (int j = 0; j < num_chars[i]; j++)
			//free(counter[i][j]);
		//free(counter[i]);
	//}
	free(counter);

	// success
	return(1);

} // end Post_Segment


//-------------------------------------------------------------------------------------------|

void Draw_Line_Project(UCHAR *buffer, int width, HDC hdc)
{
// draw's the strips of line text's

	for (int i = 0; i < num_lines; i++)
	{
		for (int j = top[i]; j <= bottom[i]; j++)
		{
			for (int k = 0; k < width; k++)
			{
				if (buffer[k+j*width])
					SetPixel(hdc, k, j, RGB(255,255,255));
				else
					SetPixel(hdc, k, j, 0);
			} // end for
		} // end for j
	} // end for i

} // end draw_line roject

//-------------------------------------------------------------------------------------------|

void Draw_Segment(UCHAR *buffer, int width, int x1, int y1, int x2, int y2, HDC hdc)
{
// function draw's the regions of buffer between points (x1,y1) and (x2,y2)
//	using calls to SetPixel(...) Win32 API.

	// now draw the region's between these bands
	for (int i = y1; i <= y2; i++)
		for (int j = x1; j <= x2; j++)
			if (buffer[j+i*width])
				SetPixel(hdc, j, i, RGB(255,255,255));
			else
				SetPixel(hdc, j, i, 0);

} // end function draw segment


//-------------------------------------------------------------------------------------------|

void Draw_All_Segments(UCHAR *buffer, int width, HDC hdc)
{
// Draw's all the character segments found within bitmap

	// make sure buffer's are set
	if (!top || !bottom || !right || !left)
		return;

	// start fro the very first line and...
	for (int line = 0; line < num_lines; line++)
	{
		// ...go thru every character in the line, and...
		for (int chr = 0; chr < num_chars[line]; chr++)
		{

			// draw the recangular bounded region of segment
			//	using SetPixel(...) API.
			for (int r = top[line]; r <= bottom[line]; r++)
				for (int c = left[line][chr]; c <= right[line][chr]; c++)
				{
					if (buffer[c+r*width])
						SetPixel(hdc, c, r, RGB(255,255,255));
					else
						SetPixel(hdc, c, r, 0);
				} // end for c

		} // end for chr

	} // end for line

} // end Draw_All_Segments

//-------------------------------------------------------------------------------------------|

void Draw_All_Post_Segments(UCHAR *buffer, int width, HDC hdc)
{
// Draw's all the character segments found within bitmap

	// make sure buffer's are set
	if (!high || !low || !right || !left)
		return;

	// start fro the very first line and...
	for (int line = 0; line < num_lines; line++)
	{
		// ...go thru every character in the line, and...
		for (int chr = 0; chr < num_chars[line]; chr++)
		{

			// draw the recangular bounded region of segment
			//	using SetPixel(...) API.
			for (int r = high[line][chr]; r <= low[line][chr]; r++)
				for (int c = left[line][chr]; c <= right[line][chr]; c++)
				{
					if (buffer[c+r*width])
						SetPixel(hdc, c, r, RGB(255,255,255));
					else
						SetPixel(hdc, c, r, 0);
				} // end for c

		} // end for chr

	} // end for line

} // end Draw_All_Segments

//-------------------------------------------------------------------------------------------|
// END OF FILE
//-------------------------------------------------------------------------------------------|