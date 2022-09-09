//--------------------------------------------------|
// T3DLIB.H -- Game Engine Header					|
//--------------------------------------------------|
#ifndef T3DLIB_H
#define T3DLIB_H

//--------------------------------------------------|
// INCLUDES
//--------------------------------------------------|

//--------------------------------------------------|
// DEFINES
//--------------------------------------------------|

// default screen values, these are all overriden by the 
//	call to DDraw_Init() and are just here to have something
//	to set the globals to instead of constant values
#define SCREEN_WIDTH				640			// screen size
#define SCREEN_HEIGHT				480
#define SCREEN_BPP					8			// screen bits per pixel
#define	MAX_COLORS_PALETTE			256

// used for selecting full screen/windowed mode
#define SCREEN_FULLSCREEN			0
#define SCREEN_WINDOWED				1

// bitmap defines
#define BITMAP_ID					0x4D42		// universal bitmap id
#define BITMAP_STATE_DEAD			0
#define BITMAP_STATE_ALIVE			1
#define BITMAP_STATE_DYING			2
#define BITMAP_ATTR_LOADED			128

#define BITMAP_EXTRACT_MODE_CELL	0
#define BITMAP_EXTRACT_MODE_ABS		1

// directdraw pixel format defines, used to help
//	bitmap loader put data in proper format
#define DD_PIXEL_FORMAT8			8
#define DD_PIXEL_FORMAT555			15
#define DD_PIXEL_FORMAT565			16
#define DD_PIXEL_FORMAT888			24
#define DD_PIXEL_FORMATALPHA888		32

// defines for BOBs
#define BOB_STATE_DEAD				0			// this is a dead BOB
#define BOB_STATE_ALIVE				1			// BOB is alive
#define BOB_STATE_DYING				2			// this BOB is dying
#define BOB_STATE_ANIM_DONE			1			// done animation state
#define MAX_BOB_FRAMES				64			// maximum # of frames for BOB
#define MAX_BOB_ANIMATIONS			16			// maximum # of animations

#define BOB_ATTR_SINGLE_FRAME		1			// BOB has a single frame
#define BOB_ATTR_MULTI_FRAME		2			// BOB has multiple frames
#define BOB_ATTR_MULTI_ANIM			4			// BOB has multiple animations
#define BOB_ATTR_ANIM_ONE_SHOT		8			// BOB will perform the animation once
#define BOB_ATTR_VISIBLE			16			// BOB is visible
#define BOB_ATTR_BOUNCE				32			// BOB bounces of edges
#define BOB_ATTR_WRAPAROUND			64			// BOB wraps around edges
#define BOB_ATTR_LOADED				128			// BOB has been loaded
#define BOB_ATTR_CLONE				256			// BOB is a clone

// screen transition commands
#define SCREEN_DARKNESS				0			// fade to black
#define SCREEN_WHITENESS			1			// fade to white
#define SCREEN_SWIPE_X				2			// do a horizontal swipe
#define SCREEN_SWIPE_Y				3			// vertical swipe
#define SCREEN_DISSOLVE				4			// pixel dissolve
#define SCREEN_SCRUNCH				5			// a square comperssion
#define SCREEN_BLUENESS				6			// fade to blue
#define SCREEEN_REDNESS				7			// fade to red
#define SCREEN_GREENESS				8			// fade to green

// defines for blink colors
#define BLINKER_ADD					0			// add light to data base
#define BLINKER_DELETE				1			// remove light from database
#define BLINKER_UPDATE				2			// update light
#define BLINKER_RUN					3			// run normal

// pi defines
#define PI							((float)3.141592654f)
#define PI2							((float)6.283185307f)
#define PI_DIV_2					((float)1.570796327f)
#define PI_DIV_4					((float)0.785398163f)
#define PI_INV						((float)0.318309886f)

// fixed point mathematics constants
#define FIXP16_SHIFT				16
#define FIXP16_MAG					65536
#define FIXP16_DP_MASK				0x0000ffff
#define FIXP16_WP_MASK				0xffff0000
#define FIXP16_ROUND_UP				0x00008000

//--------------------------------------------------|
// MACROS
//--------------------------------------------------|

// these read the keyboard asynchronusly
#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// this bulids a 16-bit color in 5.5.5 mode, ignore the most significant bit
#define _RGB16BIT555(r,g,b)	((b & 31) + ((g & 31) << 5) + ((r & 31) << 10))

// this builds a 16-bit color in 5.6.5 mode, with 6 bits for green
#define _RGB16BIT565(r,g,b)	((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))

// this builds a 24-bit color value 8.8.8 format
#define _RGB24BIT(r,g,b)	((b) + (g << 8) + (r << 16))

// this builds a 32-bit color in A.8.8.8 mode
#define _RGB32BIT(a,r,g,b)	((b) + (g << 8) + (r << 16) + (a << 24))

// bit manuiplation functions
#define SET_BIT(word,bit_flag)	((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag)((word)=((word) & (~bit_flag)))

// Macro initalizes a direct draw struct, basically
//	zero's it out and sets the dwSize feild
#define DDRAW_INIT_STRUCT(ddstruct) {memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct);}

// used to comupte the min and max of two expressions
#define MIN(a,b)	((a < b) ? a : b)
#define MAX(a,b)	((a > b) ? a : b)

// used for swapping algorithim
#define SWAP(a,b,t)	{t=a; b=a; a=t;}

// some math macro
#define DEG_TO_RAD(ang)	((ang)*PI/180.0)
#define RAD_TO_DEG(rads)((rads)*180.0/PI)

#define RAND_RANGE(x,y)	((x) + (rand()%((y)-(x)+1)))

//--------------------------------------------------|
// TYPES
//--------------------------------------------------|

// basic unsigned types
typedef unsigned int	QUAD;
typedef unsigned int	UINT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

// contianer structure for bitmaps ".BMP" file
typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER	bitmapfileheader;		// ".BMP" file header
	BITMAPINFOHEADER	bitmapinfoheader;		// contians all info on the bitmap
	PALETTEENTRY		palette[256];			// store's the palette data here, for 8-bit
												//	bitmaps and below
	UCHAR				*buffer;				// store's the actual pixel data of
												//	the bitmap
} BITMAP_FILE, *BITMAP_FILE_PTR;

// the blitter object structure BOB
typedef struct BOB_TYP
{
	int state;					// state of BOB (general)
	int anim_state;				// an animation variable
	int attr;					// attributes pretainig to the BOB
	int x,y;					// position bitmap will be displayed at
	int xv,yv;					// velocity of BOB
	int width,height;			// the width and height of BOB
	int width_fill;				// internal, used to force 8*x wide surfaces
	int counter_1;				// general counters
	int counter_2;
	int max_count_1;			// general threshold values
	int max_count_2;
	int varsI[16];				// stack of 16 integers
	int varsF[16];				// stack of 16 floating points
	int curr_frame;				// current frame of animation
	int num_frames;				// total number of animation frames
	int curr_animation;			// index of current animation
	int anim_counter;			// used to time animation transitions
	int anim_index;				// animation element index
	int anim_count_max;			// number of cycles befor animation
	int bpp;					// bits per pixel
	int *animation[MAX_BOB_ANIMATIONS];	// animation sequences

	LPDIRECTDRAWSURFACE4 image[MAX_BOB_FRAMES];	// dd-surfaces, hold 
												// bitmaps
} BOB, *BOB_PTR;

// the simple bitmap image
typedef struct BITMAP_IMAGE_TYP
{
	int state;					// state of bitmap
	int attr;					// attributes of bitmap
	int x,y;					// position of bitmap
	int width,height;			// size of bitmap
	int bpp;					// bitmap pixels per byte
	int num_bytes;				// total bytes of bitmap
	UCHAR *buffer;				// pixels of bitmap

} BITMAP_IMAGE, *BITMAP_IMAGE_PTR;

// blinking light structure
typedef struct BLINKER_TYP
{
	int color_index;			// index of color to blink
	PALETTEENTRY on_color;		// RGB value of "on" color
	PALETTEENTRY off_color;		// RGB value of "off" color
	int on_time;				// number of frames to keep "on"
	int off_time;				// number of frames tp keep "off"

	// internal member
	int counter;				// counter for state transitions
	int state;					// state of light, -1 off, 1 on, 0 dead

} BLINKER, *BLINKER_PTR;

// a 2D vertex with integer points
typedef struct VERTEX2DI_TYP
{
	int x,y;					// the vertices
} VERTEX2DI, *VERTEX2DI_PTR;

// a 2D vertex with floating point
typedef struct VERTEX2DF_TYP
{
	float x,y;					// the vertices
} VERTEX2DF, *VERTEX2DF_PTR;

// a 2D polygon
typedef struct POLYGON2D_TYP
{
	int state;					// state of the polyon
	int num_verts;				// number of vertices
	int x0,y0;					// position center of polygon
	int xv,yv;					// velocity of polygon
	DWORD color;				// could be index or PLAETTEENTRY
	VERTEX2DF *vlist;			// pointer to the vertex list

} POLYGON2D, *POLYGON2D_PTR;

// matrix defintions
typedef struct MATRIX3X3_TYP
{
	float M[3][3];				// data storage
} MATRIX3X3, *MATRIX3X3_PTR;

typedef struct MATRIX1X3_TYP
{
	float M[3];
} MATRIX1X3, *MATRIX1X3_PTR;

typedef struct MATRIX3X2_TYP
{
	float M[3][2];
} MATRIX3X2, *MATRIX3X2_PTR;

typedef struct MATRIX1X2_TYP
{
	float M[2];
} MATRIX1X2, *MATRIX1X2_PTR;

//--------------------------------------------------|
// PRTOTYPES
//--------------------------------------------------|

// The DiectDraw Interface /////////////////////////
int DDraw_Init(int width, int height, int bpp);
int DDraw_Shutdown(void);
LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE4 lpdds,int num_rects, LPRECT clip_list);
LPDIRECTDRAWSURFACE4 DDraw_Create_Surface(int width, int height, int mem_flags=0, int color_key_value=0);

int DDraw_Flip(void);
int DDraw_Wait_For_Vsync(void);
int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE4 lpdds, int color, RECT *client=NULL);
UCHAR *DDraw_Lock_Surface(LPDIRECTDRAWSURFACE4 lpdds, int *lpitch);
int DDraw_Unlock_Surface(LPDIRECTDRAWSURFACE4 lpdds);
UCHAR *DDraw_Lock_Back_Surface(void);
UCHAR *DDraw_Lock_Primary_Surface(void);
int DDraw_Unlock_Primary_Surface(void);
int DDraw_Unlock_Back_Surface(void);

// BOB functions /////////////////////////////////////
int Create_BOB(BOB_PTR bob, int x, int y, int width, int height, int num_frames, int attr,
			   int mem_flags=0, int color_key_value=0, int bpp=8);
int Clone_BOB(BOB_PTR source, BOB_PTR dest);
int Destroy_BOB(BOB_PTR bob);
int Draw_BOB(BOB_PTR bob, LPDIRECTDRAWSURFACE4 dest);
int Draw_Scaled_BOB(BOB_PTR bob, int swidth, int sheight, LPDIRECTDRAWSURFACE4 dest);
int Draw_BOB16(BOB_PTR bob, LPDIRECTDRAWSURFACE4 dest);
int Draw_Scaled_BOB16(BOB_PTR bob, int swidth, int sheight, LPDIRECTDRAWSURFACE4 dest);

int Load_Frame_BOB(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy, int mode);
int Load_Frame_BOB16(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame,int cx,int cy, int mode);
int Animate_BOB(BOB_PTR bob);
int Move_BOB(BOB_PTR bob);
int Load_Animation_BOB(BOB_PTR bob, int anim_index, int num_frames, int *sequence);
int Set_Pos_BOB(BOB_PTR bob, int x, int y);
int Set_Vel_BOB(BOB_PTR bob, int xv, int yv);
int Set_Anim_Speed_BOB(BOB_PTR bob, int speed);
int Set_Animation_BOB(BOB_PTR bob, int anim_index);
int Hide_BOB(BOB_PTR bob);
int Show_BOB(BOB_PTR bob);
int Collision_BOBS(BOB_PTR bob1, BOB_PTR bob2);

// General utlity functions //////////////////////////
DWORD Start_Clock(void);
DWORD Get_Clock(void);
DWORD Wait_Clock(DWORD count);

int Collision_Test(int x1, int y1, int w1, int h1,
				   int x2, int y2, int w2, int h2);

int Color_Scan(int x1, int y1, int x2, int y2,
			   UCHAR scan_start, UCHAR scan_end,
			   UCHAR *scan_buffer, int scan_lpitch);

int Color_Scan16(int x1, int y1, int x2, int y2,
				 USHORT scan_start, USHORT scan_end,
				 UCHAR *scan_buffer, int scan_lpitch);

// Graphics functions ////////////////////////////////
int Draw_Clip_Line(int x0, int y0, int x1, int y1, int color, UCHAR *dest_buffer, int lpitch);
int Draw_Clip_Line16(int x0,int y0,int x1, int y1, int color, UCHAR *dest_buffer, int lpitch);
int Clip_Line(int &x1, int &y1, int &x2, int &y2);
int Draw_Line(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);
int Draw_Line16(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);
int Draw_Pixel(int x, int y, int color, UCHAR *video_buffer, int lpitch);
int Draw_Rectangle(int x1, int y1, int x2, int y2, int color, LPDIRECTDRAWSURFACE4 lpdds);

void HLine(int x1, int x2, int y, int color, UCHAR *vbuffer, int lpitch);
void VLine(int y1, int y2, int x, int color, UCHAR *vbuffer, int lpitch);
void Screen_Transitions(int effect, UCHAR *vbuffer, int lpitch);

// Palette functions /////////////////////////////////
int Set_Palette(LPPALETTEENTRY set_palette);
int Set_Palette_Entry(int color_index, LPPALETTEENTRY color);
int Get_Palette_ENtry(int color_index, LPPALETTEENTRY color);
int Rotate_Colors(int start_index, int end_index);
int Blink_Colors(int command, BLINKER_PTR new_light, int id);

// Simple bitmap image functions /////////////////////
int Create_Bitmap(BITMAP_IMAGE_PTR image, int x, int y, int width, int height, int bpp=8);
int Destroy_Bitmap(BITMAP_IMAGE_PTR image);
int Draw_Bitmap(BITMAP_IMAGE_PTR source_bitmap, UCHAR *dest_buffer, int lpitch, int transparent);
int Load_Image_Bitmap(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx, int cy, int mode);
int Scroll_Bitmap(BITMAP_IMAGE_PTR image, int dx, int dy=0);

int Copy_Bitmap(BITMAP_IMAGE_PTR dest_bitmap, int dest_x, int dest_y,
				BITMAP_IMAGE_PTR source_bitmap, int source_x, int source_y,
				int width, int height);

int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height);

// Bitmap file functions /////////////////////////////
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename);
int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap);

// GDI functions /////////////////////////////////////
int Draw_Text_GDI(char *text, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE4 lpdds);
int Draw_Text_GDI(char *text, int x, int y, int color, LPDIRECTDRAWSURFACE4 lpdds);

// 2D Polygon Functions //////////////////////////////
void Draw_Filled_Polygon2D(POLYGON2D_PTR poly, UCHAR *vbuffer, int mempitch);
int Translate_Polygon2D(POLYGON2D_PTR poly, int dx, int dy);
int Rotate_Polygon2D(POLYGON2D_PTR poly, int thetha);
int Scale_Polygon2D(POLYGON2D_PTR poly, float sx, float sy);
void Build_Sin_Cos_Tables(void);
int Draw_Polygon2D(POLYGON2D_PTR poly, UCHAR *vbuffer, int lpitch);

// Math functions ///////////////////////////////////
int Fast_Distance_2D(int x, int y);
float Fast_Distance_3D(float x, float y, float z);

int Find_Bounding_Box_Poly2D(POLYGON2D_PTR poly,
							 float &min_x, float &max_x,
							 float &min_y, float &max_y);

// Memory manuiplation functions ////////////////////
inline void Mem_Set_WORD(void *dest, USHORT data, int count);
inline void Mem_Set_QUAD(void *dest, UINT   data, int count);

//--------------------------------------------------|
// EXTERNS
//--------------------------------------------------|

// directdraw externs, using interface 4.0 
extern LPDIRECTDRAW4		lpdd;					// dd-object
extern LPDIRECTDRAWSURFACE4	lpddsprimary;			// dd primary surface
extern LPDIRECTDRAWSURFACE4 lpddsback;				// dd back surface
extern LPDIRECTDRAWPALETTE	lpddpal;				// pointer to the newly created palette
extern LPDIRECTDRAWCLIPPER	lpddclipper;			// dd clipper
extern PALETTEENTRY			palette[256];			// color palette
extern PALETTEENTRY			save_palette[256];		// used to save palettes
extern DDSURFACEDESC2		ddsd;					// dd surface description struct
extern DDBLTFX				ddbltfx;				// used to fill
extern HRESULT				ddrval;					// result back from dd calls
extern UCHAR				*primary_buffer;		// primary video buffer
extern UCHAR				*back_buffer;			// secondary back buffer
extern int					primary_lpitch;			// memory line pitch of primary surface
extern int					back_lpitch;			// memory line pitch of back surface
extern BITMAP_FILE			bitmap8bit;				// a 8-bit bimap data
extern BITMAP_FILE			bitmap16bit;			// a 16-bit bitmap file
extern BITMAP_FILE			bitmap24bit;			// a 24-bit bitmap file

extern DWORD				start_clock_count;		// used for timming
extern int					windowed_mode;			// tracks if dd is windowed or fullscreen

// these defined the general clipping rectangle for sofware clipping
extern int					min_clip_x,				// clipping region
							max_clip_x,
							min_clip_y,
							max_clip_y;

// these are overwritten globally by DD_Init()
extern int					screen_width,			// width of the screen
							screen_height,			// height of screen
							screen_bpp;				// bits per pixel

extern int					dd_pixel_format;		// default pixel format

extern int					widow_client_x0;		// usedd to track the starting (x,y) client atrea
extern int					window_client_y0;		// foe windowed mode dd operations

// storage for lookup tables
extern float				cos_look[360];
extern float				sin_look[360];

#endif