//--------------------------------------------------|
// TicTacToe.cpp -- "My Old Nemisis" perhaps it is	|
//	time to reveal your destiny. This Program palys	|
//	the game of Tic Tac Toe in two modes. VS HUMAN	|
//	VS AI/MACHINE.									|
//													|
// By: Code Red										|
//													|
//--------------------------------------------------|
#define WIN32_LEAN_AND_MEAN

//--------------------------------------------------|
// INCLUDES											
//--------------------------------------------------|
#include <Windows.h>	// Windows headers
#include <Windowsx.h>
#include <MMsystem.h>

#include <stdio.h>		// standard C/C++ headers
#include <stdlib.h>
#include <malloc.h>

#include "Resource.h"
#include "Board.h"
#include "Move.h"

//--------------------------------------------------|
// DEFINES											
//--------------------------------------------------|

#define WINDOW_CLASS_NAME	"T3GAME"
#define WINDOW_TITLE		"TIC TAC TOE"
#define WINDOW_WIDTH		400
#define WINDOW_HEIGHT		400

// these read the keyboard asyncronously
#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

//--------------------------------------------------|
// PROTOTYPES
//--------------------------------------------------|
int Game_Init();
int Game_Main();
int Game_Shutdown();
int Look_Ahead(Board game, Move &play_it, int depth);
int Machine_Play();

//--------------------------------------------------|
// GLOBALS
//--------------------------------------------------|

HINSTANCE	main_instance		= NULL;		// App descriptor
HWND		main_window_handle	= NULL;		// window descriptor
HDC			hDC;							// device context
RECT		window_rect;					// window client size
HBRUSH		white_brush;					// window brush

Board	tictactoe;		// tictactoe board
int		mode_play;		// indicate's the mode of play = 0 vs
	// human, 1 vs CPU as Maximizer, and 2 CPU as minimizer

char	buffer[80];		// used for printing

//--------------------------------------------------|
// FUNCTIONS
//--------------------------------------------------|
void Init_Junk()
{
// function is used for initialization puroposes
//	is a lazy man's work
	int reply = MessageBox(main_window_handle,
		"Would you like to play with the machine?", "Initializing",
		MB_YESNO | MB_ICONINFORMATION);

	if (reply == IDYES)
	{
		reply = MessageBox(main_window_handle, 
			"Computer plays as player 1?", "Finalizing", 
			MB_YESNO | MB_ICONINFORMATION);

		if (reply == IDYES)
		{
			mode_play = 2;
			Machine_Play();
		}
		else
			mode_play = 1;
	}
	else
		mode_play = 0;
} // end Init_Junk

//------------------------------------------------|

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
							WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT	ps;
	HDC			hdc;

	// what's the message
	switch (msg)
	{
	case WM_CREATE:
		{
			// moment of creation begins here

			// create the window brush
			white_brush = CreateSolidBrush(RGB(255,255,255));

			return(0);
		} break;

	case WM_LBUTTONDOWN:	// user is left-clicking
		{
			// get the current mouse points
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);

			// check if left-clicking occured within bounds
			if ((x >= 50 && x <= 350) &&
				(y >= 50 && y <= 350))
			{
				// where excatly
				if (x >= 50 && x < 150)
					x = 0;
				else
					if (x >= 150 && x < 250)
						x = 1;
					else
						x = 2;

				// now for y
				if (y >= 50 && y < 150)
					y = 0;
				else 
					if (y >= 150 && y < 250)
						y = 1;
					else
						y = 2;

				Move next(x, y);		// create the next move and...
				tictactoe.Play(next);	// ...play the game.

				if (mode_play)			// play machine only if allowed
					Machine_Play();

			} // end if

			return(0);
		} break;

	case WM_PAINT:
		{
			// vaildate the region and so...
			hdc = BeginPaint(hwnd, &ps);

			tictactoe.Draw_Config(hdc);

			EndPaint(hwnd, &ps);

			return(0);
		}break;

	case WM_DESTROY:
		{
			// delete brush
			DeleteObject(white_brush);

			// sned WM_QUIT message
			PostQuitMessage(0);

			return(0);
		} break;
		
	default: break;
	} // end message switch

	// let Windows handle the others
	return(DefWindowProc(hwnd, msg, wparam, lparam));

} // end WindowProc

//--------------------------------------------------|
// entry
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE hprevinstance,
				   LPSTR lpcmdline,
				   int ncmdshow)
{
	WNDCLASSEX	winclass;	// the win class struct
	HWND		hwnd;		// generic window handle
	MSG			msg;		// generic message

	// Uno, fill-in the window class
	winclass.cbClsExtra		= 0;
	winclass.cbSize			= sizeof(WNDCLASSEX);
	winclass.cbWndExtra		= 0;
	winclass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hIcon			= LoadIcon(hinstance, 
		MAKEINTRESOURCE(IDI_ICON1));
	winclass.hIconSm		= LoadIcon(hinstance, 
		MAKEINTRESOURCE(IDI_ICON1));
	winclass.hInstance		= hinstance;
	winclass.lpfnWndProc	= WindowProc;
	winclass.lpszClassName	= WINDOW_CLASS_NAME;
	winclass.lpszMenuName	= NULL;
	winclass.style			= CS_DBLCLKS | CS_OWNDC |
		CS_HREDRAW | CS_VREDRAW;

	main_instance = hinstance;

	// then, register class
	if (!RegisterClassEx(&winclass))
		return(0);

	// now create the window
	if (!(hwnd = CreateWindowEx(0, 
		WINDOW_CLASS_NAME,
		WINDOW_TITLE, 
		WS_POPUP | WS_VISIBLE,
		(GetSystemMetrics(SM_CXSCREEN)/2 - WINDOW_WIDTH/2), 
		(GetSystemMetrics(SM_CYSCREEN)/2 - WINDOW_HEIGHT/2),
		WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL,	// hwnd parent
		NULL,	// hmenu
		hinstance,
		NULL)))
		return(0);

	// set the global handle
	main_window_handle = hwnd;

	// initalize the main game
	Game_Init();

	// enter the main event loop, This time I'm using
	//	a sort of Real-Timed loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
			
		DispatchMessage(&msg);

		// check for Esc
		if (KEY_DOWN(VK_ESCAPE))
			SendMessage(hwnd, WM_CLOSE, 0, 0);

		// run game
		Game_Main();

	} // end while

	// shut down game
	Game_Shutdown();

	// over, bye
	return(msg.wParam);
} // end WinMain

//--------------------------------------------------|

int Game_Init()
{
	// Initializes game components and resources

	// seed random number generator
	srand(GetTickCount());

	// get the device context
	hDC = GetDC(main_window_handle);

	// intialize the client window size
	GetClientRect(main_window_handle, &window_rect);

	Init_Junk();	// call lazy man's work

	return(1);
} // end game_init


//--------------------------------------------------|

int Game_Main()
{
	// this is where most of the action
	//	takes place, this is the main loop of game

	// test for user changing game mode
	if (KEY_DOWN(VK_SPACE))
	{
		// clear the window
		FillRect(hDC, &window_rect, white_brush);
			
		// Initialize and draw
		tictactoe.Initialize();
		tictactoe.Draw_Config(hDC);

		// call Init Junk to do the dirty work
		Init_Junk();
		
	} // end if

	// Draw board
	tictactoe.Draw_Config(hDC);

	// check game status 
	if (tictactoe.Done())
	{
		int player = tictactoe.Check_Winner();
		if (player > 0)
		{
			sprintf(buffer, "Player %d Wins the game, Try Again?",
				player);
		} // end if
		else
			sprintf(buffer, "Game is a draw, Try Again?");

		int reply;

		// Prompt for retry or exit
		reply = MessageBox(main_window_handle,
			buffer, "Game Over",
			MB_YESNO | MB_ICONINFORMATION);

		if (reply == IDYES)			// reiniatialize game
		{
			// clear the window
			FillRect(hDC, &window_rect, white_brush);
			
			// Initialize and draw
			tictactoe.Initialize();
			tictactoe.Draw_Config(hDC);

			// call Init Junk to do the dirty work
			//Init_Junk();
			if (mode_play == 2)
				Machine_Play();

		} // end if
		else						// exit app
			SendMessage(main_window_handle, WM_CLOSE, 0, 0);

	} // end if

	return(1);
} // end game_main


//--------------------------------------------------|

int Game_Shutdown()
{
	// shut's down game and releases resources

	// release the DC
	ReleaseDC(main_window_handle, hDC);

	return(1);
} // end game_shutdown

//--------------------------------------------------|

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
		
		Move *possible_moves = new Move [depth];	// remainig moves always equals depth
		game.Legal_Moves(possible_moves);			// initalize these moves
		
		int worst_pay, payoff = game.Worst_Case();	// assume this is current game's estimate

		// loop and compare all the moves and select the best one
		for (int i = 0; i < depth; i++)
		{
			Move dummy;
			Board new_game = game;				// copy this game
			new_game.Play(possible_moves[i]);	// play the new game and
			worst_pay = Look_Ahead(new_game,	// look ahead with the next possible move
				dummy,	 						// played and get an estimate for this move
				depth - 1);
			if (game.Better(worst_pay, payoff))	// if indeed worst_pay, is worth it for this move
			{
				payoff	= worst_pay;			// then copy this move and return it
				play_it = possible_moves[i];
			} // end if

		} // end for

		return(payoff);

	} // end else 	

} // end Look_Ahead		

//--------------------------------------------------|

int Machine_Play()
{
// Mimics the machine play using the three step rule.
//	Declare, look ahead, and mark.

	Move ai;	// hold's the next move

	// look ahead in the game and initalize ai 
	// with the best move
	Look_Ahead(tictactoe, ai, 9 - tictactoe.Get_Moves_Played());
	
	// play ai
	tictactoe.Play(ai);

	return(0);

} // end Machine_Play

//--------------------------------------------------|