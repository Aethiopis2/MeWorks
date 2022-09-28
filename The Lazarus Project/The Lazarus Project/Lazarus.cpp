//-----------------------------------------------------------------------------------------|
// PROJECT TITLE:		The Lazarus Project
//
// FILE NAME:			Lazarus.cpp
// EXE NAME:			Optical Geez
// 
// PROGRAM DESC:		The following file contains the defintion the prototypes described
//						by the Lazarus.h file. The Basic learning algorithim for this proj
//						is quite simple: Geez Fidel's are classified based on associated 
//						weights (A form of modified Neural Network -- dont know how exactly
//						yet?!). These weights are initalized based on a simple training algorithim
//						that is bound to give larger values for similarities; i.e. based on
//						the frequency of the distribution of points.
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
#include "Lazarus.h"

//-----------------------------------------------------------------------------------------|
// GLOBALS/EXTERNS
//-----------------------------------------------------------------------------------------|

int weight[NUM_PATTERNS][PATTERN_SPACE] = { 0 };	// pre-initalize to NULL
int matrix[PATTERN_SPACE]	= { 0 };				// the Matrix used for training 

//-----------------------------------------------------------------------------------------|
// PROTOTYPES
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// FUNCTIONS
//-----------------------------------------------------------------------------------------|

int Sum(int *input, const int p)
{
// this function returns the weighted sum of inputs against the weight table provided in
//	the global parameter. The third parameter specifies for which pattern to calculate
//	the sum for.
// NOTE: the function assumes the input parameter to be equal in size to PATTERN_SPACE

	int sum = 0;	// store's our weighted sum

	// loop and calc the sum for this input
	for (int i = 0; i < PATTERN_SPACE; i++)
		sum += (weight[p][i] * input[i]);

	// return the sum
	return(sum);

} // end Sum

//-----------------------------------------------------------------------------------------|

int Positive_Sum(int *input, const int p)
{
// this function returns the positive weighted sum of inputs for the pattern "p".

	int sum = 0;	// store's our some value

	// loop and calculate the positive values
	for (int i = 0; i < PATTERN_SPACE; i++)
		if (weight[p][i] > 0)
			sum += (input[i] + weight[p][i]);

	// return the sum
	return(sum);

} // end Positive_Sum(...)

//-----------------------------------------------------------------------------------------|

void Train_Weight(int *m, const int p)
{
// this function trains (adjust the weight value's of pattern "p" based on inputs
//	provided from a specially generated input matrix, "m".

	// loop thru and ajust all the weight's for this pattern
	for (int i = 0; i < PATTERN_SPACE; i++)
		weight[p][i] += m[i];	// simply add this value, increase the frequency count

} // end Train_weight

//-----------------------------------------------------------------------------------------|

void Train(int m[][PATTERN_SPACE])
{
// this function trains the weight's of each pattern by simpling adding the intput m to the
//	existing weight value of each pattern

	// loop and train each weight
	for (int i = 0; i < NUM_PATTERNS; i++)
	{
		// set the matrix used for training
		Set_Matrix(m[i]);
		Train_Weight(matrix, i);
	} // end for train

} // end for Train

//-----------------------------------------------------------------------------------------|

int Run_Classifer(int *input)
{
// This is where project OCR all comes down to:- the classifer. It basically return's the
//	pattern which has got the heighst ratio for matching statistics among all other patterns

	int p;									// pattern/looping var
	int sumt = 0;							// the sum of each pattern
	int sump = 0;							// the sum of positive weight's for each pattern
	double ratio[NUM_PATTERNS] = { 0.0 };	// the ratio of sumt by sump
	double max;								// the maximum ratio
	double threshold = 0.01;				// threshold indicating a good match
	int index;								// the index to choose

	// loop thru each pattern and compute the sum's
	//	and the ratio
	//Set_Matrix(input);
	for (p = 0; p < NUM_PATTERNS; p++)
	{
		// calculate the weighted sums
		sumt = Sum(input, p);
		sump = Positive_Sum(input, p);

		if (sump != 0)
		{
			// now calc the ratio of the two
			ratio[p] = ((double)sumt) / ((double)sump);
		}

	} // end for p

	// find out the maximum ratio
	max = ratio[0];		// assume this is max
	index = 0;
	for(p = 1; p < NUM_PATTERNS; p++)
	{
		if (max < ratio[p])
		{
			max = ratio[p];
			index = p;
		} // end if

	} // end for p

	// at this point return the index based on the threshold test
	if (max > threshold)
		return(index);
	else
		return(-1);			// still-unknown pattern

} // end Run_Classifer

//-----------------------------------------------------------------------------------------|

void Set_Matrix(int *buffer)
{
// this function set's the global pointer "matrix" as per the requirements
//	of the simple-training algorithim: i.e. set's NULL values to -1
// NOTE: function assumes the size of the buffer to be equal in size to the PATTERN_SPACE

	// CLEAR
	memset(matrix, 0, sizeof(int) * PATTERN_SPACE);

	// loop and set the matrix
	for (int i = 0; i < PATTERN_SPACE; i++)
	{
		if (buffer[i])	// if is set to 1
			matrix[i] = 1;
		else
			matrix[i] = -1;
	} // end for

} // end Set_Matrix

//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|