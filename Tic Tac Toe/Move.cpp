//--------------------------------------------------|
// Move.h -- Definition of Object, Move, which is	|
//	used to make moves (mark the grid). The object  |
//	contains two attributes which mark a position in|
//	Board's grid.									|
//													|
// NOTE: Essentialy this class is the same as the	|
//	POINT structure of Windows types. Using POINT	|
//	to mark moves also works.						|
//													|
// By: Code Red										|
//													|
//--------------------------------------------------|
#define WIN32_LEAN_AND_MEAN

#include "Move.h"

Move::Move()
{
// Constructor simply sets the attributes to 0.

	row = col = 0;

} // end constructor

//--------------------------------------------------|

Move::Move(int y, int x)
{
// this default constructor copies the parameters
//	into it's member data's.

	row = y;
	col = x;
} // end default constructor

//--------------------------------------------------|

void Move::operator=(Move &template_move)
{
// This overloaded assignment operator copies the 
//	attributes of the object in the parameter to this
//	object.

	// avoid self assignment
	if (this != &template_move)
	{
		this->row = template_move.row;
		this->col = template_move.col;
	} // end if

} // end assignmet overloading

//--------------------------------------------------|