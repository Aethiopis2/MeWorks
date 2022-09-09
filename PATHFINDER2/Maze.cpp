//-------------------------------------------------
// Maze.cpp - Contains the defintion of the maze
//	object (class). Is an extension of Maze.h
//
// By: Code Red
//
// Date created: 14th of May 2015, Thursday
// Last update: 22nd of May 2015, Friday
//
//-------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#define INITGUID

//-------------------------------------------------
// INCLUDES
//-------------------------------------------------
#include <windows.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stack>
using namespace std;

#include <ddraw.h>

#include "t3dlib.h"
#include "Maze.h"

//-------------------------------------------------
// GLOBALS
//-------------------------------------------------

// dimesions of a square
int cell_width;
int cell_height;

// walls for the square
WALLS cell_wall[16];

// hold the positions of the squares of the grid
SQUARE_POS *cell_pos;

// used for maze generation
USHORT direction[4] = {WEST, SOUTH, EAST, NORTH};	// directions of mov't
USHORT opposite[4] = {EAST, NORTH, WEST, SOUTH};
			
// four neighbouring cells, logically
int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};

//-------------------------------------------------
// FUNCTIONS
//-------------------------------------------------

Maze::Maze()
{
// constructor simply sets all the data members to NULL
//	for consistent data

	// set grid to NULL
	grid = NULL;

	// set the other data members to 0
	row = col = size = start_x = start_y = end_x = end_y = 0;

	// loop and do the same for the cell_walls
	for (int i = 0; i < 16; i++)
	{
		cell_wall[i].surface = NULL;
	
	} // end for i

} // end constructor

//-------------------------------------------------

Maze::~Maze()
{
// Destructor frees any dynamically allocated memories
//	before leaving the app

	// test if grid is valid
	if (grid)
		free(grid);

	// free walls surface
	for (int i = 0; i < 16; i++)
	{
		// test for valid ptr
		if (cell_wall[i].surface)
			free(cell_wall[i].surface);
	} // end for

} // end destructor

//-------------------------------------------------

int Maze::Draw_Maze(UCHAR *video, int mem_pitch)
{
// this function draws the entire grid via calls to 
//	Draw_Maze_Cell utility function

	// loop and draw the maze grid, cell by cell
	for (int i = 0; i < size; i++)
		Draw_Maze_Cell(cell_pos[i].x, cell_pos[i].y, video, mem_pitch, grid[i]);

	// success
	return(1);

} // end Draw_Maze

//-------------------------------------------------

int Maze::Draw_Maze_Cell(int x, int y, UCHAR *video_buffer, int mem_pitch, int cell)
{
// this function draw's the maze configuraton to the video surface
//	sent

	UCHAR pixel;

	// tivially reject false data
	if (cell < 0 || cell > 15)
		return(0);

	// pre-compute the starting addr of the video
	video_buffer += (x + y*mem_pitch);
	UCHAR *src = cell_wall[cell].surface;

	// loop and draw the maze based on the content's
	//	of grid as an index into cell_walls bitmap arrays
	for (int index_y = 0; index_y < cell_height; index_y++)
	{
		// loop for cols
		for (int index_x = 0; index_x < cell_width; index_x++)
		{	
			// read in the pixel
			if ((pixel = src[index_x]))
				video_buffer[index_x] = pixel;

		} // end for index_x

		// advance pointers
		video_buffer	+= (mem_pitch);
		src				+= cell_width;

	} // end for index_y

	return(1);

} // end Draw_Maze_cELL

//-------------------------------------------------

int Maze::Init_Maze(int new_row, int new_col, UCHAR color)
{
// this function properly initalizes the data members
//	of maze object, by first setting the values of row and col
//	from the parameter passed and by randomly initalizing the starting
//	and ending positions, and also invokes Init_Cell_Walls
//	to inialize the cell_wall images(shapes) based on the
//	dimensions of the cell.

	// seed the random number generator
	srand(GetTickCount());

	// now set row and col members
	row = new_row;
	col = new_col;
	size = row * col;		// total number of cells

	// allocate memory for the maze
	//	but first test for validity
	if (grid)
		free(grid);

	// begin allocation
	if (!(grid = (USHORT *)malloc(sizeof(USHORT) * size)))
		return(0);		// error

	// zero-out the grid
	memset(grid, 0, (sizeof(USHORT) * size));

	// set the starting and ending subscripts for the array
	start_x = 0;
	start_y = 1+rand()%(row-1);
	end_x = col-1;
	end_y = 1+rand()%(row-1);

	// invoke on Init_Cell_Walls
	Init_Cell_Walls(color);

	// initalize the cell postions
	Init_Cell_Pos();

	// generate the maze
	Generate_Maze();

	// returns success
	return(1);

} // end Init_Maze

//-------------------------------------------------

bool Maze::In_Bound(int pos)
{
// this function test's and return's a true if the current_cell
//	is within the bounds of the grid array, tests only vertical

	// perform test and return
	return(pos >= 0 && pos < size);

} // end In_Bound

//-------------------------------------------------

bool Maze::In_Bound(int x, int y)
{
// test for horizontal array bounds, avoids PAC-MAN
//	like Mazes

	//if ((x == 0 || x == col-1) && (direction[y] == WEST || direction[y] == EAST))
	if (((x == 0) && (direction[y] == WEST)) || ((x == col-1) && (direction[y] == EAST)))
		return(false);
	else
		return(true);
}

//-------------------------------------------------

int Maze::neighbor_count(int x, int y, BOOL *table)
{
// this function counts the number of neighbors a given
//	cell has a returns the count and a pointer of tables
//	to index the particular direction of movment

	int count = 0;			// count of neigbouring cells that can be moved to
	int curr = x + y*col;	// the current location in the linear array

	// loop and check all the neighbours
	for (int neighbor = 0; neighbor < 4; neighbor++)
	{
		// pre-compute the index of the next neighbor
		//	to make life easy
		int next = (x + dx[neighbor]) + ((y + dy[neighbor]) * col);

		// check if next neighbor has not been visited
		//	and is within the bounds of the array
		if ((In_Bound(next)) && (grid[next] == 0) && (In_Bound(x,neighbor)))
		{
			// set the table at this index to true
			//	or can be visited
			table[neighbor] = true;

			// increment the count
			++count;
			
		} // end if In_Bound && unvisited
		else
			table[neighbor] = false;	// mark visited

	} // end for

	// return the count
	return(count);

} // end neighbor_count

//------------------------------------------------

void Maze::Generate_Maze()
{
// this function generates a random maze based on a depth first
//	search algorithim

	BOOL table[4] = {0};

	// create a stack to hold cell locations
	stack<SQUARE>my_stack;

	// choose a cell at random and call it current cell
	SQUARE curr_cell;

	curr_cell.x = rand()%col;
	curr_cell.y = rand()%row;
	curr_cell.current_cell = &grid[curr_cell.x+curr_cell.y*col];

	// add one to visited count
	int visited = 1;

	while (visited < size)
	{
		// find neighbours of current_cell with all walls
		//	intact
		int count = neighbor_count(curr_cell.x, curr_cell.y,table);

		// if one or more is found
		if (count > 0)
		{
			// select one at random
			int i = rand()%4;
			while (!table[i])
			{
				if (++i > 4)
					i = 0;

			} // end while
 
			// knock down the wall b/n it and the current cell
			*curr_cell.current_cell |= direction[i];

			// push the current cell location the new cell
			my_stack.push(curr_cell);

			// make the new cell the current_cell
			curr_cell.x += dx[i];
			curr_cell.y += dy[i];

			curr_cell.current_cell = &grid[curr_cell.x + curr_cell.y*col];
			*(curr_cell.current_cell) |= opposite[i];

			++visited;	// add 1 to visited cells 

		} // end if
		else
		{
			if (!my_stack.empty())
			{
				// pop the most recent cell entry off the cell stack
				my_stack.pop();
				curr_cell = my_stack.top();
			} //end if
			else
				return;
		} // end else

	} // end while

	// set the starting and ending points
	grid[start_x + start_y*col]	|= WEST;		// open west wall for entry
	grid[end_x + end_y*col]		|= EAST;		// open east wall for exit

} // end Generate_Maze*/

//-------------------------------------------------

int Maze::Init_Cell_Pos()
{
// this function intializes the pointer cell_position according
//	to the number of squares of the grid

	// test for pointer
	if (cell_pos)
		free(cell_pos);

	// allocate memory for the cell position
	if (!(cell_pos = (SQUARE_POS *)malloc(sizeof(SQUARE_POS) * size)))
		return(0);

	// use alias pointer
	SQUARE_POS_PTR square = cell_pos;

	// now that we've got a valid pointer to cell_positions
	//	loop and initalize the starting positions of each cell
	for (int index_y = 0; index_y < row; index_y++)
	{
		for (int index_x = 0; index_x < col; index_x++)
		{
			// calculate the next position of cell
			square[index_x].x = GRID_START_X+index_x * (cell_width-1);
			square[index_x].y = GRID_START_Y+index_y * (cell_height-1);

		} // end for index_x

		// advance pointer
		square += col;

	} // end for index_y

	// success
	return(1);

} // end Init_Pos

//-------------------------------------------------

int Maze::Init_Cell_Walls(UCHAR color)
{
// this function initalizes the cell_width and height globals
//	based on the size (count) of squares and draws the possible
//	shapes of the cell_wall based on a static algorithim

	int index, i, j;		// looping vars

	// intialize the cell dimension globals
	//	NOTE: Logic always assumes that GRID_WIDTH(HEIGHT) >= col(row)
	cell_width	= (int)((GRID_WIDTH) / col);
	cell_height	= (int)((GRID_HEIGHT) / row);

	// loop and process all the cell walls
	for (index = 0; index < 16; index++)
	{
		// test and allocate memory for cell_wall surfaces
		if (cell_wall[index].surface)
			free(cell_wall[index].surface);

		// now allocate
		if (!(cell_wall[index].surface = (UCHAR *)malloc(cell_width*cell_height)))
			return(0);		// no good

		// zero-out the memory
		//memset(cell_wall[i].surface, 0, cell_width * cell_height);
		for (int k= 0; k < (cell_width*cell_height); k++)
			cell_wall[index].surface[k] = 0;//100;

		// now draw the wall shapes on the surface
		//	based on a very long switch statement, orders the
		//	shapes according to the values of maze grid, this 
		//	makes possible for effective indexing during drawing
		//	to video surface
		switch (index)
		{
		case 0:			// all walls up
			{
				// loop and set all walls up
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((j == 0 || j == (cell_width-1)) || (i == 0 || i == (cell_height-1)))	
							cell_wall[index].surface[j+ i*cell_width] = color;
					} // end for j
				} // end for i

			} break;

		case 1:		// north wall down
			{
				// set all the other walls expcept for
				//	north to up
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((j == 0 || j == (cell_width-1)) || (i == (cell_height-1)))				
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				} // end for i

			} break;

		case 2:		// east wall down
			{
				// set all walls up except for the east side
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((j == 0 ) || (i == 0) || (i == (cell_height - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				} // end for i

			} break;

		case 3:		// north-east wall down
			{
				// set all walls up except for the north and east side
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((j == 0 ) || (i == (cell_height - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				} // end for i

			} break;

		case 4:		// south wall down
			{
				// set all walls up expt south wall
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((i == 0) || (j == 0) || (j == (cell_width - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 5:		// north and south walls down
			{
				// set all walls up expt north and south walls
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((j == 0) || (j == (cell_width - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 6:		// south east walls down
			{
				// set all walls up expt south and east walls
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((i == 0) || (j == 0))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 7:		// north, east, south walls down
			{
				// set all walls down expt west wall
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((j == 0))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 8:		// west wall down
			{
				// set all walls up expt west wall
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((i == 0) || (i == (cell_height-1)) || (j == (cell_width - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 9:		// north west wall down
			{
				// set all walls up expt north and west wall
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((i == (cell_height-1)) || (j == (cell_width - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 10:	// west and east walls down
			{
				// set all walls up expt west and east walls
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((i == 0) || (i == (cell_height - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 11:	// north, east, west walls down
			{
				// set all walls down expt south wall
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((i == (cell_height - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 12:	// south west walls down
			{
				// set all walls up expt south west wall
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((i == 0) || (j == (cell_width - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 13:	// north, south, west, walls down
			{
				// set all walls down expt east wall
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((j == (cell_width - 1)))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 14:	// south, east, west walls down
			{
				// set all walls down expt north wall
				for (i = 0; i < cell_height; i++)
				{
					for (j = 0; j < cell_width; j++)
					{
						if ((i == 0))
							cell_wall[index].surface[j + i*cell_width] = color;
					} // end for j
				}// end for i

			} break;

		case 15: break;		// all down, no need of processing simply flow along
		default: break;

		} // end switch

	} // end for index

	// success
	return(0);

} // end Init_Cell_Walls

//-------------------------------------------------