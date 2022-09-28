//-------------------------------------------------------------------------------------------|
// FILE NAME:			My_OCR.h
// PROGRAM AUTHOR:		Dr. Rediet Worku, aka Code Red
// DATE CREATED:		07th of September 2017, Thursday
// LAST UPDATE:			
// COMPLIED WITH:		Microsoft Visual Studio 2010 Ultimate
// COMPLIED ON:			HP ProBook Intel Core i5, running on Windows 7 (Computer name: Butu)
// PROGRAM DESCRIPION:	Contains decleration of routiens and globals used in OCR.
//-------------------------------------------------------------------------------------------|
#ifndef MYOCR_H
#define MYOCR_H

//-------------------------------------------------------------------------------------------|
// DEFINES
//-------------------------------------------------------------------------------------------|

//-------------------------------------------------------------------------------------------|
// TYPEDEFS
//-------------------------------------------------------------------------------------------|

// common typedefs
typedef unsigned char	UCHAR;

//-------------------------------------------------------------------------------------------|
// INCLUDES
//-------------------------------------------------------------------------------------------|

//-------------------------------------------------------------------------------------------|
// EXTERNS
//-------------------------------------------------------------------------------------------|

// gloabs used for the process of segmetation
extern int *bottom;			// these generally point to lines which contain texts i.e. the hight of lines of texts
extern int *top;

extern int **right;			// these specify the width of each FIdel in each line
extern int **left;

extern int **high;			// these are the true hight's of each fidel in every line
extern int **low;

extern int num_lines;		// this is the number of lines of text in the image file
extern int *num_chars;		// this is the number of texts (fidel's) in each line

extern int current_line;

//-------------------------------------------------------------------------------------------|
// PROTOTYPES
//-------------------------------------------------------------------------------------------|

// Initalizer
int Init_OCR(int width, int height);
void Shutdown_OCR(void);

// segmentation functions
int Line_Project(UCHAR *buffer, int width, int height);
int Extract_Fidel(UCHAR *buffer, int width, int height);
int Post_Segment(UCHAR *buffer, int width, int height);

// drawing functions
void Draw_Line_Project(UCHAR *buffer, int width, HDC hdc);
void Draw_Segment(UCHAR *buffer, int width, int x1, int y1, int x2, int y2, HDC hdc);
void Draw_All_Segments(UCHAR *buffer, int width, HDC hdc);
void Draw_All_Post_Segments(UCHAR *buffer, int width, HDC hdc);

#endif
//-------------------------------------------------------------------------------------------|
// END OF FILE
//-------------------------------------------------------------------------------------------|