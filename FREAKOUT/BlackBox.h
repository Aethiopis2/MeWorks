// BLACKBOX.H - Header file for demo game engine library

// Watch for multiple inclusions
#ifndef BLACKBOX
#define BLACKBOX

// DEFINES ///////////////////////////////////////////////

// Default screen size
#define SCREEN_WIDTH	640		// Screen size
#define SCREEN_HEIGHT	480
#define SCREEN_BPP		8		// bits per pixel
#define MAX_COLORS		256		// maximum colors

// MACROS ////////////////////////////////////////////////

// these read the keybord asyncrounously
#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(key_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// intializes a direct draw struct
#define DD_INIT_STRUCT(ddstruct)	{ memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

// TYPES /////////////////////////////////////////////////

// basic unsigned types
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char UCHAR;
typedef unsigned char BYTE;

// EXTERNAlS ////////////////////////////////////////////

extern LPDIRECTDRAW7		lpdd;			// dd object
extern LPDIRECTDRAWSURFACE7	lpddsprimary;	// dd primary surface
extern LPDIRECTDRAWSURFACE7 lpddsback;		// dd back surface
extern LPDIRECTDRAWPALETTE	lpddpal;		// a pointer to a palette
extern LPDIRECTDRAWCLIPPER	lpddclipper;	// dd clipper
extern PALETTEENTRY		palette[256];		// color palette
extern PALETTEENTRY		save_palette[256];	// used to save palette's
extern DDSURFACEDESC2	ddsd;				// surface discription sturct
extern DDBLTFX			ddbltfx;			// used to fill
extern DDSCAPS2			ddscaps;			// dd surface capablities struct
extern HRESULT			ddrval;				// result back from dd calls
extern DWORD			start_clock_count;	// used for timming

// these defined the general clipping rectangle
extern int	min_clip_x,			// clipping rectangle
			max_clip_x,
			min_clip_y,
			max_clip_y;

// these are overwritten globally by the DD_INIT()
extern int	screen_width,	// width of screen
			screen_height,	// height of screen
			screen_bpp;		// bits per pixel

// PROTOTYPES ////////////////////////////////////////////////////////

// Direct draw functions
int DD_Init(int width, int height, int bpp);
int DD_Shutdown(void);
LPDIRECTDRAWCLIPPER DD_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,
									  int num_rects, LPRECT clip_list);
int DD_Flip(void);
int DD_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, int color);

// general utility functions
DWORD Start_Clock(void);
DWORD Get_Clock(void);
DWORD Wait_Clock(DWORD count);

// graphics functions
int Draw_Rectangle(int x1, int y1,
				   int x2, int y2,
				   int color,
				   LPDIRECTDRAWSURFACE7 lpdds=lpddsback);

// gdi functions
//int Draw_Text_GDI(char *text, int x, int y, COLORREF color,
//				  LPDIRECTDRAWSURFACE7 lpdds=lpddsback);
int Draw_Text_GDI(char *text, int x, int y, int color,
				  LPDIRECTDRAWSURFACE7 lpdds=lpddsback);

#endif