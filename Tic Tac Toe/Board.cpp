//--------------------------------------------------|
// Board.cpp -- Decleration of Object, Board, that	|
//	used to play the game of tic tac toe. The object|
//	contains two attributes, a 3x3 array, grid, and |
//	an integer member, moves_played, which tracks	|
//	the number of turns played in the game. While as|
//	grid reprsents the actual 3x3 matrix world of	|
//	the game.										|
//													|
// By: Code Red										|
//													|
//--------------------------------------------------|
#define WIN32_LEAN_AND_MEAN

//--------------------------------------------------|
// INCLUDES
//--------------------------------------------------|
#include <Windows.h>
#include <windowsx.h>

#include <iostream>
using namespace std;

#include <stdlib.h>

#include "Board.h"
#include "Move.h"

//--------------------------------------------------|
// FUNCTIONS
//--------------------------------------------------|
Board::Board()
{
// simply calls on function Initialize, and creates
//	some certain GDI objects used for drawing

	Initialize();

} // end constructor

//--------------------------------------------------|

void Board::Initialize()
{
// simply set attribute's to 0

	// loop and set the grid array to 0
	for (int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			grid[i][j] = 0;

	// clear moves_played
	moves_played = 0;

} // end Initialize

//--------------------------------------------------|

void Board::Play(Move mark_this)
{
// The function marks (play's) the grid object on the
//	position sent by the parameter and increases the
//	play count.

	// perform error checking first
	// by some act of fate if mark_this has not been
	// marked before
	if (grid[mark_this.row][mark_this.col] == 0)
	{ // then you "mark this"

		// mark the grid / play the game
		grid[mark_this.row][mark_this.col] = 1 + (moves_played % 2);

		++moves_played;	// increase play count
	} // end if empty

} // end Play

//--------------------------------------------------|

void Board::Draw_Config(HDC hdc)
{
// Draw's the board's current configuration on the current window
//	by calling two member fnctions, Draw_Grid() and Draw_Marker().

	
	// draw some text information
	SetTextColor(hdc, RGB(255,0,0));
	TextOut(hdc, 0, 400 - 16, "Tic Tac Toe: Classic", strlen("Tic Tac Toe: Classic"));

	SetTextColor(hdc, RGB(100, 155, 200));
	TextOut(hdc, 0, 0,
		"Hit <Esc> -- exit game    <Space> -- change game modes",
		strlen("Hit <Esc> -- exit game    <Space> -- change game modes"));

	Draw_Grid(hdc);
	Draw_Markers(hdc);

} // end Draw_Config

//--------------------------------------------------|

void Board::Draw_Grid(HDC hdc)
{
// draw's the tic tac toe grid based on a priori
//	information.

	int i;	// loop var

	// these record the co-ordinates to draw from and to
	POINT heads[4] = {150,50, 250,50, 50,150, 50,250};		// from...
	POINT tails[4] = {150,350, 250,350, 350,150, 350,250};	// ...to

	for (i = 0; i < 4; i++)
	{
		MoveToEx(hdc, heads[i].x, heads[i].y, NULL);
		LineTo(hdc, tails[i].x, tails[i].y);
	} // end for

} // end Draw_Gird

//--------------------------------------------------|

void Board::Draw_Markers(HDC hdc)
{
// draw's the x's and o's of tic tac toe, based on
// a priori information.

	int i, j;	// looping var

	// loop and draw the x's and o's
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			// mapping array notation to window pixel points
			int i1 = i * 100 + 50 + 4;
			int i2 = i * 100 + 150 - 4;
			int j1 = j * 100 + 50 + 4;
			int j2 = j * 100 + 150 - 4;

			// draw X if player 1
			if (grid[i][j] == 1)
			{
				// first diagonal
				MoveToEx(hdc, i1, j1, NULL);
				LineTo(hdc, i2, j2);

				// next diagonal
				MoveToEx(hdc, i2, j1, NULL);
				LineTo(hdc, i1, j2);
			} // end if player 1
			else
			{
				if (grid[i][j] == 2) // player 2
				{
					// Draw ellipse
					Ellipse(hdc, i1, j1, i2, j2);
				} // end if
			} // end else

		} // end for j
	} // end for i

} // end Draw_Markers

//--------------------------------------------------|

void Board::operator =(Board &template_grid)
{
// Overloaded assignment operator, copies the contents
//	of the object in the parameter to this object.

	// avoid self assignment
	if (this == &template_grid)
		return;

	// loop and copy gird to this->gird.
	int i,j;	// looping var
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			this->grid[i][j] = template_grid.grid[i][j];

	// copy moves played
	this->moves_played = template_grid.moves_played;

} // end overloaded assignment operator

//--------------------------------------------------|

int Board::Legal_Moves(Move *possible_moves)
{
// Count's and initalizes the legal moves (moves not
//	yet played) into the Move pointer passed as the
//	parameter and return's the count.

	int count = 0;	// tracks spaces, and used as index to
		// the pointer

	int i,j;	// looping var

	// loop and intialize the possible moves
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (grid[i][j] == 0)	// empty
			{
				possible_moves[count].row = i;
				possible_moves[count].col = j;

				++count;	// update count
			} // end if

		} // end for j
	} // end for i

	return(count);	// return the count

} // end Legal_Moves

//--------------------------------------------------|

int Board::Evaluate()
{
// Evalute's the current game's configuration, and
//	return's a payoff for this game.

	int winner = Check_Winner();

	if (winner == 1)	// player 1 won this game
		return(10 - moves_played);
	else
		if (winner == 2)	// must be player 2
			return(moves_played - 10);
		else
			return(0);		// draw

} // end evalute

//--------------------------------------------------|

int Board::Get_Moves_Played()
{
// return's data member moves_played

	return(moves_played);

} // end Get_Moves

//--------------------------------------------------|

int Board::Check_Winner()
{
// return's a 1 if player 1 won the game or a 2 for
//	player 2. 

	int i;	

	// check for horizontal wins
	for (i = 0; i < 3; i++)
	{
		if (grid[i][0] && grid[i][0] == grid[i][1] && 
			grid[i][0] == grid[i][2])
			return(grid[i][0]);	// a winner
	} // end for

	// now for vertical
	for (i = 0; i < 3; i++)
	{
		if (grid[0][i] && grid[0][i] == grid[1][i] &&
			grid[0][i] == grid[2][i])
			return(grid[0][i]);	// a winner
	} // end for

	// finally for the two diagonal's
	if (grid[0][0] && grid[0][0] == grid[1][1] &&
		grid[0][0] == grid[2][2])
		return(grid[0][0]);

	// diagonal two
	if (grid[0][2] && grid[0][2] == grid[1][1] &&
		grid[0][2] == grid[2][0])
		return(grid[0][2]);

	// game must be a draw
	return(0);

} // end Check_Winner

//--------------------------------------------------|

bool Board::Done()
{
// return's a true if game is over 

	return(Check_Winner() > 0 || moves_played >= 9);

} // end done

//--------------------------------------------------|

int Board::Worst_Case()
{
// return's the worst possible payoff for this game.
//	i.e. simply return's a +10 to mean worst for player 2
//	or -10 for player 1.

	// who's turn
	if ((1 + moves_played) % 2 == 0)	// player 2
		return(10);
	else						// player 1
		return(-10);

} // end Worst_Case

//--------------------------------------------------|

bool Board::Better(int payoff, int value)
{
// based on game turn return's a true if indeed
//	the parameter payoff is better of for player than
//	parameter value. Logic is player 1 is the maximaizer
//	and player 2 the minimizer.

	if ((1 + moves_played) % 2 == 0)		// player 2's turn
		return(payoff < value);
	else							// player 1's turn
		return(payoff > value);

} // end Better

//--------------------------------------------------|

void Board::print()
{
// stub tester

	int i,j;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (grid[i][j] == 1)
			{
				if (j < 2)
					cout << "X |";
				else
					cout << "X  ";
			}
			else
			{
				if (grid[i][j] == 2)
				{
					if (j < 2)
						cout << "O |";
					else
						cout << "O  ";
				}
				else
				{
					if (j < 2)
						cout << "  |";
					else
						cout << "   ";
				}
			} // end else
		} // end for j

		cout << endl;

		if (i < 2)
		{
			for (int k = 0; k < 3; k++)
				cout << "---";
			cout << endl;
		} // end i == 0
	} // end for j

} // end print