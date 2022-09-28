//-----------------------------------------------------------------------------------------|
// PROJECT TITLE:		The Lazarus Project
//
// FILE NAME:			Lasarus.h
// EXE NAME:			Optical Geez
// 
// PROGRAM DESC:		The following file contains the declearation of sets of prototypes 
//						that are used in the Machine Learning for OCR of Geez scripts.
//
// DATE CREATED:		5th of Oct 2017, Thursday (aka the hand of fate)
// LAST UPDATE:
//
// PROGRAM AUTHOR:		Dr. Rediet Worku aka Code Red
//-----------------------------------------------------------------------------------------|
#ifndef LAZARUS_H
#define LAZARUS_H

//-----------------------------------------------------------------------------------------|
// DEFINES
//-----------------------------------------------------------------------------------------|

#define NUM_PATTERNS		231		// the number of patterns to be classified - The Amharic/Geez Fidels
#define PATTERN_GROUP		1		// the number of groups per each pattern used for trianinig

// dummines used for estimating dimension size (i.e. the value in the pattern space)
#define WIDTH				30
#define HEIGHT				30

#define PATTERN_SPACE		WIDTH * HEIGHT		// the dimensions of the image i.e. the value width x height

//-----------------------------------------------------------------------------------------|
// TYPEDEFS
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// INCLUDES
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// GLOBALS/EXTERNS
//-----------------------------------------------------------------------------------------|

extern int weight[NUM_PATTERNS][PATTERN_SPACE];		// the weight table: 1 per each pattern classified
extern int matrix[PATTERN_SPACE];

//-----------------------------------------------------------------------------------------|
// PROTOTYPES
//-----------------------------------------------------------------------------------------|

int Init_Weight(void);
int Sum(int *input, const int p);
int Positive_Sum(int *input, const int p);
void Train_Weight(int *m, const int p);
void Train(int m[][PATTERN_SPACE]);
int Run_Classifer(int *input);
void Set_Matrix(int *buffer);

//-----------------------------------------------------------------------------------------|
// FUNCTIONS
//-----------------------------------------------------------------------------------------|

#endif
//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|