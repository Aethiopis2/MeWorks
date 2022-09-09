//--------------------------------------------------|
// Pathfinder.cpp -- Main module for pathfinder maze
//	game.
//
// By: Code Red
//
// Date created: 14th of May 2015, Thursday
// Last update: 22nd of May 2015, Friday
//
//--------------------------------------------------|

#define WIN32_LEAN_AND_MEAN			// say no to MFC
#define INITGUID					// initalize GUIDs

//--------------------------------------------------|
// INCLUDES											
//--------------------------------------------------|

#include <Windows.h>	// Windows headers
#include <Windowsx.h>
#include <MMsystem.h>

#include <stdio.h>		// standard C/C++ headers
#include <stdlib.h>
#include <malloc.h>
#include <objbase.h>
#include <io.h>

#include <ddraw.h>		// directX header
#include <dinput.h>	
#include <dsound.h>

#include "T3DLIB.H"
#include "Maze.h"
#include "Resource.h"

//--------------------------------------------------|
// DEFINES											
//--------------------------------------------------|

#define WINDOW_CLASS_NAME	"GAMECONSOLE"
#define WINDOW_TITLE		"PATHFINDER v1.00"

//--------------------------------------------------|
// TYPES											
//--------------------------------------------------|

// the player info struct
typedef struct PLAYER_STAT_TYP
{
	int player_x, player_y;		// positions of player
	//int width, height;		// dimension of bitmap
	int vx, vy;					// speed
	int color;					// drawing color
	int anim_count;				// animation transitions count
	int anim_limit;				// controlls animation rate
	//UCHAR *image[4];			// buffer to store image data

} PLAYER_STAT, *PLAYER_STAT_PTR;

//--------------------------------------------------|
// PROTOTYPES
//--------------------------------------------------|

int Game_Init(void *parms=NULL, int num_parms=0);				// initalizes game
int Game_Main(void *parms=NULL, int num_parms=0);				// main game loop
int Game_Shutdown(void *parms=NULL, int num_parms=0);			// shut's down game resources

int Init_This();

//--------------------------------------------------|
// GLOBALS
//--------------------------------------------------|

HINSTANCE	main_instance		= NULL;		// App descriptor
HWND		main_window_handle	= NULL;		// window descriptor
BOOL		window_closed		= 0;		// semaphore, tracks if window has closed

char		buffer[80];

// the maze object 
Maze my_maze;

PLAYER_STAT player;					// the player data

int level = 0;						// current level of game
int lrow = 10,						// level rows and cols 
	lcol = 10;

//--------------------------------------------------|
// FUNCTIONS
//--------------------------------------------------|

//--------------------------------------------------|

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

			return(0);
		} break;

	case WM_PAINT:
		{
			// vaildate the region and so...
			hdc = BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);

			return(0);
		}break;

	case WM_DESTROY:
		{
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
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hIcon			= LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON1));
	winclass.hIconSm		= LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON1));
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
		0, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL,	// hwnd parent
		NULL,	// hmenu
		hinstance,
		NULL)))
		return(0);

	// set the global handle
	main_window_handle = hwnd;

	// hide the cursor
	ShowCursor(FALSE);

	// initalize the main game
	Game_Init();
	
	// enter the main event loop, This time I'm using
	//	a sort of Real-Timed loop
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			// translate key messages
			TranslateMessage(&msg);

			// notify event handler - WindowProc
			DispatchMessage(&msg);

		} // end if peek

		// run game
		Game_Main();

	} // end while

	// shut down game
	Game_Shutdown();

	// over, bye
	return(msg.wParam);

} // end WinMain

//--------------------------------------------------|

int Game_Main(void *parms, int num_parms)
{
// this is where most of the action
//	takes place, this is the main loop of game

	// some more variables
	int cell	= player.player_x + player.player_y * my_maze.col;	// current cell
	int end		= my_maze.end_x + my_maze.end_y * my_maze.col;		// exit
	int start	= my_maze.start_x + my_maze.start_y*my_maze.col;	// begin

	// make sure this isn't executed again
	if (window_closed)
		return(0);

	// test if user is hitting 'Esc' key and send
	//	close message
	if (KEY_DOWN(VK_ESCAPE))
	{
		PostMessage(main_window_handle, WM_CLOSE, 0, 0);
		window_closed = 1;
	} // end if keydown

	// refresh screen keep's some old bugs 
	//	away, or so I think
	if (KEY_DOWN(VK_F5))
	{
		DDraw_Fill_Surface(lpddsprimary,0);
		DDraw_Fill_Surface(lpddsback,0);
	} // end if key down f5

	// load some other maze for this level
	if (KEY_DOWN(VK_F1))
	{
		// reset everything in the maze
		my_maze.Init_Maze(lrow,lcol,200);

		// reset player position
		player.player_x		= cell_pos[my_maze.start_x+my_maze.start_y*my_maze.col].x+4;
		player.player_y		= cell_pos[my_maze.start_x+my_maze.start_y*my_maze.col].y+4;
	} // end if

	// cheat levels
	if (KEY_DOWN(VK_DELETE))
	{
		if (lrow <= 40 && lcol <= 40)
		{
			++level;		// go up one
			if (lrow < lcol)
				lrow += 2;
			else
				lcol += 2;
		}

		my_maze.Init_Maze(lrow,lcol,200);
		Init_This();

	} // end if

	// start timming stuff
	Start_Clock();

	// clean surfac
	DDraw_Fill_Surface(lpddsback,0);

	// lock the back surface acquire a valid pointer to
	//	video memory
	DDraw_Lock_Back_Surface();

	// draw the maze
	my_maze.Draw_Maze(back_buffer, back_lpitch);

	// used for color detection
	int pixel_r, pixel_l, pixel_u, pixel_d = 0;

	// loop and scan for the current position's color
	for (int y = (player.player_y-3); y <= (player.player_y+3); y++)
	{
		for (int x = player.player_x-3; x <= player.player_x+3; x++)
		{
			// test directions
			if (x > player.player_x)
				pixel_r |= back_buffer[(x)+y*back_lpitch];
			else
			if (x <= player.player_x)
				pixel_l |= back_buffer[(x)+y*back_lpitch];

			// verticals
			if (y > player.player_y)
				pixel_d |= back_buffer[(x)+y*back_lpitch];
			else
			if (y <= player.player_y)
				pixel_u |= back_buffer[(x)+y*back_lpitch];

		} // end for x

	} // end for y

	// draw the player marker
	for (int i = (player.player_y-2); i <= (player.player_y + 2); i++)
	{
		for (int j = (player.player_x - 2); j <= (player.player_x + 2); j++)
			Draw_Pixel(j,i,player.color,back_buffer,back_lpitch);

	} // end for i

	// unlock the surface
	DDraw_Unlock_Back_Surface();

	// test for movement keys

	// find the current pos and test movemnets
	if (KEY_DOWN(VK_RIGHT))
	{
		// test for boundaries, after a color scan
		if (pixel_r == 0)
			player.player_x += player.vx;

	} // end if right
	else
	if (KEY_DOWN(VK_LEFT))
	{
		if (player.player_x-3 < GRID_START_X)
			player.player_x = GRID_START_X+3;

		if (pixel_l == 0)
			player.player_x -= player.vx;
		//else
		//	player.player_x += player.vx;	// bounce
	}
	else
	if (KEY_DOWN(VK_UP))
	{
		if (pixel_u == 0)
			player.player_y -= player.vy;
		//else
		//	player.player_y += player.vy;
	}
	else
	if (KEY_DOWN(VK_DOWN))
	{
		if (pixel_d == 0)
			player.player_y += player.vy;
		//else
		//	player.player_y -= player.vy;

	} // end if down

	// bounce marker only after player
	//	relased the keys
	if (pixel_r)
		player.player_x -= player.vx;		// bounce back
	
	if (pixel_l)
		player.player_x += player.vx;
	
	if (pixel_u)
		player.player_y += player.vy;
	
	if (pixel_d)
		player.player_y -= player.vy;

	// set the anim color
	if (++player.anim_count > player.anim_limit)
	{
		if (player.color == 50)
			player.color = 51;
		else
		if (player.color == 51)
			player.color = 49;
		else
			player.color = 50;

		player.anim_count = 0;	//reset

	} // end if anim_count

	// test for exit condition
	if ((player.player_x >= (cell_pos[end].x+cell_width)) &&
		(player.player_y <= (cell_pos[end].y+cell_height)) &&
		(player.player_y > (cell_pos[end].y)))
	{
		// reset, and increment level stuff
		if (lrow <= 40 && lcol <= 40)
		{
			++level;

			if (lrow < lcol)
				lrow += 2;
			else
				lcol += 2;
		} // end if

		// Init
		my_maze.Init_Maze(lrow,lcol,200);

		// intialize the player status
		player.anim_count	= 0;
		player.anim_limit	= 10;
		player.color		= 50;
		player.player_x		= cell_pos[my_maze.start_x+my_maze.start_y*my_maze.col].x+4;
		player.player_y		= cell_pos[my_maze.start_x+my_maze.start_y*my_maze.col].y+4;
		player.vx			= 3;
		player.vy			= 3;

	} // end if

	// draw some text info
	sprintf(buffer, "PATHFINDER v1.00   Level: %d   <F1> - Next Maze   <F5> - Refresh Screen   <Esc> - Exit", 
		level);
	Draw_Text_GDI(buffer,24,SCREEN_HEIGHT-24, 50, lpddsback);

	// filp surfaces
	DDraw_Flip();

	// wait 30 milli-seconds
	Wait_Clock(30);

	// return success
	return(1);

} // end game_main

//--------------------------------------------------|

int Game_Init(void *parms, int num_parms)
{
// Initializes game components and resources

	// Initalize direct draw
	DDraw_Init(screen_width, screen_height, screen_bpp);

	// clean surfaces
	DDraw_Fill_Surface(lpddsback,0);
	DDraw_Fill_Surface(lpddsprimary,0);

	// intialize the maze object
	my_maze.Init_Maze(lrow,lcol, 200);

	// intialize the player status
	player.anim_count	= 0;
	player.anim_limit	= 10;
	player.color		= 50;
	player.player_x		= cell_pos[my_maze.start_x+my_maze.start_y*my_maze.col].x+4;
	player.player_y		= cell_pos[my_maze.start_x+my_maze.start_y*my_maze.col].y+4;
	player.vx			= 3;
	player.vy			= 3;

	// start from level 1, logically
	++level;

	return(1);

} // end game_init

//--------------------------------------------------|

int Game_Shutdown(void *parms, int num_parms)
{
// shut's down game and releases resources

	// terminate directdraw
	DDraw_Shutdown();

	return(1);

} // end game_shutdown

//--------------------------------------------------|

int Init_This()
{
	// Init
	my_maze.Init_Maze(lrow,lcol,200);

	// intialize the player status
	player.anim_count	= 0;
	player.anim_limit	= 10;
	player.color		= 50;
	player.player_x		= cell_pos[my_maze.start_x+my_maze.start_y*my_maze.col].x+4;
	player.player_y		= cell_pos[my_maze.start_x+my_maze.start_y*my_maze.col].y+4;
	player.vx			= 3;
	player.vy			= 3;

	return(0);
}