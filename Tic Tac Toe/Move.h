//--------------------------------------------------|
// Move.h -- Decleration of Object, Move, which is	|
//	used to make moves (mark the grid).				|
//													|
// NOTE: Essentialy this class is the same as the	|
//	POINT structure of Windows types. Using POINT	|
//	to mark moves also works.						|
//													|
// By: Code Red										|
//													|
//--------------------------------------------------|
#ifndef MOVE
#define MOVE

class Move
{
public: 

	Move();
	Move(int y, int x);

	void operator=(Move &template_move);

	int row;
	int col;	// represent a single location in a 2D world, or
		// game of tic tac toe.
};	// end move

#endif