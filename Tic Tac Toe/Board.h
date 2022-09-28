//--------------------------------------------------|
// Board.h -- Decleration of Object, Board, that is |
//	used to play the game of tic tac toe. The object|
//	contains two attributes, a 3x3 array grid, and  |
//	an integer member, moves_played, which tracks	|
//	the number of turns played in the game. While as|
//	grid reprsents the actual 3x3 matrix world of	|
//	the game.										|
//													|
// By: Code Red										|
//													|
//--------------------------------------------------|
#ifndef BOARD_H
#define BOARD_H

#include <windows.h>

#include <iostream>
using namespace std;

#include "Move.h"	// move class header

class Board
{
public:

	Board();
	void Initialize();
	void Play(Move mark_this);
	void Draw_Config(HDC hdc);
	void operator=(Board &template_grid);
	void print();

	int Legal_Moves(Move *possible_moves);
	int Evaluate();
	int Worst_Case();
	int Get_Moves_Played();
	int Check_Winner();

	bool Done();
	bool Better(int payoff, int value);

private:

	int grid[3][3];			// actual tic tac toe grid
	int moves_played;		// track's turns and number of games played

	// utlities
	void Draw_Grid(HDC hdc);	
	void Draw_Markers(HDC hdc);	

	// Windows OS GDI Stuff
	HPEN grid_pen;
	HPEN marker;
	HBRUSH white_brush;

};

#endif