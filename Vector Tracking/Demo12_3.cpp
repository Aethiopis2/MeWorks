//--------------------------------------------------|
// Demo12_3 - Vector Tracking demo. 				|
//	A minimal game.									|
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
#include <math.h>
#include <io.h>
#include <direct.h>

#include <ddraw.h>		// directX header
#include <dinput.h>	
#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>

#include "T3DLIB.H"		// Custom's
#include "T3DLIB2.H"
#include "T3DLIB3.H"

//--------------------------------------------------|
// DEFINES											
//--------------------------------------------------|

// Window constant's
#define WINDOW_CLASS_NAME	"GAMECONSOLE"
#define WINDOW_TITLE		"Demo"

// mathematical defines
const float FRICTION = (float)0.1;

// size of the universe
#define UNIVERSE_MIN_X		(-8000)
#define UNIVERSE_MAX_X		8000
#define UNIVERSE_MIN_Y		(-8000)
#define UNIVERSE_MAX_Y		8000

// wepons defines
#define MAX_PLASMA			32		// maximum number of plasma pulses
#define PLASMA_SPEED		16		// pixels/sec of plasma pulse
#define PLASMA_SPEED_SLOW	12		// slow mode for gunships

#define PLASMA_STATE_OFF	0		// this plasma is dead or off
#define PLASMA_STATE_ON		1		// this one is alive and is in flight
#define PLASMA_RANGE		30		// type 1 plasma disrupter

#define PLASMA_ANIM_PLAYER	0		// plasma tropedeo from player
#define PLASMA_ANIM_ENEMY	1		// a plasma tropedeo fro an enemy

// asteriod feild defines
#define MAX_ROCKS			300
#define ROCK_STATE_OFF		0
#define ROCK_STATE_ON		1

#define ROCK_LARGE			0		// sizes of rock
#define ROCK_MEDIUM			1
#define ROCK_SMALL			2

// explosion defines
#define MAX_BURSTS			8
#define BURST_STATE_OFF		0
#define BURST_STATE_ON		1

// defines for player states
#define PLAYER_STATE_DEAD		0
#define PLAYER_STATE_DYING		1
#define PLAYER_STATE_INVINCIBLE	2
#define PLAYER_STATE_ALIVE		3
#define PLAYER_REGEN_COUNT		100	// click before regenerating

#define WRAITH_INDEX_PTR		3	// index of direction var
#define MAX_PLAYER_SPEED		16

// sound id's
#define MAX_FIRE_SOUNDS			8
#define MAX_EXPL_SOUNDS			8

// defines for stations
#define MAX_STATIONS			20

// life state for stations
#define STATION_STATE_DEAD		0
#define STATION_STATE_ALIVE		1
#define STATION_STATE_DAMAGED	2
#define STATION_STATE_DYING		3

#define STATION_SHEILDS_ANIM_ON		0	// animations for sheidls
#define STATION_SHEILDS_ANIM_OFF	1

#define STATION_RANGE_RING		300

#define INDEX_STATION_DAMAGE	2	// tracks the current damage of the station
#define MAX_STATION_DAMAGE		100

#define STATION_MIN_UPLINK_DISTANCE	150

// defines for mines
#define MAX_MINES			16

// life state for mines
#define MINE_STATE_DEAD		0
#define MINE_STATE_ALIVE	1
#define MINE_STATE_DAMAGED	2
#define MINE_STATE_DYING	3

#define MINE_STATE_AI_ACTIVATED	1
#define MINE_STATE_AI_SLEEP		0

#define INDEX_MINE_AI_STATE			1	// state of ai system
#define INDEX_MINE_DAMAGE			2	// tracks the current damage of the mine
#define INDEX_MINE_CONTACT_COUNT	3	// tracks how long mine has been incontact with player

#define MAX_MINE_DAMAGE				50
#define MAX_MINE_CONTACT_COUNT		20

#define MAX_MINE_VELOCITY			16
#define MIN_MINE_TRACKING_DIST		1000
#define MIN_MINE_ACTIVATION_DIST	250

// defines for star feild
#define MAX_STARS		256		// number of stars in the universe

#define STAR_PLANE_0	0		// far plane
#define STAR_PLANE_1	1		// near plane
#define STAR_PLANE_2	2

#define STAR_COLOR_0	8		// color of farthest star plane
#define STAR_COLOR_1	7
#define STAR_COLOR_2	15		// color of nearest star plane

// defines for particle system
#define PARTICLE_STATE_DEAD		0
#define PARTICLE_STATE_ALIVE	1

// types of particles
#define PARTICLE_TYPE_FLICKER	0
#define PARTICLE_TYPE_FADE		1

// color of particle
#define PARTICLE_COLOR_RED		0
#define PARTICLE_COLOR_GREEN	1
#define PARTICLE_COLOR_BLUE		2
#define PARTICLE_COLOR_WHITE	3

#define MAX_PARTICLES			128

// color ranges
#define COLOR_RED_START			32
#define COLOR_RED_END			47

#define COLOR_GREEN_START		96
#define COLOR_GREEN_END			111

#define COLOR_BLUE_START		144
#define COLOR_BLUE_END			159

#define COLOR_WHITE_START		16
#define COLOR_WHITE_END			31

// indices used to access data arrays in BOBs
#define INDEX_WORLD_X			8
#define INDEX_WORLD_Y			9

// define for the state of the main loop
#define GAME_STATE_INIT				0
#define GAME_STATE_MENU				1
#define GAME_STATE_RESTART			2
#define GAME_STATE_RUNNING			3
#define GAME_STATE_UPLINK			4
#define GAME_STATE_EXIT				5
#define GAME_STATE_WAITING_FOR_EXIT	6
#define GAME_STATE_PAUSED			7

// the menu defines
#define MENU_STATE_MAIN				0	// main menu state
#define MENU_STATE_INST				1	// instructions state
#define MENU_SEL_NEW_GAME			0
#define MENU_SEL_NEW_INSTRUCTIONS	1
#define MENU_SEL_EXIT				2
#define MAX_INSTRUCTION_PAGES

// defines for font
#define FONT_NUM_CHARS			96	// entire charcter set
#define FONT_WIDTH				12
#define FONT_HEIGHT				12
#define FONT_WIDTH_NEXT_NUM		8
#define FONT_WIDTH_NEXT_LOWER	7
#define FONT_WIDTH_NEXT_UPPER	8

// number of starting objects
#define NUM_ACTIVE_MINES		16
#define NUM_ACTIVE_STATIONS		8

//--------------------------------------------------|
// TYPES											
//--------------------------------------------------|

// used to contain a single star
typedef struct STAR_TYP
{
	int x,y;		// position of star
	UCHAR color;	// color of star
	int plane;		// plane that the star is in

} STAR, *STAR_PTR;

// single particle
typedef struct PARTICLE_TYP
{
	int state;			// state of the particle
	int type;			// type of particle effect
	int x,y;			// world position of particle
	int xv,yv;			// velocity of particle
	int curr_color;		// the current rendering color
	int start_color;	// the start color or range of effect
	int end_color;		// the ending color
	int counter;		// general state transition
	int max_count;		// max value for counter

} PARTICLE, *PARTICLE_PTR;

//--------------------------------------------------|
// MACROS
//--------------------------------------------------|

//--------------------------------------------------|
// PROTOTYPES
//--------------------------------------------------|

int Game_Init(void *parms=NULL, int num_parms=0);				// initalizes game
int Game_Main(void *parms=NULL, int num_parms=0);				// main game loop
int Game_Shutdown(void *parms=NULL, int num_parms=0);			// shut's down game resources
void Game_Reset(void);

// helper functions for game logic
void Draw_Info(void);
void Start_Burst(int x, int y, int width, int height, int xv, int yv);
void Draw_Brusts(void);
void Move_Brusts(void);
void Delete_Brusts(void);
void Init_Brusts(void);
void Reset_Brusts(void);

void Draw_Rocks(void);
void Start_Rock(int x, int y, int size, int xv, int yv);
void Move_Rocks(void);
void Delete_Rocks(void);
void Init_Rocks(void);
void Reset_Rocks(void);

void Fire_Plasma(int x, int y, int xv,int yv, int source);
void Draw_Plasma(void);
void Move_Plasma(void);
void Delete_Plasma(void);
void Init_Plasma(void);
void Reset_Plasma(void);

void Move_Stars(void);
void Draw_Stars(void);
void Init_Stars(void);
void Reset_Stars(void);

void Init_Reset_Particles(void);
void Draw_Particles(void);
void Move_Particles(void);
void Start_Particles(int type, int color, int count, int x,int y, int xv,int yv);
void Start_Particle_Explosion(int type, int color, int count,
							  int x,int y, int xv,int yv, int num_particles);

void Reset_Particles(void);

void Init_Stations(void);
void Move_Stations(void);
void Draw_Stations(void);
void Start_Station(int override, int x,int y);
void Reset_Stations(void);

void Init_Mines(void);
void Move_Mines(void);
void Draw_Mines(void);
void Start_Mine(int override, int x,int y, int ai_state);
void Reset_Mines(void);

void Create_Tables(void);
float Fast_Distance_2D(float x, float y);
void Seed_Rocks(void);
int Load_Player(void);
void Draw_Scanner(void);
int Pad_Name(char *filename, char *extension, char *padstring, int num);

int Copy_Screen(UCHAR *source_bitmap, UCHAR *dest_buffer, int lpitch, int transparent);

void Load_Buttons(void);
void Load_HUD(void);

void Load_Redi(void);

inline void Copy_Palette(LPPALETTEENTRY dest, LPPALETTEENTRY source)
{
	memcpy(dest, source, 256*sizeof(PALETTEENTRY));
} // end Copy_Palette


//--------------------------------------------------|
// GLOBALS
//--------------------------------------------------|

HINSTANCE	main_instance		= NULL;		// App descriptor
HWND		main_window_handle	= NULL;		// window descriptor
BOOL		window_closed		= 0;		// semaphore, tracks if window has closed
char		buffer[80];

BITMAP_IMAGE background_bmp, menu_bmp;			// holds the backgrounds
BITMAP_IMAGE menu_sel_bmps[3];					// holds the menu selections
BITMAP_IMAGE redi;

BOB wraith;						// the player
BOB	plasma[MAX_PLASMA];			// PLASMA PULSES
BOB rocks[MAX_ROCKS];			// the asteriods
BOB rock_s, rock_m, rock_l;		// the master templates for surface info
BOB bursts[MAX_BURSTS];			// the explosion brusts
BOB stations[MAX_STATIONS];		// the starbase stations
BOB mines[MAX_MINES];			// the predator mines
BOB hud;						// the art for scanner hud
BOB stationsmall;				// small station bob

int rock_sizes[3] = {96,56,32};	// x,y dims for scaler

STAR stars[MAX_STARS];			// the star feild

PARTICLE particles[MAX_PARTICLES];	// the particles for the particle engine

// player state variables
int player_state			= PLAYER_STATE_ALIVE;
int player_score			= 0;					// the score
int player_ships			= 3;					// ships left
int player_regen_counter	= 0;					// used to track when to regen
int player_damage			= 0;					// damage of player
int player_counter			= 0;					// used for state transition tracking
int player_regen_count		= 0;					// used to regenarate player
int player_sheild_count		= 0;					// used to display sheilds
int ready_counter			= 0,					// used to draw a little "get ready"
	ready_state				= 0;

float mine_tracking_rate = 2;		// rate that mines track player

int win_counter	= 0,		// tracks if player won
	player_won	= 0;

int station_id				= -1,			// uplink station id
	num_stations_destroyed	= 0;

int intro_done	= 0;	// flags if intro has played already

int game_paused		= 0,
	pause_debounce	= 0,
	huds_on			= 1,
	scanner_on		= 1,
	huds_debounce	= 0,
	scanner_debounce= 0;

// color palette's so we don't have to reload all the time
PALETTEENTRY	game_palette[256];		// holds the main game palette
PALETTEENTRY	menu_palette[256];		// gee what do you think?
PALETTEENTRY	redi_palette[256];		// for me

// positional and state info for player
float player_x	= 0,
	  player_y	= 0,
	  player_dx	= 0,
	  player_dy	= 0,
	  player_xv	= 0,
	  player_yv	= 0,
	  vel		= 0;

// these contain the virual cos, sin lookup tables for the 16 sector circle
float cos_look16[16],
	  sin_look16[16];

// sound id's
int intro_music_id	= -1,
	main_music_id	= -1,
	ready_id		= -1,
	engines_id		= -1,
	scream_id		= -1,
	game_over_id	= -1,
	mine_powerup_id	= -1,
	deactivate_id	= -1,
	main_menu_id	= -1,
	beep0_id		= -1,
	beep1_id		= -1,
	station_blow_id	= -1,
	station_throb_id= -1;

int expl_ids[MAX_EXPL_SOUNDS]	= {-1,-1,-1,-1,-1,-1,-1,-1};
int fire_ids[MAX_FIRE_SOUNDS]	= {-1,-1,-1,-1,-1,-1,-1,-1};
int game_state					= GAME_STATE_INIT;			// inital game state

//--------------------------------------------------|
// FUNCTIONS
//--------------------------------------------------|

void Load_Redi(void)
{
// this function loads all the buttons for the interface

	// load image
	Load_Bitmap_File(&bitmap8bit, "OUTART/ENEMYAI.DAT");

	// save the palette
	Copy_Palette(redi_palette, bitmap8bit.palette);

	Create_Bitmap(&redi, 320-216/2,240-166/2, 216,166);
	Load_Image_Bitmap(&redi, &bitmap8bit,0,0, BITMAP_EXTRACT_MODE_ABS);

	// unload the object bitmap
	Unload_Bitmap_File(&bitmap8bit);

} // end Load_Redi

//--------------------------------------------------|

void Init_Reset_Particles(void)
{
// this function serves as both an init and reset for the particles

	// loop thru and reset all the particles to dead
	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		particles[index].state			= PARTICLE_STATE_DEAD;	
		particles[index].type			= PARTICLE_TYPE_FADE;
		particles[index].x				= 0;
		particles[index].y				= 0;
		particles[index].xv				= 0;
		particles[index].yv				= 0;
		particles[index].start_color	= 0;
		particles[index].end_color		= 0;
		particles[index].curr_color		= 0;
		particles[index].counter		= 0;
		particles[index].max_count		= 0;
	} // end for

} // end Init_Reset_Particles

//--------------------------------------------------|

void Start_Particle(int type, int color, int count, int x, int y, int xv, int yv)
{
// this function starts a single particle

	int pindex = -1;	// index of particle

	// first find open particle
	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		if (particles[index].state == PARTICLE_STATE_DEAD)
		{
			// set index
			pindex = index;
			break;
		} // end if
	} // end for

	// did we find one
	if (pindex == -1)
		return;

	// set general state info
	particles[pindex].state		= PARTICLE_STATE_ALIVE;
	particles[pindex].type		= type;
	particles[pindex].x			= x;
	particles[pindex].y			= y;
	particles[pindex].yv		= yv;
	particles[pindex].xv		= xv;
	particles[pindex].counter	= 0;
	particles[pindex].max_count	= count;

	// set color ranges always the same
	switch (color)
	{
	case PARTICLE_COLOR_RED:
		{
			particles[pindex].start_color	= COLOR_RED_START;
			particles[pindex].end_color		= COLOR_RED_END;
		} break;

	case PARTICLE_COLOR_GREEN:
		{
			particles[pindex].start_color	= COLOR_GREEN_START;
			particles[pindex].end_color		= COLOR_GREEN_END;
		} break;

	case PARTICLE_COLOR_BLUE:
		{
			particles[pindex].start_color	= COLOR_BLUE_START;
			particles[pindex].end_color		= COLOR_BLUE_END;
		} break;

	case PARTICLE_COLOR_WHITE:
		{
			particles[pindex].start_color	= COLOR_WHITE_START;
			particles[pindex].end_color		= COLOR_WHITE_END;
		} break;

	default: break;

	} // end switch

	// what type of particle is being requested
	if (type == PARTICLE_TYPE_FLICKER)
	{
		// set current color
		particles[pindex].curr_color = RAND_RANGE(particles[pindex].start_color, particles[pindex].end_color);
	} // end if
	else
	{
		// particle is fade type
		// set current color
		particles[pindex].curr_color = particles[index].start_color;
	} // end else

} // end Start_Particle

//--------------------------------------------------|

void Start_Particle_Explosion(int type, int color, int count,
							  int x, int y, int xv, int yv, int num_particles)
{
// this function starts a particle explosion at the given position and velocity

	while (--num_particles >= 0)
	{
		// compute random trajectory angle
		int ang = rand()%16;

		// compute trajectory velocuty
		float vel = float(2+rand()%4);

		Start_Particle(type,color,count,
			x+RAND_RANGE(-4,4), y+RAND_RANGE(-4,4),
			xv+int(cos_look16[ang]*vel), yv+int(sin_look16[ang]*16));

	} // end while

} // end Start_Particle_Explostion

//--------------------------------------------------|

void Draw_Particles(void)
{
// this function draws all the particles

	// lock back surface
	DDraw_Lock_Back_Surface();

	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		// test if particle is alive
		if (particles[index].state == PARTICLE_STATE_ALIVE)
		{
		// render the particle, perform world to screen transform
			int x = int(particles[index].x - player_x + (SCREEN_WIDTH/2));
			int y = int(particles[index].y - player_y + (SCREEN_HEIGHT/2));

			// test for clip
			if (x >= SCREEN_WIDTH || x < 0 || y >= SCREEN_WIDTH || y < 0)
				continue;

			// draw the pixel
			Draw_Pixel(x,y,particles[index].curr_color, back_buffer, back_lpitch);

		} // end if

	} // end for index

	// unlock the secondary surface
	DDraw_Unlock_Back_Surface();

} // end Draw_Particles

//--------------------------------------------------|

void Move_Particles(void)
{
// this function moves and animates all particles

	for (int index = 0; index < MAX_PARTICLES; index++)
	{
		// test if this particle is alive
		if (particles[index].state == PARTICLE_STATE_ALIVE)
		{
			// translate particle
			particles[index].x	+= particles[index].xv;
			particles[index].y	+= particles[index].yv;

			// now based on type of particle perform proper animation
			if (particles[index].type == PARTICLE_TYPE_FLICKER)
			{
				// choose a color in the color range and assign it to the current color
				particles[index].curr_color = RAND_RANGE(particles[index].start_color, particles[index].end_color);

				// now update counter
				if (++particles[index].counter >= particles[index].max_count)
				{
					// kill the particle
					particles[index].state = PARTICLE_STATE_DEAD;
				} // end if

			} // end if
			else
			{
				// musst be fade
				if (++particles[index].counter >= particles[index].max_count)
				{
					// reset counter
					particles[index].counter = 0;

					// update color
					if (++particles[index].curr_color > particles[index].end_color)
					{
						// transition is complete, terminate particle
						particles[index].state = PARTICLE_STATE_DEAD;
					} // end if

				} // end if
			} // end else

		} // end if
	} // end for index

} // end Move_Particles

//--------------------------------------------------|

void Init_Stars(void)
{
// this function initalizes all the stars in the star feild

	int index;		// looping var
	
	for (index = 0; index < MAX_STARS; index++)
	{
		// select the plane that star will be in
		switch (rand()%3)
		{
		case STAR_PLANE_0:
			{
				stars[index].color	= STAR_COLOR_0;
				stars[index].plane	= STAR_PLANE_0;
			} break;

		case STAR_PLANE_1:
			{
				stars[index].color	= STAR_COLOR_1;
				stars[index].plane	= STAR_PLANE_1;
			} break;

		case STAR_PLANE_2:
			{
				stars[index].color	= STAR_COLOR_2;
				stars[index].plane	= STAR_PLANE_2;
			} break;

		default: break;

		} // end switch

		// set the feilds that aren't plane specific
		stars[index].x = rand()%SCREEN_WIDTH;
		stars[index].y = rand()%SCREEN_HEIGHT;

	} // end for index

} // end Init_Stars

//--------------------------------------------------|

void Move_Stars(void)
{
// this function moves the star feild, note the star feild is always in
//	screen coordinates

	int index,			// looping var
		star_x,			// fast  aliases to star pos
		star_y,
		plane_0_dx,
		plane_0_dy,
		plane_1_dx,
		plane_1_dy,
		plane_2_dx,
		plane_2_dy;

	// pre coompute plane transitions
	plane_0_dx = -int(player_xv) >> 2;
	plane_0_dy = -int(player_yv) >> 2;

	plane_1_dx = -int(player_xv) >> 1;
	plane_1_dy = -int(player_yv) >> 1;

	plane_2_dx = -int(player_xv);
	plane_2_dy = -int(player_yv);

	// move all the stars based on the motion of the player
	for (index = 0; index < MAX_STARS; index++)
	{
		// locally chache star position to speed calculation
		star_x = stars[index].x;
		star_y = stars[index].y;

		// test which star feild star is in so it is translated
		//	with correct perspective

		switch (stars[index].plane)
		{
		case STAR_PLANE_0:
			{
				star_x += plane_0_dx;
				star_y += plane_0_dy;
			} break;

		case STAR_PLANE_1:
			{
				star_x += plane_1_dx;
				star_y += plane_1_dy;
			} break;

		case STAR_PLANE_2:
			{
				star_x += plane_2_dx;
				star_y += plane_2_dy;
			} break;

		} // end switch plane

		// test if star has flown off the edge
		if (star_x >= SCREEN_WIDTH)
			star_x = star_x-SCREEN_WIDTH;
		else
		if (star_x < 0)
			star_x = star_x+SCREEN_WIDTH;

		if (star_y >= SCREEN_HEIGHT)
			star_y = star_y-SCREEN_HEIGHT;
		else
		if (star_y < 0)
			star_y = star_y+SCREEN_HEIGHT;

		// reset star position in the structure
		stars[index].x = star_x;
		stars[index].y = star_y;

	} // end for index

} // end Move_Stars

//--------------------------------------------------|

void Draw_Stars(void)
{
// draws all the stars

	// lock back surface
	DDraw_Lock_Back_Surface();

	// draw all the stars
	for (int index=0; index < MAX_STARS; index++)
		Draw_Pixel(stars[index].x,stars[index].y, stars[index].color, back_buffer, back_lpitch);

	// unlock surface
	DDraw_Unlock_Back_Surface();

} // end Draw_Stars

//--------------------------------------------------|

void Draw_Scanner(void)
{
// this function draw's all the scanners

	int index, sx,sy;	// looping and position

	// lock the backsurface
	DDraw_Lock_Back_Surface();

	// draw all the rocks
	for (index = 0; index < MAX_ROCKS; index++)
	{
		// draw rock blips
		if (rocks[index].state == ROCK_STATE_ON)
		{
			sx = ((rocks[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (SCREEN_WIDTH/2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((rocks[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_Y) >> 7) + 32;

			Draw_Pixel(sx,sy, 8, back_buffer, back_lpitch);
		} // end if
	} // end for index

	// draw all the mines
	for (index = 0; index < MAX_MINES; index++)
	{
		// draw gunship blips
		if (mines[index].state == MINE_STATE_ALIVE)
		{
			sx = ((mines[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (SCREEN_WIDTH/2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((mines[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_X) >> 7) + 32;

			Draw_Pixel(sx,sy,12,back_buffer,back_lpitch);
			Draw_Pixel(sx,sy+1,12,back_buffer,back_lpitch);
		} // end if
	} // end for index

	// unlock the secondary surface
	DDraw_Unlock_Back_Surface();

	// draw all the stations
	for (index=0; index < MAX_STATIONS; index++)
	{
		// draw station blips
		if (stations[index].state == STATION_STATE_ALIVE)
		{
			sx = ((stations[index].varsI[INDEX_WORLD_X] - UNIVERSE_MIN_X) >> 7) + (SCREEN_WIDTH/2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
			sy = ((stations[index].varsI[INDEX_WORLD_Y] - UNIVERSE_MIN_X) >> 7) + 32;

			// test for state
			if (stations[index].anim_state == STATION_SHEILDS_ANIM_ON)
			{
				stationsmall.curr_frame	= 0;
				stationsmall.x			= sx - 3;
				stationsmall.y			= sy - 3;
				Draw_BOB(&stationsmall, lpddsback);
			} // end if
			else
			{
				stationsmall.curr_frame	= 1;
				stationsmall.x			= sx - 3;
				stationsmall.y			= sy - 3;
				Draw_BOB(&stationsmall, lpddsback);
			} // end else

		} // end if
	} // end for index

	// lock the secondary surface
	DDraw_Lock_Back_Surface();

	// draw player as white blip
	sx = ((int(player_x) - UNIVERSE_MIN_X) >> 7) + (SCREEN_WIDTH/2) - ((UNIVERSE_MAX_X - UNIVERSE_MIN_X) >> 8);
	sy = ((int(player_y) - UNIVERSE_MIN_X) >> 7) + 32;

	int col = rand()%256;

	Draw_Pixel(sx,sy,col,back_buffer,back_lpitch);
	Draw_Pixel(sx+1,sy,col,back_buffer,back_lpitch);
	Draw_Pixel(sx,sy+1,col,back_buffer,back_lpitch);
	Draw_Pixel(sx+1,sy+1,col,back_buffer,back_lpitch);

	// unlock the secondary surface
	DDraw_Unlock_Back_Surface();

	// now draw the art around the edges
	hud.x			= 320-64;
	hud.y			= 32-4;
	hud.curr_frame	= 0;
	Draw_BOB(&hud, lpddsback);

	hud.x			= 320+64-16;
	hud.y			= 32-4;
	hud.curr_frame	= 1;
	Draw_BOB(&hud, lpddsback);

	hud.x			= 320-64;
	hud.y			= 32+128-20;
	hud.curr_frame	= 2;
	Draw_BOB(&hud, lpddsback);

	hud.x			= 320+64-16;
	hud.y			= 32+128-20;
	hud.curr_frame	= 3;
	Draw_BOB(&hud, lpddsback);

} // end Draw_Scanner

//--------------------------------------------------|

void Init_Stations(void)
{
// this function loads and initalizes the stations to a known state

	static int shields_on_anim[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
		shields_off_anim[1] = {16};

	int frame;	// looping var

	// create the first bob
	Create_BOB(&stations[0],0,0,192,144,18,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY);

	// load animation frames
	for (frame = 0; frame <= 17; frame++)
	{
		// load the rocks imagery
		Pad_Name("OUTART/STATION", "BMP", buffer, frame);

		Load_Bitmap_File(&bitmap8bit, buffer);

		// load the actual bmp
		Load_Frame_BOB(&stations[0],&bitmap8bit,frame,0,0,BITMAP_EXTRACT_MODE_ABS);

		// unload data infile
		Unload_Bitmap_File(&bitmap8bit);
	} // end for

	// set state to off
	stations[0].state	= STATION_STATE_ALIVE;

	// set anim state
	stations[0].anim_state = STATION_SHEILDS_ANIM_ON;

	// set damage to 0
	stations[0].varsI[INDEX_STATION_DAMAGE] = 0;

	// set animation rate
	Set_Anim_Speed_BOB(&stations[0], 15);

	// load in the sheilds on/off animations
	Load_Animation_BOB(&stations[0], STATION_SHEILDS_ANIM_ON, 16, shields_on_anim);
	Load_Animation_BOB(&stations[0], STATION_SHEILDS_ANIM_OFF, 1, shields_off_anim);

	// set animation to on
	Set_Animation_BOB(&stations[0], STATION_SHEILDS_ANIM_ON);

	// make copies
	for (int ship=1; ship < MAX_STATIONS; ship++)
	{
		memcpy(&stations[ship], &stations[0], sizeof(BOB));
	} // end for copy

	// load the minature station
	Create_BOB(&stationsmall,0,0,8,8,2,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	Load_Bitmap_File(&bitmap8bit, "OUTART/STATIONSMALL8.BMP");

	// load the actual bmp
	Load_Frame_BOB(&stationsmall,&bitmap8bit,0,0,0,BITMAP_EXTRACT_MODE_CELL);
	Load_Frame_BOB(&stationsmall,&bitmap8bit,1,1,0,BITMAP_EXTRACT_MODE_CELL);

	// unload data infile
	Unload_Bitmap_File(&bitmap8bit);

} // end Init_Stations

//--------------------------------------------------|

void Reset_Stations(void)
{
// this function reset's all the stations in preparation for another run

	for (int index = 0; index < MAX_STATIONS; index++)
	{
		// set the state to off
		stations[index].state	= STATION_STATE_DEAD;

		// set anim state
		stations[index].anim_state = STATION_SHEILDS_ANIM_ON;

		// set damage to 0
		stations[index].varsI[INDEX_STATION_DAMAGE] = 0;

	} // end for

} // end Reset_Stations

//--------------------------------------------------|

void Start_Station(int override = 0, int x=0,  int y=0)
{
// this function starts a station, note that if override = 1
//	then the function uses the sent data otherwise it's random

	// first find an available station
	for (int index=0; index < MAX_STATIONS; index++)
	{
		// is this one dead
		if (stations[index].state == STATION_STATE_DEAD)
		{
			// position the station
			int xpos = RAND_RANGE((UNIVERSE_MIN_X+256), (UNIVERSE_MAX_X-256));
			int ypos = RAND_RANGE((UNIVERSE_MIN_Y+256), (UNIVERSE_MAX_Y-256));

			// set position
			stations[index].varsI[INDEX_WORLD_X] = xpos;
			stations[index].varsI[INDEX_WORLD_Y] = ypos;

			// set up a mine in the vicinity
			int ang = rand()%16;
			float mine_x = xpos + STATION_RANGE_RING*cos_look16[ang];
			float mine_y = ypos + STATION_RANGE_RING*sin_look16[ang];

			// start a deactivated mine
			Start_Mine(1, (int)mine_x, (int)mine_y, MINE_STATE_AI_SLEEP);

			// set velocity
			stations[index].xv = 0;
			stations[index].yv = 0;

			// set the remainig state variables
			stations[index].state = STATION_STATE_ALIVE;

			// set animation to on
			Set_Animation_BOB(&stations[index], STATION_SHEILDS_ANIM_ON);

			// set anim state
			stations[index].anim_state	= STATION_SHEILDS_ANIM_ON;

			// set damage to 0
			stations[index].varsI[INDEX_STATION_DAMAGE] = 0;

			// done so exit
			return;
		} // end if

	} // end for index

} // end Start_Station

//--------------------------------------------------|

void Move_Stations(void)
{
// this function moves/animates all the stations

	for (int index = 0; index < MAX_STATIONS; index++)
	{
		// test if station is alive
		if (stations[index].state == STATION_STATE_ALIVE)
		{
			// move the stations
			stations[index].varsI[INDEX_WORLD_X] += stations[index].xv;
			stations[index].varsI[INDEX_WORLD_Y] += stations[index].yv;

			// test boundaries
			if (stations[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X)
				stations[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			else
			if (stations[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X)
				stations[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;

			if (stations[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y)
				stations[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_Y;
			else
			if (stations[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y)
				stations[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;

			// test for damage level
			if (stations[index].varsI[INDEX_STATION_DAMAGE] > (MAX_STATION_DAMAGE/4) &&
				(rand()%(20 - (stations[index].varsI[INDEX_STATION_DAMAGE] >> 3))) == 1)
			{
				int width = 20+rand()%60;

				// start a brust
				Start_Burst(int(stations[index].varsI[INDEX_WORLD_X] - (stations[index].width*0.5)+RAND_RANGE(0,stations[index].width)),
					int(stations[index].varsI[INDEX_WORLD_Y] - (stations[index].height*0.5)+RAND_RANGE(0,stations[index].height)),
					width, int((width >> 2) + (width >> 1)),
					int(stations[index].xv*0.5), int(stations[index].yv*0.5));

				// add some particles

			} // end if

		} // end if alive

	} // end for index

} // end Move_Stations

//--------------------------------------------------|

void Draw_Stations(void)
{
// this function draws all the stations

	for (int index=0; index < MAX_STATIONS; index++)
	{
		// test if station is alivve
		if (stations[index].state == STATION_STATE_ALIVE)
		{
			// transform screen coords
			stations[index].x = int(stations[index].varsI[INDEX_WORLD_X] - (stations[index].width >> 1) - player_x + (SCREEN_WIDTH/2));
			stations[index].y = int(stations[index].varsI[INDEX_WORLD_Y] - (stations[index].height >> 2) - player_y + (SCREEN_HEIGHT/2));

			// draw the station
			Draw_BOB(&stations[index], lpddsback);

			// animate the station
			Animate_BOB(&stations[index]);

		} // end if 
	} // end for index

} // end Draw_Stations

//--------------------------------------------------|

void Delete_Stations(void)
{
// this function simply deletes all memory and surfaces
//	related to gunships

	for (int index=0; index < MAX_STATIONS; index++)
		Destroy_BOB(&stations[index]);

} // end Delete_Stations

//--------------------------------------------------|

void Init_Mines(void)
{
// this function loads and initlaizes the mines to a known state

	int frame;	// looping var

	// create the first bob
	Create_BOB(&mines[0],0,0,48,36,16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// load animation frames
	for (frame = 0; frame <= 15; frame++)
	{
		// load the mine
		Pad_Name("OUTART/PREDMINE", "BMP", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load the actual .BMP
		Load_Frame_BOB(&mines[0],&bitmap8bit,frame,0,0,BITMAP_EXTRACT_MODE_ABS);

		// unload the data infile
		Unload_Bitmap_File(&bitmap8bit);

	} // end if

	// set the state to off
	mines[0].state = MINE_STATE_DEAD;
	mines[0].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_SLEEP;
	mines[0].varsI[INDEX_MINE_DAMAGE] = 0;
	mines[0].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

	// set animation rate
	Set_Anim_Speed_BOB(&mines[0], 3);

	// make copies
	for (int mine = 1; mine < MAX_MINES; mine++)
		memcpy(&mines[mine], &mines[0], sizeof(BOB));

} // end Init_Mines

//--------------------------------------------------|

void Reset_Mines(void)
{
// this function resets all the mines

	// loop
	for (int mine = 0; mine < MAX_MINES; mine++)
	{
		mines[mine].state = MINE_STATE_DEAD;
		mines[0].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_SLEEP;
		mines[0].varsI[INDEX_MINE_DAMAGE] = 0;
		mines[0].varsI[INDEX_MINE_CONTACT_COUNT] = 0;
	} // end for mine

} // end Reset_Mines

//--------------------------------------------------|

void Start_Mine(int override=0, int x=0, int y=0, int ai_state=MINE_STATE_AI_ACTIVATED)
{
// this function stars a mine, if override is 1 the function uses the sent data

	// first find an available mine
	for (int index = 0; index < MAX_MINES; index++)
	{
		// is this one dead
		if (mines[index].state == MINE_STATE_DEAD)
		{
			if (!override)
			{
				// position the mine
				int xpos = RAND_RANGE((UNIVERSE_MIN_X+256), (UNIVERSE_MAX_X-256));
				int ypos = RAND_RANGE((UNIVERSE_MIN_Y+256), (UNIVERSE_MAX_Y-256));

				// set position
				mines[index].varsI[INDEX_WORLD_X] = xpos;
				mines[index].varsI[INDEX_WORLD_Y] = ypos;

				// set velocity
				mines[index].xv = RAND_RANGE(-8, 8);
				mines[index].yv = RAND_RANGE(-8, 8);

				// set state variable
				mines[index].state = MINE_STATE_ALIVE;

				// set damage to 0
				mines[index].varsI[INDEX_MINE_DAMAGE] = 0;

				// set the aiii state
				mines[index].varsI[INDEX_MINE_AI_STATE] = ai_state;

				// set contact count
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

				// done so exit
				return;
			} // end if
			else
			{
				// set position
				mines[index].varsI[INDEX_WORLD_X] = x;
				mines[index].varsI[INDEX_WORLD_Y] = y;

				// set velocity
				mines[index].xv = 0;
				mines[index].yv = 0;

				// set state variable
				mines[index].state = MINE_STATE_ALIVE;

				// set damage to 0
				mines[index].varsI[INDEX_MINE_DAMAGE] = 0;

				// set the aiii state
				mines[index].varsI[INDEX_MINE_AI_STATE] = ai_state;

				// set contact count
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;

				// done so exit
				return;
			} // end else
		} // end if alive

	} // end for index

} // end Start_mine

//--------------------------------------------------|

void Move_Mines(void)
{
// thiis function animates the mines

	for (int index=0; index < MAX_MINES; index++)
	{
		// test if mine is aliive
		if (mines[index].state == MINE_STATE_ALIVE)
		{
			// test for activation
			if (mines[index].varsI[INDEX_MINE_AI_STATE] == MINE_STATE_AI_ACTIVATED)
			{
				// move the miiines
				mines[index].varsI[INDEX_WORLD_X] += mines[index].xv;
				mines[index].varsI[INDEX_WORLD_Y] += mines[index].yv;

				// add damage trails
				if (mines[index].varsI[INDEX_MINE_DAMAGE] > (MAX_MINE_DAMAGE >> 1) &&
					(rand()%3) == 1)
				{
					Start_Particle(PARTICLE_TYPE_FLICKER, PARTICLE_COLOR_WHITE, 30+rand()%25,
						mines[index].varsI[INDEX_WORLD_X]+RAND_RANGE(-4,4),
						mines[index].varsI[INDEX_WORLD_Y]+RAND_RANGE(-4,4),
						int(mines[index].xv*0.125), int(mines[index].yv*0.125));

					Start_Particle(PARTICLE_TYPE_FADE, PARTICLE_COLOR_RED, 5,
						mines[index].varsI[INDEX_WORLD_X]+RAND_RANGE(-4,4),
						mines[index].varsI[INDEX_WORLD_Y]+RAND_RANGE(-4,4),
						int(mines[index].xv*0.125), int(mines[index].yv*0.125));
				} // end if

				// tracking algorithim

				// compute vector toward player
				float vx = player_x - mines[index].varsI[INDEX_WORLD_X];
				float vy = player_y - mines[index].varsI[INDEX_WORLD_Y];

				// normalize vector
				float length = Fast_Distance_2D(vx,vy);

				// only track if reasonlably close
				if (length < MIN_MINE_TRACKING_DIST)
				{
					vx=mine_tracking_rate*vx/length;
					vy=mine_tracking_rate*vy/length;

					// add velocity vector to current velocity
					mines[index].xv = int(mines[index].xv + vx);
					mines[index].yv = int(mines[index].yv + vy);

					// add a little noise
					if ((rand()%10)==1)
					{
						vx = RAND_RANGE(-1,1);
						vy = RAND_RANGE(-1,1);
						mines[index].xv += vx;
						mines[index].yv += vy;
					} // end if

					// test velocity vvector of mines
					length = Fast_Distance_2D(mines[index].xv, mines[index].yv);

					// test for velocity over flow and slow
					if (length > MAX_MINE_VELOCITY)
					{
						// slow down
						mines[index].xv *= 0.75;
						mines[index].yv *= 0.75;
					} // end if
				
				} // end if
				else
				{
					// add a random velocity component
					if ((rand()%30)==1)
					{
						vx = RAND_RANGE(-2,2);
						vy = RAND_RANGE(-2,2);

						// add velocity vector to current velocity
						mines[index].xv += vx;
						mines[index].yv += vy;

						// test for velocity vector of mines
						length = Fast_Distance_2D(mines[index].xv, mines[index].yv);

						// test for velocity over flow and slow
						if (length > MAX_MINE_VELOCITY)
						{
							// slow down
							mines[index].xv *= 0.75;
							mines[index].yv *= 0.75;
						} // end if
					} // end if

				} // end else

			} // end if activated
			else
			{
				// compute dist from player
				float vx = player_x - mines[index].varsI[INDEX_WORLD_X];
				float vy = player_y - mines[index].varsI[INDEX_WORLD_Y];

				float length = Fast_Distance_2D(vx,vy);

				if (length < MIN_MINE_ACTIVATION_DIST)
				{
					mines[index].varsI[INDEX_MINE_AI_STATE] = MINE_STATE_AI_ACTIVATED;

					// sound off
					DSound_Play(mine_powerup_id);
				} // end if

			} // end else

			// test for boundaries
			if (mines[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X)
				mines[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			else
			if (mines[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X)
				mines[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;

			if (mines[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y)
					mines[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_Y;
			else
			if (mines[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y)
				mines[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;

			// check for collusion with player
			if (player_state == PLAYER_STATE_ALIVE && 
				Collision_Test(player_x-(wraith.width*.5),
					player_y - (wraith.height*.5),
					wraith.width, wraith.height,
					mines[index].varsI[INDEX_WORLD_X]-(mines[index].width*.5),
					mines[index].varsI[INDEX_WORLD_Y]-(mines[index].height*.5),
					mines[index].width, mines[index].height))
			{
				// test for contact count
				if (++mines[index].varsI[INDEX_MINE_CONTACT_COUNT] > MAX_MINE_CONTACT_COUNT)
				{
					// add players to score
					player_score += 250;

					// damage player
					player_damage += 30;

					// engage sheilds
					player_sheild_count = 3;

					int width = 30+rand()%40;

					// start a brust
					Start_Burst(mines[index].varsI[INDEX_WORLD_X],
						mines[index].varsI[INDEX_WORLD_Y],
						width, (width*.5) + (width*.25),
						int(mines[index].xv)*.5, int(mines[index].yv)*.5);

					// kill the mine
					mines[index].state == MINE_STATE_DEAD;

					// start a new mine
					Start_Mine();

					// continue next mine
					continue;
					
				} // end if contact count

			} // end if
			else
			{
				// no collosion so reset
				mines[index].varsI[INDEX_MINE_CONTACT_COUNT] = 0;
			} // end else

		} // end if alive

	} // end for index

} // end Move_Mines

//--------------------------------------------------|

void Draw_Mines(void)
{
// ths function draws all the mines

	for (int index=0; index < MAX_MINES; index++)
	{
		// test if mine is alive
		if (mines[index].state == MINE_STATE_ALIVE)
		{
			// transform to screen coords
			mines[index].x = mines[index].varsI[INDEX_WORLD_X] - (mines[index].width >> 1) - player_x + (SCREEN_WIDTH/2);
			mines[index].y = mines[index].varsI[INDEX_WORLD_Y] - (mines[index].height >> 1) - player_y + (SCREEN_HEIGHT/2);

			// draw the mines
			Draw_BOB(&mines[index], lpddsback);

			// animate bob
			Animate_BOB(&mines[index]);

			// draw text info above mine
			sprintf(buffer, "VELOCITY[%f, %f]", mines[index].xv, mines[index].yv);
			Draw_Text_GDI(buffer,mines[index].x-32,mines[index].y-16, RGB(0,255,0), lpddsback);

		} // end if

	} // end for index

} // end Draw_Mines

//--------------------------------------------------|

void Delete_Mines(void)
{
// this function removes the mines

	for (int index=0; index < MAX_MINES; index++)
		Destroy_BOB(&mines[index]);

} // end Delete_mines

//--------------------------------------------------|

void Init_Plasma(void)
{
// this functiion intializes and loads all the plasma
//	wepon pulses

	// the plasma animation
	static int plasma_anim_player[] = {0,1,2,3,4,5,6,7},
		plasma_anim_enemy[8] = {8,9,10,11,12,13,14,15};

	// load the plasma imagery
	Load_Bitmap_File(&bitmap8bit, "OUTART/ENERGY8.BMP");

	// create the bob
	Create_BOB(&plasma[0],0,0, 8,8, 16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
		DDSCAPS_SYSTEMMEMORY);

	// load the aniimation frames
	for (int frame=0; frame < 16; frame++)
		Load_Frame_BOB(&plasma[0],&bitmap8bit,frame,frame%8,frame/8,BITMAP_EXTRACT_MODE_CELL);

	// set animation rate
	Set_Anim_Speed_BOB(&plasma[0], 1);

	// load animations
	Load_Animation_BOB(&plasma[0], PLASMA_ANIM_PLAYER, 8, plasma_anim_player);
	Load_Animation_BOB(&plasma[0], PLASMA_ANIM_ENEMY, 8, plasma_anim_enemy);

	// set the state to off
	plasma[0].state = PLASMA_STATE_OFF;

	// copy the others
	for (int pulse=1; pulse < MAX_PLASMA; pulse++)
		memcpy(&plasma[pulse], &plasma[0], sizeof(BOB));

	// unload the bitmap
	Unload_Bitmap_File(&bitmap8bit);

} // end Init_Plasma

//--------------------------------------------------|

void Reset_Plasma(void)
{
// this function reset's all the plasma pulses

	for (int pulse = 0; pulse < MAX_PLASMA; pulse++)
	{
		plasma[pulse].state = PLASMA_STATE_OFF;
	} // end for pulse

} // end Reset_Plasma

//--------------------------------------------------|

void Delete_Plasma(void)
{
// this function simply deletes all memory and surfaces
//	related to plasma pulses

	for (int index = 0; index < MAX_PLASMA; index++)
		Destroy_BOB(&plasma[index]);

} // end Delete_Plasma

//--------------------------------------------------|

void Move_Plasma(void)
{
// this function moves all the plasma pulses and checks for 
//	collision with the rocks

	for (int index = 0; index < MAX_PLASMA; index++)
	{
		// test if plasma pulse is in flight
		if (plasma[index].state == PLASMA_STATE_ON)
		{
			// move the pulse
			plasma[index].varsI[INDEX_WORLD_X] += plasma[index].xv;
			plasma[index].varsI[INDEX_WORLD_Y] += plasma[index].yv;

			// test boundaries
			if ( (++plasma[index].counter_1 > PLASMA_RANGE) ||
				(plasma[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X) ||
				(plasma[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X) ||
				(plasma[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y) ||
				(plasma[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y) )
			{
				//KILL THE pulse
				plasma[index].state = PLASMA_STATE_OFF;
				plasma[index].counter_1 = 0;

				// move to next pulse
				continue;
			} // end if plasma out of bounds

			// test for mines
			for (int mine=0; mine < MAX_MINES; mine++)
			{
				if (mines[mine].state == MINE_STATE_ALIVE &&
					plasma[index].anim_state == PLASMA_ANIM_PLAYER)
				{
					// test collision
					if (Collision_Test(plasma[index].varsI[INDEX_WORLD_X]-(plasma[index].width*.5),
						plasma[index].varsI[INDEX_WORLD_Y]-(plasma[index].height*.5),
						plasma[index].width, plasma[index].height,
						mines[mine].varsI[INDEX_WORLD_X]-(mines[mine].width*.5),
						mines[mine].varsI[INDEX_WORLD_Y]-(mines[mine].height*.5),
						mines[mine].width, mines[mine].height))
					{
						// kill the pulse
						plasma[index].state = PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						// do damage to mine
						mines[mine].varsI[INDEX_MINE_DAMAGE] += (1+rand()%3);

						// increase damage on the mine andd test for death
						if (mines[mine].varsI[INDEX_MINE_DAMAGE] > MAX_MINE_DAMAGE)
						{
							// killt the mine
							mines[mine].state = MINE_STATE_DEAD;

							// start a new mine
							Start_Mine();

							// add to player score
							player_score += 250;
						} // end if mine dead

						int width = 30+rand()%40;

						// start a brust
						Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
							plasma[index].varsI[INDEX_WORLD_Y],
							width, (width *.5) + (width*.25),
							int(mines[mine].xv)*.5, int(mines[mine].yv)*.5);

						break;
					} // end if collosion mine

				} // end if mine alive

			} // end for mines

//--------------------------------------------------|

			// test for stations
			for (int station=0; station < MAX_STATIONS; station++)
			{
				if (stations[station].state == STATION_STATE_ALIVE &&
					plasma[index].anim_state == PLASMA_ANIM_PLAYER)
				{
					// test for collosion
					if (Collision_Test(plasma[index].varsI[INDEX_WORLD_X]-(plasma[index].width*.5),
						plasma[index].varsI[INDEX_WORLD_Y]-(plasma[index].height*.5),
						plasma[index].width, plasma[index].height,
						stations[station].varsI[INDEX_WORLD_X]-(stations[station].width*.5),
						stations[station].varsI[INDEX_WORLD_Y]-(stations[station].height*.5),
						stations[station].width, stations[station].height) )
					{
						// kill the pulse
						plasma[index].state = PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						// do damage to station
						stations[station].varsI[INDEX_STATION_DAMAGE] += (1+rand()%3);

						// increase the damage on the station and test for death
						if (stations[station].varsI[INDEX_STATION_DAMAGE] > MAX_STATION_DAMAGE)
						{
							// kill the station
							stations[station].state = STATION_STATE_DEAD;

							// add to players score
							player_score += 10000;

							// has played won
							if (++num_stations_destroyed >= NUM_ACTIVE_STATIONS)
							{
								win_counter = 0;
								player_won = 1;
							} // end if

							// make big sound
							DSound_Play(station_blow_id);

						} // end if

						// start a burst
						Start_Burst(plasma[index].varsI	[INDEX_WORLD_X],
							plasma[index].varsI[INDEX_WORLD_Y],
							40 + rand()%10, 30+rand()%16,
							int(stations[station].xv*.5), int(stations[station].yv*.5));

						break;
					} // end if collosion

				} // end if alive

			} // end for stations

//--------------------------------------------------|

			// test for collision with player
			if (plasma[index].anim_state == PLASMA_ANIM_ENEMY && player_state == PLAYER_STATE_ALIVE &&
				Collision_Test(plasma[index].varsI[INDEX_WORLD_X]-(plasma[index].width*.5),
				plasma[index].varsI[INDEX_WORLD_Y]-(plasma[index].height*.5),
				plasma[index].width, plasma[index].height,
				player_x-(wraith.width*.5),
				player_y-(wraith.height*.5),
				wraith.width, wraith.height) )
			{
				Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
					plasma[index].varsI[INDEX_WORLD_Y],
					40+rand()%20, 30+rand()%16,
					int(player_xv)*.5, int(player_yv)*.5);

				// update player's damage
				player_damage += 2;

				// engage sheilds
				player_sheild_count = 3;

				// kill the pulse
				plasma[index].state = PLASMA_STATE_OFF;
				plasma[index].counter_1 = 0;

				continue;
			} // end if collide with player

//--------------------------------------------------|

			// test for collision with rocks
			for (int rock=0; rock < MAX_ROCKS; rock++)
			{
				if (rocks[rock].state == ROCK_STATE_ON)
				{
					// test for collision
					if (Collision_Test(plasma[index].varsI[INDEX_WORLD_X]-(plasma[index].width*.5),
						plasma[index].varsI[INDEX_WORLD_Y]-(plasma[index].height*.5),
						plasma[index].width, plasma[index].height,
						rocks[rock].varsI[INDEX_WORLD_X]-(rocks[rock].width*.5),
						rocks[rock].varsI[INDEX_WORLD_Y]-(rocks[rock].height*5),
						rocks[rock].width, rocks[rock].height))
					{
						// kill pulse
						plasma[index].state == PLASMA_STATE_OFF;
						plasma[index].counter_1 = 0;

						switch(rocks[rock].varsI[0])
						{
						case ROCK_LARGE:
							{
								// start explosion
								Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
									plasma[index].varsI[INDEX_WORLD_Y],
									68+rand()%12,54+rand()%10,
									rocks[rock].xv*.5, rocks[rock].yv*.5);

							} break;

						case ROCK_MEDIUM:
							{
								// start explosion
								Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
									plasma[index].varsI[INDEX_WORLD_Y],
									52+rand()%10,44+rand()%8,
									rocks[rock].xv*.5, rocks[rock].yv*.5);

							} break;

						case ROCK_SMALL:
							{
								// start explosion
								Start_Burst(plasma[index].varsI[INDEX_WORLD_X],
									plasma[index].varsI[INDEX_WORLD_Y],
									34-4+rand()%8,30-3+rand()%6,
									rocks[rock].xv*.5, rocks[rock].yv*.5);

							} break;

						} // end switch

						// update score
						player_score += rocks[rock].varsI[2];

						// test the strength of rock
						rocks[rock].varsI[2] -= 50;

						// split test
						if (rocks[rock].varsI[2] > 0 && rocks[rock].varsI[2] < 50)
						{
							// test the size of rock
							switch (rocks[rock].varsI[0])
							{
							case ROCK_LARGE:
								{
									// split into two medium
									Start_Rock(rocks[rock].varsI[INDEX_WORLD_X]+rand()%16,
										rocks[rock].varsI[INDEX_WORLD_Y]+rand()%16,
										ROCK_MEDIUM,
										rocks[rock].xv-2+rand()%4,rocks[rock].yv-2+rand()%4);

									Start_Rock(rocks[rock].varsI[INDEX_WORLD_X]+rand()%16,
										rocks[rock].varsI[INDEX_WORLD_Y]+rand()%16,
										ROCK_MEDIUM,
										rocks[rock].xv-2+rand()%4, rocks[rock].yv-2+rand()%4);

									// throw in a small
									if ((rand()%3) == 1)
									{
										Start_Rock(rocks[rock].varsI[INDEX_WORLD_X]+rand()%16,
											rocks[rock].varsI[INDEX_WORLD_Y]+rand()%16,
											ROCK_SMALL,
											rocks[rock].xv-2+rand()%4, rocks[rock].yv-2+rand()%4);
									} // end if

									// kill the original
									rocks[rock].state = ROCK_STATE_OFF;

								} break;

							case ROCK_MEDIUM:
								{
									// split into 1-3 small
									int num_rocks = 1+rand()%3;
									
									for(; num_rocks >= 1; num_rocks--)
									{
										Start_Rock(rocks[rock].varsI[INDEX_WORLD_X]+rand()%8,
											rocks[rock].varsI[INDEX_WORLD_Y]+rand()%8,
											ROCK_SMALL,
											rocks[rock].xv-2+rand()%4, rocks[rock].yv-2+rand()%4);
									} // end for num_rocks

									// kill the original
									rocks[rock].state = ROCK_STATE_OFF;

								} break;

							case ROCK_SMALL:
								{
									// just kill it
									rocks[rock].state = ROCK_STATE_OFF;
								} break;

							default: break;

							} // end switch

						} // end if split
						else
						if (rocks[rock].varsI[2] <= 0)
						{
							// kill rocks
							rocks[rock].state = ROCK_STATE_OFF;
						} // end else

						// break out of the loop
						break;

					} // end if collision

				} // end if rock alive

			} // end for rock

		} // end if

	} // end for index

} // end Move_Plasma

//--------------------------------------------------|

void Draw_Plasma(void)
{
// this function draws all the plasma pulses

	for (int index =0; index < MAX_PLASMA; index++)
	{
		// test if plasma pulse is in flight
		if (plasma[index].state == PLASMA_STATE_ON)
		{
			// transform to screen coords
			plasma[index].x = plasma[index].varsI[INDEX_WORLD_X] - (plasma[index].width >> 1) - player_x + (SCREEN_WIDTH/2);
			plasma[index].y = plasma[index].varsI[INDEX_WORLD_Y] - (plasma[index].height >> 1) - player_y + (SCREEN_HEIGHT/2);

			// draw the pulse
			Draw_BOB(&plasma[index] , lpddsback);

			// animate the pulse
			Animate_BOB(&plasma[index]);

		} // end if

	} // end for index

} // end Draw_Plasma

//--------------------------------------------------|

void Fire_Plasma(int x, int y, int xv, int yv, int source=PLASMA_ANIM_PLAYER)
{
// this function fires a plasma pulse at the given starting 
//	position and velocity, of course, one must be for this to work

	// scan for a pulse that is avaliable
	for (int index = 0; index < MAX_PLASMA; index++)
	{
		// is this one available
		if (plasma[index].state == PLASMA_STATE_OFF)
		{
			// start this one up,
			plasma[index].varsI[INDEX_WORLD_X] = x - (plasma[0].width*.5);
			plasma[index].varsI[INDEX_WORLD_Y] = y - (plasma[0].height*.5);

			plasma[index].xv			= xv;
			plasma[index].yv			= yv;
			plasma[index].curr_frame	= 0;
			plasma[index].state			= PLASMA_STATE_ON;
			plasma[index].counter_1		= 0; // used to track distance
			plasma[index].anim_state	= source;
			Set_Animation_BOB(&plasma[index], source);

			// start sound up
			for (int sound_index = 0; sound_index < MAX_FIRE_SOUNDS; sound_index++)
			{
				// test if this sound is playing
				if (DSound_Status_Sound(fire_ids[sound_index]) == 0)
				{
					DSound_Play(fire_ids[sound_index]);
					break;

				} // end if

			} // end for sound index

			// later
			return;

		} // end if

	} // end for

} // end Fire_Plasma

//--------------------------------------------------|

int Pad_Name(char *filename, char *extension, char *padstring, int num)
{
// builds up a pad name for file
	
	int index;
	char buffer[80];

	// build up blank padstring
	sprintf(padstring, "%s0000.%s", filename, extension);

	// this function pads a string with 0's
	itoa(num, buffer, 10);

	// compute the position of the last digit
	int last_digit = strlen(filename)+4-1;

	// now copy the number into the padstring at the correct position
	memcpy(padstring + last_digit - strlen(buffer) + 1, buffer, strlen(buffer));

	// return success
	return(1);

} // end Pad_Name

//--------------------------------------------------|

void Init_Rocks(void)
{
// this function initializes and loads all the rocks

	int frame;	// loop index

	// create the large rock
	Create_BOB(&rock_l,0,0,96,96,16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// load animation frames
	for (frame = 0; frame <= 15; frame++)
	{
		// load the rocks imagery
		Pad_Name("OUTART/ROCKL", "BMP", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load tha acutal bmp
		Load_Frame_BOB(&rock_l,&bitmap8bit,frame,0,0,BITMAP_EXTRACT_MODE_ABS);

		// unload the data file
		Unload_Bitmap_File(&bitmap8bit);

	} // end for

	// set animation rate
	Set_Anim_Speed_BOB(&rock_l, 1+rand()%5);
	Set_Vel_BOB(&rock_l, -4+rand()%8, 4+rand()%4);
	Set_Pos_BOB(&rock_l, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);

	// set size of rock
	rock_l.varsI[0] = ROCK_LARGE;
	rock_l.varsI[1] = rock_sizes[rock_l.varsI[0]];

	// set the state to off
	rock_l.state = ROCK_STATE_OFF;

	// create the medium rock
	Create_BOB(&rock_m,0,0,56,56,16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// load animation frames
	for (frame = 0; frame <= 15; frame++)
	{
		// load the rocks imagery
		Pad_Name("OUTART/ROCKM", "BMP", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load tha acutal bmp
		Load_Frame_BOB(&rock_m,&bitmap8bit,frame,0,0,BITMAP_EXTRACT_MODE_ABS);

		// unload the data file
		Unload_Bitmap_File(&bitmap8bit);

	} // end for

	// set animation rate
	Set_Anim_Speed_BOB(&rock_m, 1+rand()%5);
	Set_Vel_BOB(&rock_m, -4+rand()%8, 4+rand()%4);
	Set_Pos_BOB(&rock_m, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);

	// set size of rock
	rock_m.varsI[0] = ROCK_MEDIUM;
	rock_m.varsI[1] = rock_sizes[rock_m.varsI[0]];

	// set the state to off
	rock_m.state = ROCK_STATE_OFF;

	// create the small rock
	Create_BOB(&rock_s,0,0, 32,32, 16,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// load animation frames
	for (frame = 0; frame <= 15; frame++)
	{
		// load the rocks imagery
		Pad_Name("OUTART/ROCKS", "BMP", buffer, frame);
		Load_Bitmap_File(&bitmap8bit, buffer);

		// load tha acutal bmp
		Load_Frame_BOB(&rock_s,&bitmap8bit,frame,0,0,BITMAP_EXTRACT_MODE_ABS);

		// unload the data file
		Unload_Bitmap_File(&bitmap8bit);

	} // end for

	// set animation rate
	Set_Anim_Speed_BOB(&rock_s, 1+rand()%5);
	Set_Vel_BOB(&rock_s, -4+rand()%8, 4+rand()%4);
	Set_Pos_BOB(&rock_s, rand()%SCREEN_WIDTH, rand()%SCREEN_HEIGHT);

	// set size of rock
	rock_s.varsI[0] = ROCK_SMALL;
	rock_s.varsI[1] = rock_sizes[rock_s.varsI[0]];

	// set the state to off
	rock_s.state = ROCK_STATE_OFF;

	// make semi copies
	for (int rock=0; rock < MAX_ROCKS; rock++)
	{
		memcpy(&rocks[rock], &rock_l, sizeof(BOB));

		// set animation rate
		Set_Anim_Speed_BOB(&rocks[rock], 1+rand()%5);

		// set velocity
		Set_Vel_BOB(&rocks[rock], -4+rand()%8, -4+rand()%8);

		// set position
		Set_Pos_BOB(&rocks[rock], 0,0);
		rocks[rock].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
		rocks[rock].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

		// set the size of the rock
		rocks[rock].varsI[0] = rand()%3;
		rocks[rock].varsI[1] = rock_sizes[rocks[rock].varsI[0]];

		// based on the size of the rock sitch in the correct bitmaps
		//	and sizes
		switch (rocks[rock].varsI[1])
		{
		case ROCK_LARGE:
			{
				//copy dd bitmap surfaces
				memcpy(rocks[rock].image,rock_l.image,sizeof(LPDIRECTDRAWSURFACE4)*MAX_BOB_FRAMES);

				// SET width and height
				rocks[rock].width	= rock_l.width;
				rocks[rock].height	= rock_l.height;

			} break;

		case ROCK_MEDIUM:
			{
				//copy dd bitmap surfaces
				memcpy(rocks[rock].image,rock_m.image,sizeof(LPDIRECTDRAWSURFACE4)*MAX_BOB_FRAMES);

				// SET width and height
				rocks[rock].width	= rock_m.width;
				rocks[rock].height	= rock_m.height;

			} break;

		case ROCK_SMALL:
			{
				//copy dd bitmap surfaces
				memcpy(rocks[rock].image,rock_s.image,sizeof(LPDIRECTDRAWSURFACE4)*MAX_BOB_FRAMES);

				// SET width and height
				rocks[rock].width	= rock_s.width;
				rocks[rock].height	= rock_s.height;

			} break;

		default: break;

		} // end switch

	} // end for rock

} // end Init_Rocks

//--------------------------------------------------|

void Reset_Rocks(void)
{
// this function resets all the rocks

	for (int rock = 0; rock < MAX_ROCKS; rock++)
	{
		// reset state
		rocks[rock].state = ROCK_STATE_OFF;

	} // end for rocks

} // end Reset_Rocks

//--------------------------------------------------|

void Delete_Rocks(void)
{
// simply deletes all memory and surfaces

	for (int index = 0; index < MAX_ROCKS; index++)
		Destroy_BOB(&rocks[index]);

} // end Delete_Rocks

//--------------------------------------------------|

void Seed_Rocks(void)
{
// seeds the universe with rocks

	int index;	// loop

	for (index = 0; index < (MAX_ROCKS*.75); index++)
	{
		// is this rock avalable?
		if (rocks[index].state == ROCK_STATE_OFF)
		{
			// SET parameters
			Set_Anim_Speed_BOB(&rocks[index], 1+rand()%5);
			Set_Vel_BOB(&rocks[index], -6+rand()%12, -6+rand()%12);

			// set position
			Set_Pos_BOB(&rocks[index], 0,0);
			rocks[index].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
			rocks[index].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

			// set the size of rock
			rocks[index].varsI[0] = rand()%3;
			rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

			// set strength of rock
			switch(rocks[index].varsI[0])
			{
			case ROCK_LARGE:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 100+rand()%100;

					// copy dd bitmap
					memcpy(rocks[index].image, rock_l.image, sizeof(LPDIRECTDRAWSURFACE4)*MAX_BOB_FRAMES);

					// set width and height
					rocks[index].width	= rock_l.width;
					rocks[index].height	= rock_l.height;

				} break;

			case ROCK_MEDIUM:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 40+rand()%30;

					// copy dd bitmap
					memcpy(rocks[index].image, rock_m.image, sizeof(LPDIRECTDRAWSURFACE4)*MAX_BOB_FRAMES);

					// set width and height
					rocks[index].width	= rock_m.width;
					rocks[index].height	= rock_m.height;

				} break;

			case ROCK_SMALL:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 10;

					// copy dd bitmap
					memcpy(rocks[index].image, rock_s.image, sizeof(LPDIRECTDRAWSURFACE4)*MAX_BOB_FRAMES);

					// set width and height
					rocks[index].width	= rock_s.width;
					rocks[index].height	= rock_s.height;

				} break;

			default: break;

			} // end switch

			// turn rock on
			rocks[index].state = ROCK_STATE_ON;

		} // end if

	} // end for index

} // end Seed_Rocks

//--------------------------------------------------|

void Move_Rocks(void)
{
// this functon moves all the rocks

	for (int index = 0; index < MAX_ROCKS; index++)
	{
		// test if rock is active
		if (rocks[index].state == ROCK_STATE_ON)
		{
			// move rock
			rocks[index].varsI[INDEX_WORLD_X] += rocks[index].xv;
			rocks[index].varsI[INDEX_WORLD_Y] += rocks[index].yv;

			// test if object is off universe
			if (rocks[index].varsI[INDEX_WORLD_X] > UNIVERSE_MAX_X)
				rocks[index].varsI[INDEX_WORLD_X] = UNIVERSE_MIN_X;
			else
			if (rocks[index].varsI[INDEX_WORLD_X] < UNIVERSE_MIN_X)
				rocks[index].varsI[INDEX_WORLD_X] = UNIVERSE_MAX_X;

			if (rocks[index].varsI[INDEX_WORLD_Y] > UNIVERSE_MAX_Y)
				rocks[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MIN_Y;
			else
			if (rocks[index].varsI[INDEX_WORLD_Y] < UNIVERSE_MIN_Y)
				rocks[index].varsI[INDEX_WORLD_Y] = UNIVERSE_MAX_Y;

			// TEST for collision with rocks
			if (player_state == PLAYER_STATE_ALIVE && 
				Collision_Test(player_x-(wraith.width*.5), player_y-(wraith.height*.5),
				wraith.width, wraith.height,
				rocks[index].varsI[INDEX_WORLD_X]-(rocks[index].width*.5),
				rocks[index].varsI[INDEX_WORLD_Y]-(rocks[index].height*.5),
				rocks[index].width, rocks[index].height) )
			{
				// what size rock did we hit
				switch(rocks[index].varsI[0])
				{
				case ROCK_LARGE:
					{
						// start explosion
						Start_Burst(rocks[index].varsI[INDEX_WORLD_X], 
							rocks[index].varsI[INDEX_WORLD_Y],
							68+rand()%12, 54+rand()%10,
							rocks[index].xv*.5, rocks[index].yv*.5);

						// update player damage
						player_damage += 35;

						// update velocity vector
						player_xv += (rocks[index].xv);
						player_yv += rocks[index].yv;

						// rotate ship a bit
						wraith.varsI[WRAITH_INDEX_PTR] += (RAND_RANGE(-4,4));
						
						if (wraith.varsI[WRAITH_INDEX_PTR] > 15)
							wraith.varsI[WRAITH_INDEX_PTR] -= 15;
						else
						if (wraith.varsI[WRAITH_INDEX_PTR] < 0)
							wraith.varsI[WRAITH_INDEX_PTR] += 15;

					} break;

				case ROCK_MEDIUM:
					{
						// start explosion
						Start_Burst(rocks[index].varsI[INDEX_WORLD_X], 
							rocks[index].varsI[INDEX_WORLD_Y],
							52+rand()%10, 44+rand()%8,
							rocks[index].xv*.5, rocks[index].yv*.5);

						// update player damage
						player_damage += 15;

						// update velocity vector
						player_xv += (rocks[index].xv*.5);
						player_yv += (rocks[index].yv*.5);

						// rotate ship a bit
						wraith.varsI[WRAITH_INDEX_PTR] += (RAND_RANGE(-4,4));
						
						if (wraith.varsI[WRAITH_INDEX_PTR] > 15)
							wraith.varsI[WRAITH_INDEX_PTR] -= 15;
						else
						if (wraith.varsI[WRAITH_INDEX_PTR] < 0)
							wraith.varsI[WRAITH_INDEX_PTR] += 15;

					} break;

				case ROCK_SMALL:
					{
						// start explosion
						Start_Burst(rocks[index].varsI[INDEX_WORLD_X], 
							rocks[index].varsI[INDEX_WORLD_Y],
							34-4+rand()%8, 30-3+rand()%6,
							rocks[index].xv*.5, rocks[index].yv*.5);

						// update player damage
						player_damage += 5;

						// update velocity vector
						player_xv += (rocks[index].xv*.25);
						player_yv += (rocks[index].yv*.25);

						// rotate ship a bit
						wraith.varsI[WRAITH_INDEX_PTR] += (RAND_RANGE(-4,4));
						
						if (wraith.varsI[WRAITH_INDEX_PTR] > 15)
							wraith.varsI[WRAITH_INDEX_PTR] -= 15;
						else
						if (wraith.varsI[WRAITH_INDEX_PTR] < 0)
							wraith.varsI[WRAITH_INDEX_PTR] += 15;

					} break;

				} // end switch

				// update score
				player_score += rocks[index].varsI[2];

				// engage shields
				player_sheild_count = 3;

				// kill the original
				rocks[index].state = ROCK_STATE_OFF;

			} // end if collision

		} // end if rock

	} // end for index

	// now test if it's time to add a new rock to the list
	if (rand()%100==50)
	{
		// scan for a rock to initlaize
		for (index = 0; index < MAX_ROCKS; index++)
		{
			// is this rock available?
			if (rocks[index].state == ROCK_STATE_OFF)
			{
				// set params
				Set_Anim_Speed_BOB(&rocks[index], 1+rand()%5);
				Set_Vel_BOB(&rocks[index], -6+rand()%12, -6+rand()%12);
				Set_Pos_BOB(&rocks[index], 0,0);
				
				rocks[index].varsI[INDEX_WORLD_X] = RAND_RANGE(UNIVERSE_MIN_X, UNIVERSE_MAX_X);
				rocks[index].varsI[INDEX_WORLD_Y] = RAND_RANGE(UNIVERSE_MIN_Y, UNIVERSE_MAX_Y);

				// set the size of rock
				rocks[index].varsI[0] = rand()%3;
				rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

				switch(rocks[index].varsI[0])
				{
				case ROCK_LARGE:
					{
						// set hardness of rock
						rocks[index].varsI[2] = 100 + rand()%100;

						// copy dd bitmap
						memcpy(rocks[index].image, rock_l.image, MAX_BOB_FRAMES*sizeof(LPDIRECTDRAWSURFACE4));

						// set width and height
						rocks[index].width	= rock_l.width;
						rocks[index].height	= rock_l.height;

					} break;

				case ROCK_MEDIUM:
					{
						// set hardness of rock
						rocks[index].varsI[2] = 40 + rand()%30;

						// copy dd bitmap
						memcpy(rocks[index].image, rock_m.image, MAX_BOB_FRAMES*sizeof(LPDIRECTDRAWSURFACE4));

						// set width and height
						rocks[index].width	= rock_m.width;
						rocks[index].height	= rock_m.height;

					} break;

				case ROCK_SMALL:
					{
						// set hardness of rock
						rocks[index].varsI[2] = 10;

						// copy dd bitmap
						memcpy(rocks[index].image, rock_s.image, MAX_BOB_FRAMES*sizeof(LPDIRECTDRAWSURFACE4));

						// set width and height
						rocks[index].width	= rock_s.width;
						rocks[index].height	= rock_s.height;

					} break;

				default: break;
					
				} // end switch

				// turn rock on
				rocks[index].state = ROCK_STATE_ON;

				return;
			} // end if

		} // end for index

	} // end if

} // end Move_Rocks

//--------------------------------------------------|

void Start_Rock(int x, int y, int size, int xv, int yv)
{
// this function starts a rock up with the sent parms

	// scan for a rock to initlaize
	for (int index = 0; index < MAX_ROCKS; index++)
	{
		// is this rock available?
		if (rocks[index].state == ROCK_STATE_OFF)
		{
			// set params
			Set_Anim_Speed_BOB(&rocks[index], 1+rand()%5);
			Set_Vel_BOB(&rocks[index], xv, yv);
			Set_Pos_BOB(&rocks[index], 0,0);
				
			rocks[index].varsI[INDEX_WORLD_X] = x;
			rocks[index].varsI[INDEX_WORLD_Y] = y;

			// set the size of rock
			rocks[index].varsI[0] = size;
			rocks[index].varsI[1] = rock_sizes[rocks[index].varsI[0]];

			switch(rocks[index].varsI[0])
			{
			case ROCK_LARGE:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 100 + rand()%100;

					// copy dd bitmap
					memcpy(rocks[index].image, rock_l.image, MAX_BOB_FRAMES*sizeof(LPDIRECTDRAWSURFACE4));

					// set width and height
					rocks[index].width	= rock_l.width;
					rocks[index].height	= rock_l.height;

				} break;

			case ROCK_MEDIUM:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 40 + rand()%30;

					// copy dd bitmap
					memcpy(rocks[index].image, rock_m.image, MAX_BOB_FRAMES*sizeof(LPDIRECTDRAWSURFACE4));

					// set width and height
					rocks[index].width	= rock_m.width;
					rocks[index].height	= rock_m.height;

				} break;

			case ROCK_SMALL:
				{
					// set hardness of rock
					rocks[index].varsI[2] = 10;

					// copy dd bitmap
					memcpy(rocks[index].image, rock_s.image, MAX_BOB_FRAMES*sizeof(LPDIRECTDRAWSURFACE4));

					// set width and height
					rocks[index].width	= rock_s.width;
					rocks[index].height	= rock_s.height;

				} break;

			default: break;
					
			} // end switch

			// turn rock on
			rocks[index].state = ROCK_STATE_ON;

			return;
		
		} // end if

	} // end for index

} // end Start_Rock

//--------------------------------------------------|

void Draw_Rocks(void)
{
// this function draws all the rocks

	for (int index = 0; index < MAX_ROCKS; index++)
	{
		// test for active rock
		if (rocks[index].state == ROCK_STATE_ON)
		{
			// transform screen coords
			rocks[index].x = rocks[index].varsI[INDEX_WORLD_X] - (rocks[index].width*.5) - player_x + (SCREEN_WIDTH/2);
			rocks[index].y = rocks[index].varsI[INDEX_WORLD_Y] - (rocks[index].height*.5) - player_y + (SCREEN_HEIGHT/2);

			// draw rock
			Draw_BOB(&rocks[index], lpddsback);

			// animate
			Animate_BOB(&rocks[index]);

		} // end if

	} // end for index

} // end Draw_Rocks

//--------------------------------------------------|

void Init_Bursts(void)
{
// thiis functiion initaiizles and loads all the bursts

	// load the bursts iimagery
	Load_Bitmap_File(&bitmap8bit, "OUTART/EXPL8.BMP");

	// create the first BOB
	Create_BOB(&bursts[0], 0,0, 42,36, 14,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// load animation frames
	for (int frames = 0; frames < 14; frames++)
		Load_Frame_BOB(&bursts[0], &bitmap8bit, frames, frames%6,frames/6, BITMAP_EXTRACT_MODE_CELL);

	// set animation rate
	Set_Anim_Speed_BOB(&bursts[0], 1);

	// set size of burst
	bursts[0].varsI[0] = bursts[0].width;
	bursts[0].varsI[1] = bursts[0].height;

	// set state to off
	bursts[0].state = BURST_STATE_OFF;

	for (int burst =1; burst < MAX_BURSTS; burst++)
		memcpy(&bursts[burst], &bursts[0], sizeof(BOB));

	// unload data file
	Unload_Bitmap_File(&bitmap8bit);

} // end Init_Bursts

//--------------------------------------------------|

void Reset_Bursts(void)
{
// this function resets all the bursts

	for (int burst = 0; burst < MAX_BURSTS; burst++)
		bursts[burst].state = BURST_STATE_OFF;

} // end Reset_Bursts

//--------------------------------------------------|

void Delete_Bursts(void)
{
// this function delete's the burst BOBs

	for (int index = 0; index < MAX_BURSTS; index++)
		Destroy_BOB(&bursts[index]);

} // end Delete_Bursts

//--------------------------------------------------|

void Move_Bursts(void)
{
// this function moves all the bursts

	for (int index = 0; index < MAX_BURSTS; index++)
	{
		// terst if butst pulse is moving
		if (bursts[index].state == BURST_STATE_ON)
		{
			// move the burst
			bursts[index].varsI[INDEX_WORLD_X] += bursts[index].xv;
			bursts[index].varsI[INDEX_WORLD_Y] += bursts[index].yv;

			// test if burst if offscreen or done with the animation
			if (bursts[index].curr_frame >= bursts[index].num_frames-1)
				bursts[index].state = BURST_STATE_OFF;

		} // end if

	} // end for index

} // end Move_Bursts

//--------------------------------------------------|

void Draw_Bursts(void)
{
// this function draws all the bursts
	
	for (int index = 0; index < MAX_BURSTS; index++)
	{
		// test if burst is in flight
		if (bursts[index].state == BURST_STATE_ON)
		{
			// transform bursts into screen coords
			bursts[index].x = bursts[index].varsI[INDEX_WORLD_X] - (bursts[index].width*.5) - player_x + (SCREEN_WIDTH/2);
			bursts[index].y = bursts[index].varsI[INDEX_WORLD_Y] - (bursts[index].height*.5) - player_y + (SCREEN_HEIGHT/2);

			// is scaling needed
			if (bursts[index].varsI[0] != bursts[index].width ||
				bursts[index].varsI[1] != bursts[index].height)
			{
				// draw the burst scaled
				Draw_Scaled_BOB(&bursts[index],
					bursts[index].varsI[0],bursts[index].varsI[1], lpddsback);
			} // end if
			else
				Draw_BOB(&bursts[index], lpddsback);	// draw normal

			// animate the explosion
			Animate_BOB(&bursts[index]);
		} // end if

	} // end for index

} // end Draw_Bursts

//--------------------------------------------------|

void Start_Burst(int x, int y, int width, int height, int xv, int yv)
{
// this funcion starts a burst up

	// now test if it's time to add a new burst to the list

	// scan for burst to initiate
	for (int index = 0; index < MAX_BURSTS; index++)
	{
		// is this burst available?
		if (bursts[index].state == BURST_STATE_OFF)
		{
			// set animation rate
			Set_Anim_Speed_BOB(&bursts[index], 1);
			bursts[index].curr_frame = 0;

			// set velocity
			Set_Vel_BOB(&bursts[index], xv, yv);

			// set position
			Set_Pos_BOB(&bursts[index], 0,0);
			bursts[index].varsI[INDEX_WORLD_X] = x;
			bursts[index].varsI[INDEX_WORLD_Y] = y;

			// set size of burst
			bursts[index].varsI[0] = width;
			bursts[index].varsI[1] = height;

			// turn burst on
			bursts[index].state = BURST_STATE_ON;

			// SHOOT some particles out
			Start_Particle_Explosion(PARTICLE_TYPE_FLICKER, RAND_RANGE(PARTICLE_COLOR_RED, PARTICLE_COLOR_GREEN),
				20+rand()%40, bursts[index].varsI[INDEX_WORLD_X], bursts[index].varsI[INDEX_WORLD_Y], 
				bursts[index].xv, bursts[index].yv, 8+rand()%8);

			// start sound up
			for (int sound_index = 0; sound_index < MAX_EXPL_SOUNDS; sound_index++)
			{
				// test if this sound is playing
				if (DSound_Status_Sound(expl_ids[sound_index]) == 0)
				{
					DSound_Play(expl_ids[sound_index]);
					break;
				} // end if
			} // end for sound_index

			return;
		} // end if

	} // end for index

} // end Start_Burst

//--------------------------------------------------|

void Draw_Info(void)
{
// this functiion draws all the informatiion at the top of the screen

	char score[16];				// holds the score
	static int red_glow = 0;	// used for damage display
	static int warning_count = 0;

	// buiild up score string
	sprintf(score, "0000000%d", player_score);

	// bulid up final string
	sprintf(buffer, "SCORE %s", &score[strlen(score)-8]);
	Draw_Text_GDI(buffer, 10, 10, RGB(0,255,0), lpddsback);

	// draw damage
	sprintf(buffer, "DAMAGE %d%%", player_damage);

	if (player_damage < 90)
		Draw_Text_GDI(buffer,350-8*strlen(buffer),10,RGB(0,255,0), lpddsback);
	else
		Draw_Text_GDI(buffer,350-8*strlen(buffer),10,RGB(red_glow,0,0), lpddsback);

	// update glow
	if ((red_glow += 15) > 255)
		red_glow = 0;

	// draw ships
	sprintf(buffer, "SHIPS %d", player_ships);
	Draw_Text_GDI(buffer, 520, 10, RGB(0, 255, 0), lpddsback);

	sprintf(buffer, "VEL %.2f Kps", vel*(100/MAX_PLAYER_SPEED));
	Draw_Text_GDI(buffer, 10, 460, RGB(0,255,0), lpddsback);

	sprintf(buffer, "Pos [%.2f, %.2f]", player_x, player_y);
	Draw_Text_GDI(buffer,480,460, RGB(0,255,0), lpddsback);

	sprintf(buffer, "TRACKING RATE = %.2f", mine_tracking_rate);
	Draw_Text_GDI(buffer, 320-120, 460, RGB(0,255,0), lpddsback);

} // end Draw_Info

//--------------------------------------------------|

int Copy_Screen(UCHAR *source_bitmap, UCHAR *dest_buffer, int lpitch, int transparent)
{
// the function draws bitmap into the destination memory

	UCHAR *dest_addr,		// starting address of bitmap in destination surface
		*source_addr;		// source address of bitmap data

	UCHAR pixel;		// thee pxel

	int index_x, pixel_x;	// loopiing vars

	// terst f btmap s loaded

	// compute the starting destnation & starting adresses
	dest_addr	= dest_buffer;
	source_addr	= source_bitmap;

	// if transparent
	if (transparent)
	{
		// copy each line of biitmap into destination
		for (index_x = 0; index_x < SCREEN_HEIGHT; index_x++)
		{
			// copy the memory
			for (pixel_x = 0; pixel_x < SCREEN_WIDTH; pixel_x++)
			{
				if ((pixel = source_addr[pixel_x]) != 0)
					dest_addr[pixel_x] = pixel;
			} // end for 

			// advance all pointers
			dest_addr	+= lpitch;
			source_addr	+= SCREEN_WIDTH;
		} // end for index
	} // end if
	else
	{
		// non transparent version
		for (index_x=0; index_x < SCREEN_HEIGHT; index_x++)
		{
			// copy memory
			memcpy(dest_addr, source_addr, SCREEN_WIDTH);

			// advance pointers
			dest_addr	+= lpitch;
			source_addr += SCREEN_WIDTH;
		} // end for
	} // end else

	// success
	return(1);

} // end Copy_Screen

//--------------------------------------------------|

void Do_Intro(void)
{
// the worlds simplest intro

	// clear out buffers
	DDraw_Fill_Surface(lpddsback, 0);
	DDraw_Fill_Surface(lpddsprimary, 0);

	// draw in the logo screen
	if (Load_Bitmap_File(&bitmap8bit, "outart/trackintro.BMP") == 0)
		return;

	// set the palette
	Set_Palette(bitmap8bit.palette);

	// copu bitmap into primary buffer
	DDraw_Lock_Primary_Surface();
	Copy_Screen(bitmap8bit.buffer, primary_buffer, primary_lpitch, 0);
	DDraw_Unlock_Primary_Surface();

	// unload bitmap file
	Unload_Bitmap_File(&bitmap8bit);

	// wait until player hits space bar
	while(!KEY_DOWN(VK_SPACE));
	//Sleep(5000);

	// transition to black
	Screen_Transitions(SCREEN_DARKNESS, NULL, 0);

	// clear out buffers
	DDraw_Fill_Surface(lpddsback, 0);
	DDraw_Fill_Surface(lpddsprimary, 0);

} // end Do_Intro

//--------------------------------------------------|

int Load_Sound_Music(void)
{
// this function loads all the sounds and music

	// load the intro music
	if ((intro_music_id = DSound_Load_WAV("OUTSOUND/INTRO.WAV")) == -1)
		return(0);

	// load the main music
	if ((main_music_id = DSound_Load_WAV("OUTSOUND/starsng.WAV")) == -1)
		return(0);

	// load get ready
	if ((ready_id = DSound_Load_WAV("OUTSOUND/ENTERING1.WAV")) == -1)
		return(0);

	// load engines
	if ((engines_id = DSound_Load_WAV("OUTSOUND/ENGINES.WAV")) == -1)
		return(0);

	// load scream
	if ((scream_id = DSound_Load_WAV("OUTSOUND/BREAKUP.WAV")) == -1)
		return(0);

	// load game over
	if ((game_over_id = DSound_Load_WAV("OUTSOUND/GAMEOVER.WAV")) == -1)
		return(0);

	// load mine powerup
	if ((mine_powerup_id = DSound_Load_WAV("OUTSOUND/MINEPOWER1.WAV")) == -1)
		return(0);

	if ((deactivate_id = DSound_Load_WAV("OUTSOUND/DEACTIVATE1.WAV")) == -1)
		return(0);

	if ((station_throb_id = DSound_Load_WAV("OUTSOUND/STATIONTHROB.WAV")) == -1)
		return(0);

	if ((beep0_id = DSound_Load_WAV("OUTSOUND/BEEP3.WAV")) == -1)
		return(0);

	if ((beep1_id = DSound_Load_WAV("OUTSOUND/BEEP1.WAV")) == -1)
		return(0);

	// load the explosion sounds
	if ((station_blow_id = DSound_Load_WAV("OUTSOUND/STATIONBLOW.WAV")) == -1)
		return(0);

	// these are two d/t source masters
	if ((expl_ids[0] = DSound_Load_WAV("OUTSOUND/EXPL1.WAV")) == -1)
		return(0);

	if ((expl_ids[1] = DSound_Load_WAV("OUTSOUND/EXPL2.WAV")) == -1)
		return(0);

	//  NOW make the copies
	for (int index = 2; index < MAX_EXPL_SOUNDS; index++)
		expl_ids[index] = DSound_Replicate_Sound(expl_ids[rand()%2]);

	// load the plasma wepon sounds
	if ((fire_ids[0] = DSound_Load_WAV("OUTSOUND/PULSE.WAV")) == -1)
		return(0);

	// make copies
	for (index = 1; index < MAX_FIRE_SOUNDS; index++)
		fire_ids[index] = DSound_Replicate_Sound(fire_ids[0]);

	// success
	return(1);

} // end Load_Sound_Music

//--------------------------------------------------|

void Load_HUD(void)
{
// this function loads the animation for HUD

	int index;		// looping variable

	// load the wraith ship
	Load_Bitmap_File(&bitmap8bit, "OUTART/HUDART8.BMP");

	// now create the wraith
	Create_BOB(&hud,0,0,24,26,4,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// load hud frames
	for (index = 0; index < 4; index++)
		Load_Frame_BOB(&hud,&bitmap8bit,index,index,0,BITMAP_EXTRACT_MODE_CELL);

	// unload the data file
	Unload_Bitmap_File(&bitmap8bit);

} // end Load_HUD

//--------------------------------------------------|

int Load_Player(void)
{
// this function loads the animation for the player

	int index;	// looping var

	// load the wraith ship
	Load_Bitmap_File(&bitmap8bit, "OUTART/WRAITH8.bmp");

	// set the palette to background image platte
	Set_Palette(bitmap8bit.palette);

	// now create the wriath
	Create_BOB(&wraith,0,0, 64,64, 35,
		BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
		DDSCAPS_SYSTEMMEMORY);

	// load wraith frames
	for (index = 0; index < 35; index++)
		Load_Frame_BOB(&wraith, &bitmap8bit, index, index%8,index/8, BITMAP_EXTRACT_MODE_CELL);

	// set position
	Set_Pos_BOB(&wraith, (SCREEN_WIDTH/2)-wraith.width/2, (SCREEN_HEIGHT/2)-wraith.height/2);

	// set starting direction
	wraith.varsI[WRAITH_INDEX_PTR] = 0;

	// unload the data file
	Unload_Bitmap_File(&bitmap8bit);

	// return success
	return(1);

}

//--------------------------------------------------|

void Create_Tables(void)
{
// this function creates all the lookup table values 

	for (int ang = 0; ang < 16; ang++)
	{
		float fang = PI*(ang*22.5)/180;

		cos_look16[ang] = -cos(fang+PI/2);
		sin_look16[ang] = -sin(fang+PI/2);

	} // end for ang

} // end Create_Tables

//--------------------------------------------------|

float Fast_Distance_2D(float x, float y)
{
// this function calcuates the distance from 0,0 to x,y

	// first compute the absolute value of x,y
	x = fabs(x);
	y = fabs(y);

	// compute the minimum of x,y
	float mn = MIN(x,y);

	// return the distance
	return(x+y-(mn/2)-(mn/4)+(mn/8));

} // end fast distance 2d

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
	winclass.hIcon			= LoadIcon(NULL,IDI_APPLICATION);
	winclass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
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
	return((int)msg.wParam);

} // end WinMain

//--------------------------------------------------|

void Game_Reset(void)
{
// this function resets all the game related systems and vars for another run

	int index;	// looping var

	// player state variable
	player_state			= PLAYER_STATE_ALIVE;
	player_score			= 0;	// the socre
	player_ships			= 3;	// ships left
	player_regen_counter	= 0;	// used to track when to regen
	player_damage			= 0;	// damge of player
	player_counter			= 0;	// used for state transition tracking
	player_regen_count		= 0;	// used to regenerate player
	player_sheild_count		= 0;	// used to display sheilds
	win_counter	= 0;
	player_won	= 0;
	num_stations_destroyed	= 0;

	//menu_up		= 0;	// flags the manin menu is up
	//intro_done	= 0;	// flags if intro has played

	// positional and state info for player
	player_x	= 0;
	player_y	= 0;
	player_dx	= 0;
	player_dy	= 0;
	player_xv	= 0;
	player_yv	= 0;
	vel			= 0;
	wraith.varsI[WRAITH_INDEX_PTR]	= 0;
	game_state = GAME_STATE_RESTART;
	station_id = -1;

	ready_counter = 0;		// used to draw a little "get ready"
	ready_state	  = 0;

	// call all the reset functions
	Init_Reset_Particles();

	Reset_Mines();
	Reset_Rocks();
	Reset_Plasma();
	Reset_Bursts();
	Reset_Stations();

	// start the rocks
	Seed_Rocks();

	// start the mines
	for (index = 0; index < NUM_ACTIVE_MINES; index++)
		Start_Mine();

	// start the stations
	for (index = 0; index < NUM_ACTIVE_STATIONS; index++)
		Start_Station();

} // end Game_Reset

//--------------------------------------------------|

int Game_Main(void *parms, int num_parms)
{
// this is where most of the action
//	takes place, this is the main loop of game

	int index;					// looping var
	int dx, dy;					// general deltas used in collision detection
	float xv = 0, yv = 0;		// used to compute velocity
	static int engines_on = 0;	// tracks state of engines each frame
	static int redi_up = 0;

	// what state is the main loop in
	if (game_state == GAME_STATE_INIT)
	{
		// reset everything

		// transition into menu state
		//game_state = GAME_STATE_MENU;

		// transition into run state
		game_state = GAME_STATE_RESTART;
	} // end if game state init
	else
	if (game_state == GAME_STATE_MENU)
	{
		game_state = GAME_STATE_RESTART;
	} // END IF
	else
	if (game_state == GAME_STATE_RESTART)
	{
		// start the main music
		DSound_Play(main_music_id, DSBPLAY_LOOPING);

		// restart the game
		Game_Reset();

		// set to run state
		game_state = GAME_STATE_RUNNING;
	} // end if restart
	else
	if (game_state == GAME_STATE_RUNNING)
	{
		// start the timming clock
		Start_Clock();

		// clear the drawing surface
		DDraw_Fill_Surface(lpddsback, 0);

		// read all input devices
		DInput_Read_Keyboard();

		// only process if player is alive
		if (player_state == PLAYER_STATE_ALIVE)
		{
			// test for tracking rate change
			if (keyboard_state[DIK_EQUALS])
			{
				if ((mine_tracking_rate += 0.1) > 4.0)
					mine_tracking_rate = 4.0;
			} // end if
			else
			if (keyboard_state[DIK_MINUS])
			{
				if ((mine_tracking_rate -= 0.1) < 0)
					mine_tracking_rate = 0;
			} // end else if

			// test if player is moving
			if (keyboard_state[DIK_RIGHT])
			{
				// rotate player to right
				if (++wraith.varsI[WRAITH_INDEX_PTR] > 15)
					wraith.varsI[WRAITH_INDEX_PTR] = 0;
			} // end if right
			else
			if (keyboard_state[DIK_LEFT])
			{
				// rotate player to left
				if (--wraith.varsI[WRAITH_INDEX_PTR] < 0)
					wraith.varsI[WRAITH_INDEX_PTR] = 15;
			} // end if

			// vertical speed motion
			if (keyboard_state[DIK_UP])
			{
				// move player forward
				xv = cos_look16[wraith.varsI[WRAITH_INDEX_PTR]];
				yv = sin_look16[wraith.varsI[WRAITH_INDEX_PTR]];

				// test to turn on engines
				if (!engines_on)
					DSound_Play(engines_id, DSBPLAY_LOOPING);

				// set engines to on
				engines_on = 1;

				Start_Particle(PARTICLE_TYPE_FADE, PARTICLE_COLOR_GREEN, 3,
					player_x+RAND_RANGE(-2,2), player_y+RAND_RANGE(-2,2), (-int(player_xv)*.125), (-int(player_yv)*.125));
			} // end if up
			else
			if (engines_on)
			{
				// reset the engine on flag and turn off sound
				engines_on = 0;

				// turn off the sound
				DSound_Stop_Sound(engines_id);
			} // end if

			// toggle huds
			if (keyboard_state[DIK_H] && !huds_debounce)
			{
				huds_debounce = 1;
				huds_on = (huds_on) ? 0 : 1;

				DSound_Play(beep1_id);
			} // end if

			if (!keyboard_state[DIK_H])
				huds_debounce = 0;

			// toggle scanner
			if (keyboard_state[DIK_S] && !scanner_debounce)
			{
				scanner_debounce = 1;
				scanner_on = (scanner_on) ? 0 : 1;

				DSound_Play(beep1_id);
			} // end if

			if (!keyboard_state[DIK_S])
				scanner_debounce = 0;

			// add velocity to change to player's velocity
			player_xv += xv;
			player_yv += yv;

			// test for maximum velocity
			vel = Fast_Distance_2D(player_xv, player_yv);

			if (vel >= MAX_PLAYER_SPEED)
			{
				// recompute velocity
				player_xv = (MAX_PLAYER_SPEED-1)*player_xv/vel;
				player_yv = (MAX_PLAYER_SPEED-1)*player_yv/vel;
			} // end if

			// move player 
			player_x += player_xv;
			player_y += player_yv;

			// wrap player to universe
			if (player_x > UNIVERSE_MAX_X)
				player_x = UNIVERSE_MIN_X;
			else
			if (player_x < UNIVERSE_MIN_X)
				player_x = UNIVERSE_MAX_X;

			// y coords
			if (player_y > UNIVERSE_MAX_Y)
				player_y = UNIVERSE_MIN_Y;
			else
			if (player_y < UNIVERSE_MIN_Y)
				player_y = UNIVERSE_MAX_Y;

			// test if player is fireing
			if (keyboard_state[DIK_LCONTROL] || keyboard_state[DIK_SPACE])
			{
				// compute plasma velocity vector
				float plasma_xv = cos_look16[wraith.varsI[WRAITH_INDEX_PTR]]*PLASMA_SPEED;
				float plasma_yv = sin_look16[wraith.varsI[WRAITH_INDEX_PTR]]*PLASMA_SPEED;

				// fire the plasma taking into cosideration ship's velocity
				Fire_Plasma(player_x, player_y, 
					player_xv+plasma_xv, player_yv+plasma_yv,
					PLASMA_ANIM_PLAYER);

			} // end if

			// regenarate player
			if (++player_regen_counter >= PLAYER_REGEN_COUNT)
			{
				// regenrate the player a bit
				if (player_damage > 0)
					player_damage--;

				// reset counter
				player_regen_counter = 0;

			} // end if

			// test for dying state transition
			if (player_damage >= 100 && player_state == PLAYER_STATE_ALIVE)
			{
				// set damage to 0
				player_damage = 100;

				// kill player
				player_state = PLAYER_STATE_DYING;
				player_ships--;

				// set counter to 0
				player_counter = 0;

				// turn engines off
				engines_on = 0;

				// start scream
				DSound_Play(scream_id);
			} // end if

		} // end if player _alive
		else
		if (player_state == PLAYER_STATE_DYING)
		{
			// player is dying

			// start random bursts
			int bwidth = 16 + rand()%64;

			if ((rand()%4) == 1)
			{
				Start_Burst(player_x+rand()%40,player_y+rand()%8,
				bwidth, (bwidth >> 2) + (bwidth >> 1),
				-4+rand()%8,2+rand()%4);
			} // end if

			// get jigge with it
			wraith.varsI[WRAITH_INDEX_PTR] += (RAND_RANGE(-1,1));

			if (wraith.varsI[WRAITH_INDEX_PTR] > 15)
				wraith.varsI[WRAITH_INDEX_PTR] = 0;
			else
			if (wraith.varsI[WRAITH_INDEX_PTR] < 15)
				wraith.varsI[WRAITH_INDEX_PTR] = 15;

			// update state counter
			if (++player_counter > 150)
			{
				// set state to invicible
				player_state = PLAYER_STATE_INVINCIBLE;

				// reset counter
				player_counter = 0;

				// reset damage
				player_damage = 0;
			} // end if

		} // end if
		else
		if (player_state == PLAYER_STATE_INVINCIBLE)
		{
			// player is invincible

			// flicker sheilds and play energizing sound

			// update state counter
			if (++player_counter > 70)
			{
				// set state to alive
				player_state = PLAYER_STATE_ALIVE;

				// reset counter
				player_counter = 0;

				// reset player position
				player_x  = 0;
				player_y  = 0;
				player_dx = 0;
				player_dy = 0;
				player_xv = 0;
				player_yv = 0;
				wraith.varsI[WRAITH_INDEX_PTR] = 0;

				// decrease player ships
				if (player_ships == 0)
				{
					// change state to dead, reset all vars
					player_state	= PLAYER_STATE_DEAD;
					player_counter	= 0;
					ready_state		= 1;
					ready_counter	= 0;

					// turn engines
					DSound_Stop_Sound(engines_id);

					// play game over sound
					DSound_Play(game_over_id);

				} // end if

			} // end if

			// start ready again
			if ((player_counter == 20) && (player_ships > 0))
			{
				// reset ready
				ready_counter	= 0;
				ready_state		= 0;
			} // end if 

		} // end if */

		// end player sub states

		// move the stations, mines, stars, particles and all
		Move_Stations();

		Move_Mines();
		
		Move_Rocks();
		
		Move_Stars();
		
		Move_Particles();
		
		Move_Bursts();

		// draw stars stations, mines, rocks and all
		Draw_Stars();

		Draw_Stations();

		Draw_Rocks();

		Draw_Mines();

		Draw_Particles();

		Draw_Plasma();

		// move the plasma
		Move_Plasma();

		// draw the player if alive
		if (player_state == PLAYER_STATE_ALIVE || player_state == PLAYER_STATE_DYING)
		{
			// set the current frame
			if (engines_on)
				wraith.curr_frame = wraith.varsI[WRAITH_INDEX_PTR] + 16*(rand()%2);
			else
				wraith.curr_frame = wraith.varsI[WRAITH_INDEX_PTR];

			// draw the bob
			Draw_BOB(&wraith, lpddsback);

			// draw the sheilds
			if (player_sheild_count > 0)
			{
				player_sheild_count--;

				// select sheild frame
				wraith.curr_frame = 34-player_sheild_count;

				// draw the bob
				Draw_BOB(&wraith, lpddsback);
			} // end if

		} // end if

		// draw the explosions last
		Draw_Bursts();

		// draw the score and ships left
		if (huds_on)
			Draw_Info();

		if (scanner_on)
			Draw_Scanner();

		// last state here so it will overlay
		if (player_state == PLAYER_STATE_DEAD)
		{
			// player is dead
			ready_state		= 1;
			ready_counter	= 0;

			// player is done!
			Draw_Text_GDI("G A M E   O V E R", (SCREEN_WIDTH/2)-8*(strlen("G A M E   O V E R")>>1), SCREEN_HEIGHT/2, RGB(0,255,0),
				lpddsback);

			if (keyboard_state[DIK_R])
				game_state = GAME_STATE_INIT;

		} // end if

		// draw get ready
		if (ready_state == 0)
		{
			// test if counter is 10 for voice
			if (ready_counter == 10)
				DSound_Play(ready_id);

			// draw text
			Draw_Text_GDI("E N T E R I N G   S E C T O R - ALPHA 11", 320-8*strlen("E N T E R I N G   S E C T O R - ALPHA 11")/2,
				200, RGB(0,rand()%256,0), lpddsback);

			// increament counter
			if (++ready_counter > 60)
			{
				// set state to ready
				ready_state		= 1;
				ready_counter	= 0;
			} // end if

		} // end if

		/*// test for me!
		if (keyboard_state[DIK_LALT] && keyboard_state[DIK_RALT] && keyboard_state[DIK_A])
		{
			// lock memory
			DDraw_Lock_Back_Surface();

			Draw_Bitmap(&redi, back_buffer, back_lpitch, 0);

			// release the buffer
			DDraw_Unlock_Back_Surface();
			redi_up = 1;
		} // end if
		else
		{ 
			redi_up = 0;
			Set_Palette(game_palette);
		} // end else

		if (redi_up == 1)
		{
			Set_Palette(redi_palette);
			redi_up = 2;
		} // end if*/

		// flip the surfaces
		DDraw_Flip();

		// sync to 30ish fps
		Wait_Clock(30);

		// check of user is tying to exit
		//if (++player_counter > 10)
		if (keyboard_state[DIK_ESCAPE])
		{
			// send game back
			game_state = GAME_STATE_EXIT;

			// stop all sounds
			DSound_Stop_Sound(engines_id);
		} // end if Esc

	} // end if game_state_running
	else
	if (game_state == GAME_STATE_PAUSED)
	{
		// pause game
		if (keyboard_state[DIK_P] && !pause_debounce)
		{
			pause_debounce = 1;
			game_paused = (game_paused) ? 0 : 1;

			DSound_Play(beep1_id);
		} // end if

		if (!keyboard_state[DIK_P])
			game_paused = 0;

		if (game_paused)
		{
			// draw text
			Draw_Text_GDI("G A M E  P A U S E D -- P R E S S <P>",
				320-8*strlen("G A M E  P A U S E D -- P R E S S <P>")/2, 200,
				RGB(255, 0, 0), lpddsback);
		} // end if
		else
			game_state = GAME_STATE_RUNNING;

	} // end if*/
	else
	if (game_state == GAME_STATE_EXIT)
	{
		// this is the exit state called just once
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
		game_state = GAME_STATE_WAITING_FOR_EXIT;
	} // end if
	else
	if (game_state == GAME_STATE_WAITING_FOR_EXIT)
	{
		// wait here in safe state

	} // end wait*/

	// return success

	return(1);

} // end game_main

//--------------------------------------------------|

int Game_Init(void *parms, int num_parms)
{
// Initializes game components and resources

	int index;				// looping var
	char filename[80];		// used to build up file names

	srand(Start_Clock());	// seed random number generator

	// create the look up table
	Create_Tables();

	// intialize direct draw
	DDraw_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);

	// initailze direct sound
	DSound_Init();

	// initalize direct input
	DInput_Init();

	// initalize the keyboard
	DInput_Init_Keyboard();

	// load all the sound and music
	Load_Sound_Music();

	// initalize the plasma pulses
	Init_Plasma();

	// Initalize all the stars
	Init_Stars();

	// init rocks
	Init_Rocks();
	Seed_Rocks();

	// init all the explosions
	Init_Bursts();

	// initalize all the mines
	Init_Mines();

	// initalize the stations must be after mines
	Init_Stations();

	// initalize the particle system
	Init_Reset_Particles();

	Load_Redi();

	// set the clipping rectangle
	RECT screen_rect = {0,0, screen_width,screen_height};
	lpddclipper = DDraw_Attach_Clipper(lpddsback,1,&screen_rect);

	// hide the mouse
	ShowCursor(FALSE);

	// start the intro music
	DSound_Play(intro_music_id, DSBPLAY_LOOPING);

	// do the introduction
	Do_Intro();

	// stop the intro sound
	DSound_Stop_Sound(intro_music_id);

	// load the player
	Load_Player();

	// load the HUD art
	Load_HUD();

	// success
	return(1);

} // end game_init

//--------------------------------------------------|

int Game_Shutdown(void *parms, int num_parms)
{
// shut's down game and releases resources

	// delete all the explosions
	Delete_Bursts();

	// delete the player
	Destroy_BOB(&wraith);

	// delete all the rocks
	Delete_Rocks();

	// delete all the plasma pulses
	Delete_Plasma();

	// delete all the stations
	Delete_Stations();

	// delete the mines
	Delete_Mines();

	// shut down ddraw
	DDraw_Shutdown();

	// unload sounds
	DSound_Delete_All_Sounds();

	// shutdown direct sound
	DSound_Shutdown();

	// release all input devices
	DInput_Release_Keyboard();

	// shut input down
	DInput_Shutdown();

	return(1);

} // end game_shutdown

//--------------------------------------------------|