//---------------------------------------------------
// Stub.cpp -- test app
//	
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>

#include "Move.h"
#include "Board.h"

int Look_Ahead(Board game, Move &play_it, int depth)
{
// looks ahead and return's the best possible move for the
// next game.

	if (game.Done() || depth == 0)
		return(game.Evaluate());  // rate this game
	else
	{
	// not a goal state, hence generate a list of moves
	// and play each to their very depth.
			
		int i;
		
		Move *possible_moves = new Move [depth];	// remainig moves always equals depth
		game.Legal_Moves(possible_moves);		// initalize these moves
		
		int worst_pay, payoff = game.Worst_Case();	// assume this is current game's estimate

		// loop and compare all the moves and select the best one
		for (i = 0; i < depth; i++)
		{
			Move dummy;
			Board new_game = game;				// copy this game
			new_game.Play(possible_moves[i]);	// play the new game and
			worst_pay = Look_Ahead(new_game,	// look ahead with the next possible move
				dummy,	 						// played and get an estimate for this move
				depth - 1);
			if (game.Better(worst_pay, payoff))	// if indeed worst_pay, is worth it for this move
			{
				payoff = worst_pay;				// then copy this move and return it
				play_it = possible_moves[i];
			} // end if
		} // end for
		
		return(payoff);

	} // end else 	
} // end Look_Ahead		

int main()
{
	Board tic;

	while (!tic.Done())
	{
		tic.print();

		int input;
		cin >> input;

		int x = input / 10;
		int y = input % 10;
		Move mine(x, y);
		tic.Play(mine);
		tic.print();

		Move ai;
		Look_Ahead(tic, ai, 9);
		cout << endl << ai.col << "  " << ai.row 
			<< endl;

		tic.Play(ai);
		
	}

	return(0);
}
		