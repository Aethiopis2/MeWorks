//-------------------------------------------------
// Maze.h - Maze class decleration.
//
// By: Code Red
//
// Date created: 14th of May 2015, Thursday
// Last update: 22nd of May 2015, Friday
//
//-------------------------------------------------
#ifndef MYMAZE
#define MYMAZE

//-------------------------------------------------
// DEFINES
//-------------------------------------------------

// these define the borders of the wall of every cell
//	in the maze grid
#define NORTH			1	
#define EAST			2
#define SOUTH			4
#define WEST			8

// these define the size of the maze grid
const int GRID_WIDTH		= SCREEN_WIDTH - 20;
const int GRID_HEIGHT		= SCREEN_HEIGHT - 40;

const int GRID_START_X		= ((SCREEN_WIDTH/2) - (GRID_WIDTH/2));		
const int GRID_START_Y		= ((SCREEN_HEIGHT/2) - (GRID_HEIGHT/2));

//-------------------------------------------------
// TYPES
//-------------------------------------------------

// these holds a single possiblity maze wall out of
//	the 16 in a linear pointer array
typedef struct WALLS_TYP
{
	UCHAR *surface;		// this holds the walls of a single maze cell
	
} WALLS, *WALLS_PTR;

// the structure holds the positions of the maze grid cells
//	interms of the screen coordinates
typedef struct SQUARE_POS
{
	int x,y;		// positions of a given cell

} SQUARE_POS, *SQUARE_POS_PTR;

struct SQUARE
{
	USHORT *current_cell;
	int x,y;
};

//-------------------------------------------------
// EXTERNS
//-------------------------------------------------

// these hold the dimensions of a single cell (square)
//	in a maze grid
extern int cell_width;
extern int cell_height;

//extern WALLS cell_wall[16];		// 16 possible walls for a given square(cell)
extern SQUARE_POS *cell_pos;

//-------------------------------------------------
// CLASS MAZE DECLRATION
//-------------------------------------------------

class Maze
{
public:

	Maze();	
	~Maze();
	int Init_Maze(int new_row, int new_col, UCHAR color);
	int Draw_Maze(UCHAR *video, int mem_pitch);

	// direct implements
	int Get_Size() { return(size); } 
	int Get_Grid_Value(int i) { return(grid[i]); }

private:

	USHORT *grid;		// the maze grid as a linear array
	int row;			// number of rows for this grid
	int size;			// col*row or the total number of squares in grid

public:

	// these mark the starting and ending position of the 
	//	maze,
	int col;			// number of columns 
	int start_x, start_y;
	int end_x, end_y;

private:

	// utilites (need not be visible to others) ones
	int Init_Cell_Pos();
	bool In_Bound(int pos);
	int Init_Cell_Walls(UCHAR color);
	void Generate_Maze();
	int neighbor_count(int x, int y, BOOL *);
	int Draw_Maze_Cell(int x, int y, UCHAR *video_buffer, int mem_pitch, int cell);
	USHORT Find_Walls(int cell, int index, USHORT flag);
	bool In_Bound(int x, int y);

}; // end Maze

#endif MYMAZE