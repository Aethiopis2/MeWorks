//--------------------------------------------------|
// T3DLIB.CPP -- Game Engine Defintion				|
//--------------------------------------------------|
#define WIN32_LEAN_AND_MEAN			// just say no to MFC
#define INITGUID					// initalize all GUIDs

//--------------------------------------------------|
// INCLUDES
//--------------------------------------------------|

#include <windows.h>				// Windows OS headers
#include <windowsx.h>				
#include <MMSystem.h>

#include <stdio.h>					// standard C/C++ headers
#include <stdlib.h>
#include <io.h>
#include <math.h>

#include <ddraw.h>					// directX directdraw header

#include "T3DLIB.H"

//--------------------------------------------------|
// DEFINES
//--------------------------------------------------|

//--------------------------------------------------|
// MACROS
//--------------------------------------------------|

//--------------------------------------------------|
// EXTERNALS
//--------------------------------------------------|

// Windows externs
extern HINSTANCE		main_instance;
extern HWND				main_window_handle;

//--------------------------------------------------|
// GLOBALS
//--------------------------------------------------|

// directdraw externs, using interface 4.0 
LPDIRECTDRAW4			lpdd		= NULL;	// dd-object
LPDIRECTDRAWSURFACE4	lpddsprimary= NULL;	// dd primary surface
LPDIRECTDRAWSURFACE4	lpddsback	= NULL;	// dd back surface
LPDIRECTDRAWPALETTE		lpddpal		= NULL;	// pointer to the newly created palette
LPDIRECTDRAWCLIPPER		lpddclipper	= NULL;	// dd clipper
PALETTEENTRY			palette[256];		// color palette
PALETTEENTRY			save_palette[256];	// used to save palettes
DDSURFACEDESC2			ddsd;					// dd surface description struct
DDBLTFX					ddbltfx;				// used to fill
HRESULT					ddrval;					// result back from dd calls
UCHAR					*primary_buffer = NULL;	// primary video buffer
UCHAR					*back_buffer = NULL;	// secondary back buffer
int						primary_lpitch=0;		// memory line pitch of primary surface
int						back_lpitch=0;			// memory line pitch of back surface
BITMAP_FILE				bitmap8bit;				// a 8-bit bimap data
BITMAP_FILE				bitmap16bit;			// a 16-bit bitmap file
BITMAP_FILE				bitmap24bit;			// a 24-bit bitmap file

DWORD					start_clock_count=0;	// used for timming
int						windowed_mode=FALSE;		// tracks if dd is windowed or fullscreen

// these defined the general clipping rectangle for sofware clipping
int						min_clip_x = 0,				// clipping region
						max_clip_x = screen_width-1,
						min_clip_y = 0,
						max_clip_y = screen_height-1;

// these are overwritten globally by DD_Init()
int						screen_width=SCREEN_WIDTH,	// width of the screen
						screen_height=SCREEN_HEIGHT,// height of screen
						screen_bpp=SCREEN_BPP;		// bits per pixel

int						dd_pixel_format = DD_PIXEL_FORMAT565;		// default pixel format

int						widow_client_x0;		// usedd to track the starting (x,y) client atrea
int						window_client_y0;		// foe windowed mode dd operations

// storage for lookup tables
float					cos_look[360];
float					sin_look[360];

//--------------------------------------------------|
// FUNCTIONS
//--------------------------------------------------|

// BOB functions ////////////////////////////////////

int Create_BOB(BOB_PTR bob,				// the bob to create
			   int x, int y,			// initial position
			   int width, int height,	// size of BOB
			   int num_frames,			// number of frames
			   int attr,				// attributes
			   int mem_flags,			// memory flags in DD foormat
			   int color_key_value,		// default color key
			   int bpp)					// bits per pixel
{
// Create the BOB object, note that all BOBs
//	are created as offscreen surfaces in VRAM as the 
//	default, if you want to use system memory
//	then set flags equal to:
//	DDSCAPS_SYSTEMMEORY
//	for video memory you can create either local VRAM surfaces or AGP
//	surfaces via the second set of constants shown below in the regular expression
//	DDSCAPS_VIDEOMEMORY | (DDSCAPS_NONLOCALVIDMEM | DDSCAPS_LOCALVIDMEM)

	int index;		// looping var

	// set the state and attributes of BOB
	bob->state			= BOB_STATE_ALIVE;
	bob->attr			= attr;
	bob->anim_state		= 0;
	bob->counter_1		= 0;
	bob->counter_2		= 0;
	bob->max_count_1	= 0;
	bob->max_count_2	= 0;

	bob->curr_frame		= 0;
	bob->num_frames		= num_frames;
	bob->bpp			= bpp;
	bob->curr_animation	= 0;
	bob->anim_index		= 0;
	bob->anim_count_max	= 0;
	bob->xv				= 0;
	bob->yv				= 0;
	bob->x				= x;
	bob->y				= y;

	// set dimensions of the new bitmap surface
	bob->width			= width;
	bob->height			= height;
	bob->width_fill		= ((width%8 != 0) ? (width%8) : 0);

	// set all images to null
	for (index=0; index < MAX_BOB_FRAMES; index++)
		bob->image[index] = NULL;

	// set all animations to null
	for (index=0; index < MAX_BOB_ANIMATIONS; index++)
		bob->animation[index] = NULL;

	// now create each surface
	for (index=0; index < bob->num_frames; index++)
	{
		// clear ddsd
		DDRAW_INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

		ddsd.dwWidth	= bob->width + bob->width_fill;
		ddsd.dwHeight	= bob->height;

		// set surface to offscreen plain
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

		// create the surfaces
		if (FAILED(lpdd->CreateSurface(&ddsd, &(bob->image[index]), NULL)))
			return(0);

		// set color key to default color
		DDCOLORKEY color_key;
		color_key.dwColorSpaceHighValue	= 0;
		color_key.dwColorSpaceLowValue	= 0;

		// now set the color key for source blitting
		(bob->image[index])->SetColorKey(DDCKEY_SRCBLT, &color_key);

	} // end for index

	// return success
	return(1);

} // end Create _BOB

//--------------------------------------------------|

int Clone_BOB(BOB_PTR source, BOB_PTR dest)
{
// this function clones a BOB and updates the attr var to reflect that
//	the BOB is a clone and not real, this is used later in the destory
//	function so a clone does'nt destroy the memory of a real BOB

	if ((source && dest) && (source != dest))
	{
		// copy the bob data
		memcpy(dest,source,sizeof(BOB));

		// set the clone attr
		dest->attr |= BOB_ATTR_CLONE;

	} // end if
	else
		return(0);

	// success
	return(1);

} // end Clone_BOB

//--------------------------------------------------|

int Destroy_BOB(BOB_PTR bob)
{
// destroy the BOB, test's if this is a real bob or a clone
//	if real then releases all the memory, else it set's the
//	pointer to null

	int index;		// looping var

	// is this bob valid?
	if (!bob)
		return(0);

	// test if this is a clone
	if (bob->attr && BOB_ATTR_CLONE)
	{
		// NULL link all surfaces
		for (index=0; index < MAX_BOB_FRAMES; index++)
		{
			if (bob->image[index])
				bob->image[index] = NULL;
		} // end for index

		// release memory for animation sequences
		for (index=0; index < MAX_BOB_ANIMATIONS; index++)
		{
			if (bob->animation[index])
				bob->animation[index] = NULL;
		} // end for

	} // end if
	else
	{
		// destroy each bitmap surface
		for (index=0; index < MAX_BOB_FRAMES; index++)
		{
			if (bob->image[index])
				bob->image[index]->Release();
		} // end for

		// release memory for animation sequeces
		for (index=0; index < MAX_BOB_ANIMATIONS; index++)
		{
			if (bob->animation[index])
				free(bob->animation[index]);
		} // end for
	} // end else not clone

	// success
	return(1);

} // end Destroy_BOB

//--------------------------------------------------|

int Draw_BOB(BOB_PTR bob,				// the bob to draw
			 LPDIRECTDRAWSURFACE4 dest) // surface to draw bob on
{
// Draw a BOB at x,y defined in the BOB
//	on the destination surface defined on dest

	RECT dest_rect,		// the destination rectangle
		source_rect;	// the source rectangle

	// is this a valid BOB
	if (!bob)
		return(0);

	// is BOB visible
	if (!(bob->attr & BOB_ATTR_VISIBLE))
		return(0);

	// fill in the destination rect
	dest_rect.left	= bob->x;
	dest_rect.top	= bob->y;
	dest_rect.right	= bob->x + bob->width;
	dest_rect.bottom= bob->y + bob->height;

	// fill in the source
	source_rect.left	= 0;
	source_rect.top		= 0;
	source_rect.right	= bob->width;
	source_rect.bottom	= bob->height;

	// blt to destination surface
	if (FAILED(dest->Blt(&dest_rect, bob->image[bob->curr_frame],
		&source_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL)))
		return(0);

	// return success
	return(1);

} // end Draw_BOB

//--------------------------------------------------|

int Draw_Scaled_BOB(BOB_PTR bob, int swidth, int sheight,	// BOB new width and dimensions
					LPDIRECTDRAWSURFACE4 dest)				// surface to draw bob on
{
// draw a scaled bob at x,y using swidth and sheight
//	as dimensions on dest surface

	RECT dest_rect,   // the destination rectangle
     source_rect; // the source rectangle                             

	// is this a valid bob
	if (!bob)
		return(0);

	// is bob visible
	if (!(bob->attr & BOB_ATTR_VISIBLE))
		return(1);

	// fill in the destination rect
	dest_rect.left   = bob->x;
	dest_rect.top    = bob->y;
	dest_rect.right  = bob->x+swidth;
	dest_rect.bottom = bob->y+sheight;

	// fill in the source rect
	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = bob->width;
	source_rect.bottom  = bob->height;

	// blt to destination surface
	if (FAILED(dest->Blt(&dest_rect, bob->image[bob->curr_frame],
			&source_rect,(DDBLT_WAIT | DDBLT_KEYSRC),
			NULL)))
		return(0);

	// return success
	return(1);

} // end Draw_Scaled_BOB

//--------------------------------------------------|

int Load_Frame_BOB(BOB_PTR bob,				// bob to load the data
				   BITMAP_FILE_PTR bitmap,	// bitmap to scan the image from
				   int frame,				// frame to load
				   int cx, int cy,			// cell to scan image from
				   int mode)				// if mode = 0 then cx,cy are cell pos
											//	else they are relative coords
{
// this function extracts a bitmap out of a bitmap file

	// test for bob validity
	if (!bob)
		return(0);

	UCHAR *source_ptr,		// working pointers
		*dest_ptr;

	// test the mode of extraction, cell based or absolute
	if (mode == BITMAP_EXTRACT_MODE_CELL)
	{
		// re-compute x,y
		cx = cx * (bob->width+1)+1;
		cy = cy * (bob->height+1)+1;
	} // end if

	// extract bitmap data
	source_ptr = bitmap->buffer + cy*bitmap->bitmapinfoheader.biWidth+cx;

	// get the addr the dest working memory

	// clear surface desc struct
	DDRAW_INIT_STRUCT(ddsd);

	// lock the display surface
	(bob->image[frame])->Lock(NULL, 
		&ddsd,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
		NULL);

	// assign pointer to memory
	dest_ptr = (UCHAR *)ddsd.lpSurface;

	// iterate thru each scan-line and copy bitmap
	for (int index_y=0; index_y < bob->height; index_y++)
	{
		memcpy(dest_ptr, source_ptr, bob->width);

		// advance pointers
		source_ptr	+= bitmap->bitmapinfoheader.biWidth;
		dest_ptr	+= ddsd.lPitch;

	} // end for index_y

	// unlock surface
	(bob->image[frame])->Unlock(NULL);

	// set state to loaded
	bob->state |= BOB_ATTR_LOADED;

	// return success
	return(1);

} // end Load_Frame_BOB

//--------------------------------------------------|

int Animate_BOB(BOB_PTR bob)
{
// this function animates a bob, basically it looks at
//	the attributes of the bob and determines if the bob is
//	single frame, multiframe, or multi animation, updates
//	the counters frames appropriatly

	// is this valid
	if (!bob)
		return(0);

	// test the level of animation
	if (bob->attr & BOB_ATTR_SINGLE_FRAME)
	{
		// current frame always 0
		bob->curr_frame = 0;
		return(1);

	} // end if
	else
	if ( bob->attr & BOB_ATTR_MULTI_FRAME)
	{
		// update counter and test if it is time to increment frame
		if (++bob->anim_counter >= bob->anim_count_max)
		{
			// reset counter
			bob->anim_counter = 0;

			// move to the next frame
			if (++bob->curr_frame >= bob->num_frames)
				bob->curr_frame = 0;

		} // end if 

	} // end else if
	else
	if (bob->attr & BOB_ATTR_MULTI_ANIM)
	{
		// look up the next sequcence

		// test if its time to animate
		if (++bob->anim_counter >= bob->anim_count_max)
		{
			// reset counter
			bob->anim_counter = 0;

			// increment the animation frame index
			bob->anim_index++;

			// extract the next frame from the animation list
			bob->curr_frame = bob->animation[bob->curr_animation][bob->anim_index];

			// is this and end sequence flag -1
			if (bob->curr_frame == -1)
			{
				// test if this is a single shot animation
				if (bob->attr & BOB_ATTR_ANIM_ONE_SHOT)
				{
					// set animation state message to done
					bob->anim_state = BOB_STATE_ANIM_DONE;
            
					// reset frame back one
					bob->anim_index--;

					// extract animation frame
					bob->curr_frame = bob->animation[bob->curr_animation][bob->anim_index];    

				} // end if
				else
				{
					// reset animation index
					bob->anim_index = 0;

					// extract first animation frame
					bob->curr_frame = bob->animation[bob->curr_animation][bob->anim_index];
				} // end else

			}  // end if
      
      } // end if

   } // end elseif

	// return success
	return(1);

} // end Amimate_BOB

//--------------------------------------------------|

int Scroll_BOB(void)
{
// this function scrolls a bob 
// not implemented

	// return success
	return(1);

} // end Scroll_BOB

//--------------------------------------------------|

int Move_BOB(BOB_PTR bob)
{
// this function moves the bob based on its current velocity
// also, the function test for various motion attributes of the'
// bob and takes the appropriate actions
   

	// is this a valid bob
	if (!bob)
		return(0);

	// translate the bob
	bob->x+=bob->xv;
	bob->y+=bob->yv;

	// test for wrap around
	if (bob->attr & BOB_ATTR_WRAPAROUND)
	{
		// test x extents first
		if (bob->x > max_clip_x)
			bob->x = min_clip_x - bob->width;
		else
		if (bob->x < min_clip_x-bob->width)
			bob->x = max_clip_x;
   
		// now y extents
		if (bob->x > max_clip_x)
			bob->x = min_clip_x - bob->width;
		else
		if (bob->x < min_clip_x-bob->width)
			bob->x = max_clip_x;

	} // end if
	else
	// test for bounce
	if (bob->attr & BOB_ATTR_BOUNCE)
	{
		// test x extents first
		if ((bob->x > max_clip_x - bob->width) || (bob->x < min_clip_x) )
			bob->xv = -bob->xv;
    
		// now y extents 
		if ((bob->y > max_clip_y - bob->height) || (bob->y < min_clip_y) )
			bob->yv = -bob->yv;

	} // end if

	// return success
	return(1);

} // end Move_BOB

//--------------------------------------------------|

int Load_Animation_BOB(BOB_PTR bob, 
                       int anim_index, 
                       int num_frames, 
                       int *sequence)
{
// this function load an animation sequence for a bob
// the sequence consists of frame indices, the function
// will append a -1 to the end of the list so the display
// software knows when to restart the animation sequence

	// is this bob valid
	if (!bob)
		return(0);

	// allocate memory for bob animation
	if (!(bob->animation[anim_index] = (int *)malloc((num_frames+1)*sizeof(int))))
		return(0);

	// load data into 
	for (int index=0; index<num_frames; index++)
		bob->animation[anim_index][index] = sequence[index];

	// set the end of the list to a -1
	bob->animation[anim_index][index] = -1;

	// return success
	return(1);

} // end Load_Animation_BOB

//--------------------------------------------------|

int Set_Pos_BOB(BOB_PTR bob, int x, int y)
{
// this functions sets the postion of a bob

	// is this a valid bob
	if (!bob)
		return(0);

	// set positin
	bob->x = x;
	bob->y = y;

	// return success
	return(1);

} // end Set_Pos_BOB

//--------------------------------------------------|

int Set_Anim_Speed_BOB(BOB_PTR bob,int speed)
{
// this function simply sets the animation speed of a bob
    
	// is this a valid bob
	if (!bob)
		return(0);

	// set speed
	bob->anim_count_max = speed;

	// return success
	return(1);

} // end Set_Anim_Speed

//--------------------------------------------------|

int Set_Animation_BOB(BOB_PTR bob, int anim_index)
{
// this function sets the animation to play

	// is this a valid bob
	if (!bob)
		return(0);

	// set the animation index
	bob->curr_animation = anim_index;

	// reset animation 
	bob->anim_index = 0;

	// return success
	return(1);

} // end Set_Animation_BOB

//--------------------------------------------------|

int Set_Vel_BOB(BOB_PTR bob,int xv, int yv)
{
// this function sets the velocity of a bob

	// is this a valid bob
	if (!bob)
		return(0);

	// set velocity
	bob->xv = xv;
	bob->yv = yv;

	// return success
	return(1);

} // end Set_Vel_BOB

//--------------------------------------------------|

int Hide_BOB(BOB_PTR bob)
{
// this functions hides bob 

	// is this a valid bob
	if (!bob)
		return(0);

	// reset the visibility bit
	RESET_BIT(bob->attr, BOB_ATTR_VISIBLE);

	// return success
	return(1);

} // end Hide_BOB

//--------------------------------------------------|

int Show_BOB(BOB_PTR bob)
{
// this function shows a bob

	// is this a valid bob
	if (!bob)
		return(0);

	// set the visibility bit
	SET_BIT(bob->attr, BOB_ATTR_VISIBLE);

	// return success
	return(1);

} // end Show_BOB

//--------------------------------------------------|

int Collision_BOBS(BOB_PTR bob1, BOB_PTR bob2)
{
	// are these a valid bobs
	if (!bob1 || !bob2)
		return(0);

	// get the radi of each rect
	int width1  = (bob1->width>>1) - (bob1->width>>3);
	int height1 = (bob1->height>>1) - (bob1->height>>3);

	int width2  = (bob2->width>>1) - (bob2->width>>3);
	int height2 = (bob2->height>>1) - (bob2->height>>3);

	// compute center of each rect
	int cx1 = bob1->x + width1;
	int cy1 = bob1->y + height1;

	int cx2 = bob2->x + width2;
	int cy2 = bob2->y + height2;

	// compute deltas
	int dx = abs(cx2 - cx1);
	int dy = abs(cy2 - cy1);

	// test if rects overlap
	if (dx < (width1+width2) && dy < (height1+height2))
		return(1);
	else	// else no collision
		return(0);

} // end Collision_BOBS

//--------------------------------------------------|

// DirectDraw Interface functions ///////////////////

int DDraw_Init(int width, int height, int bpp)
{
// this function initailizes directdraw objects

	LPDIRECTDRAW lpddtemp = NULL;		// generic IDirectDraw

	// set the globals of screen size
	screen_width	= width;
	screen_height	= height;
	screen_bpp		= bpp;

	// now create the generic interface
	if (FAILED(DirectDrawCreate(NULL, &lpddtemp, NULL)))
		return(0);

	// using the newly generic pointer, query for IDirectDraw 4.0
	if (FAILED(lpddtemp->QueryInterface(IID_IDirectDraw4, (LPVOID *)&lpdd)))
		return(0);

	// blow the genric interface away, no longer
	//	needed
	lpddtemp->Release();
	lpddtemp = NULL;

	// now set the cooperative level to FULLSCREEN
	if (FAILED(lpdd->SetCooperativeLevel(main_window_handle,
		DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE |
		DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT)))
		return(0);

	// set the display size
	if (FAILED(lpdd->SetDisplayMode(width, height, bpp, 0, 0)))
		return(0);

	// ready to rock, and create surfaces
	//	clear the dd-surfce description struct first
	DDRAW_INIT_STRUCT(ddsd);

	// now enable the valid feilds
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

	// set backbuffer count to 1
	ddsd.dwBackBufferCount = 1;

	// now request a complex, flippable surface
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX |
		DDSCAPS_FLIP;

	// create the primary surfce
	if (FAILED(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL)))
		return(0);

	// set up the back buffer
	DDRAW_INIT_STRUCT(ddsd);
	ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

	// query for the attached surface
	if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
		return(0);

	// finally create the palette data and pointer
	
	// loop and fill the palette data with random colors
	//	but leave index 0 and 255 to black and white respectively
	for (int index=1; index < MAX_COLORS_PALETTE-1; index++)
	{
		palette[index].peBlue	= rand()%256;
		palette[index].peGreen	= rand()%256;
		palette[index].peRed	= rand()%256;
		palette[index].peFlags	= PC_NOCOLLAPSE;

	} // end for index

	// set color in palette 0 to black
	palette[0].peBlue	= 0;
	palette[0].peGreen	= 0;
	palette[0].peRed	= 0;
	palette[0].peFlags	= PC_NOCOLLAPSE;

	// set color in palette 255 to white
	palette[255].peBlue		= 255;
	palette[255].peGreen	= 255;
	palette[255].peRed		= 255;
	palette[255].peFlags	= PC_NOCOLLAPSE;

	// now create the palette object with the 
	//	created palette data
	if (FAILED(lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 |
		DDPCAPS_INITIALIZE, palette, &lpddpal, NULL)))
		return(0);

	// finally attach the palette to primary surface
	if (FAILED(lpddsprimary->SetPalette(lpddpal)))
		return(0);

	// return success
	return(1);

} // end DDraw_Init

//--------------------------------------------------|

int DDraw_Shutdown(void)
{
// shut's down ddraw and releases resources

	// release the clipper
	if (lpddclipper)
	{
		lpddclipper->Release();
		lpddclipper = NULL;
	} // end if lpddclipper

	// release the palette object
	if (lpddpal)
	{
		lpddpal->Release();
		lpddpal = NULL;
	} // end if lpddpal

	// release the back buffer
	if (lpddsback)
	{
		lpddsback->Release();
		lpddsback = NULL;
	} // end if lpddsback

	// relase the primary surface
	if (lpddsprimary)
	{
		lpddsprimary->Release();
		lpddsprimary = NULL;
	} // end if lpddsprimary

	// relase IDirectDraw 4
	if (lpdd)
	{
		lpdd->Release();
		lpdd = NULL;
	} // end if lpdd

	// success
	return(1);

} // end DDraw_Shutdown

//--------------------------------------------------|

LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE4 lpdds,
										 int num_rects,
										 LPRECT clip_list)
{
// this function creates a clipper from the sent clip list and
//	attaches it to the sent surface

	int index;						// looping var
	LPDIRECTDRAWCLIPPER lpddc;		// pointer to the newly created clipper
	LPRGNDATA region_data;			// pointer to the region data and
									//	header

	// first create the direct draw clipper
	if (FAILED(lpdd->CreateClipper(NULL, &lpddc, NULL)))
	{
		// error
		return(0);
	} // end if lpddc

	// now create the clip-list
	// allocate memory
	region_data = (LPRGNDATA)malloc(sizeof(RECT) * num_rects +
		sizeof(RGNDATAHEADER));

	// now copy the rects into region_data
	memcpy(region_data->Buffer, clip_list, sizeof(RECT) * num_rects);

	// set up the feilds of the header
	region_data->rdh.dwSize			= sizeof(RGNDATAHEADER);
	region_data->rdh.iType			= RDH_RECTANGLES;
	region_data->rdh.nCount			= num_rects;
	region_data->rdh.nRgnSize		= num_rects * sizeof(RECT);

	region_data->rdh.rcBound.left	=  64000;
	region_data->rdh.rcBound.top	=  64000;
	region_data->rdh.rcBound.right	= -64000;
	region_data->rdh.rcBound.bottom	= -64000;

	// find bounds of all clipping regions
	for (index = 0; index < num_rects; index++)
	{
		// test if the next rectangle uninioned with
		//	the current bound is larger
		if (clip_list[index].left < region_data->rdh.rcBound.left)
			region_data->rdh.rcBound.left = clip_list[index].left;

		if (clip_list[index].right > region_data->rdh.rcBound.right)
			region_data->rdh.rcBound.right = clip_list[index].right;

		if (clip_list[index].top < region_data->rdh.rcBound.top)
			region_data->rdh.rcBound.top = clip_list[index].top;

		if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
			region_data->rdh.rcBound.bottom = clip_list[index].bottom;

	} // end for index

	// set the clipping list
	if (FAILED(lpddc->SetClipList(region_data, 0)))
	{
		// error, free memory and return
		free(region_data);
		return(0);
	} // end if set

	// attach clipper to the sent surface
	if (FAILED(lpdds->SetClipper(lpddc)))
	{
		// error
		free(region_data);
		return(0);
	} // end if

	// free memory
	free(region_data);

	// return clipper
	return(lpddc);

} // end DDraw_Attach_Clipper

//--------------------------------------------------|

LPDIRECTDRAWSURFACE4 DDraw_Create_Surface(int width, 
                                          int height, 
                                          int mem_flags, 
                                          int color_key_value)
{
// this function creates an offscreen plain surface

	DDSURFACEDESC2 ddsd;         // working description
	LPDIRECTDRAWSURFACE4 lpdds;  // temporary surface
    
	// set to access caps, width, and height
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// set dimensions of the new bitmap surface
	ddsd.dwWidth  =  width;
	ddsd.dwHeight =  height;

	// set surface to offscreen plain
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

	// create the surface
	if (FAILED(lpdd->CreateSurface(&ddsd,&lpdds,NULL)))
		return(NULL);

	// set color key to default color 000
	// note that if this is a 8bit bob then palette index 0 will be 
	// transparent by default
	// note that if this is a 16bit bob then RGB value 000 will be 
	// transparent
	DDCOLORKEY color_key; // used to set color key
	color_key.dwColorSpaceLowValue  = color_key_value;
	color_key.dwColorSpaceHighValue = color_key_value;

	// now set the color key for source blitting
	lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);

	// return surface
	return(lpdds);

} // end DDraw_Create_Surface

//--------------------------------------------------|

int DDraw_Flip(void)
{
// flips the primary with the secondary surface

	// check for either of the surface being locked
	if (primary_buffer || back_buffer)
		return(0);

	// perform flip
	while (FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)));

	return(1);

} // end DDraw_Flip

//--------------------------------------------------|

int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE4 lpdds, int color, RECT *client)
{
// fills the sent surface with the sent color

	// clear the global ddbltfx
	DDRAW_INIT_STRUCT(ddbltfx);

	// set the fill color
	ddbltfx.dwFillColor = color;

	// perform the mem fill
	if (FAILED(lpdds->Blt(client,
		NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx)))
		return(0);

	return(1);

} // end DDraw_Fill_Surface

//--------------------------------------------------|

UCHAR *DDraw_Lock_Surface(LPDIRECTDRAWSURFACE4 lpdds, int *lpitch)
{
// this function locks the sent surface and returns a memory
//	pointer to it

	// test if surface is valid
	if (!lpdds)
		return(0);

	// clear ddsd
	DDRAW_INIT_STRUCT(ddsd);

	// lock surface
	if (FAILED(lpdds->Lock(NULL, &ddsd, 
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)))
		return(0);

	// now surface is locked lpitch and lpSurface feilds are
	//	valid

	// set the memory pitch
	if (lpitch)
		*lpitch = (int)ddsd.lPitch;

	// return surface pointer
	return((UCHAR *)ddsd.lpSurface);

} // end DDraw_Lock_Surface

//--------------------------------------------------|

int DDraw_Unlock_Surface(LPDIRECTDRAWSURFACE4 lpdds)
{
// unlocks the sent surface

	// test for a valid surface 
	if (!lpdds)
		return(0);

	// unlock
	if (FAILED(lpdds->Unlock(NULL)))
		return(0);

	return(1);

} // end DDraw_Unlock_Surface

//--------------------------------------------------|

UCHAR *DDraw_Lock_Primary_Surface(void)
{
// this function locks the primary surface and returns
//	a pointer to it. Update's the globals primary_pitch
//	and primary_buffer

	// test for a locked surface and
	//	return it
	if (primary_buffer)
		return(primary_buffer);

	// clear ddsd
	DDRAW_INIT_STRUCT(ddsd);

	// lock the surface and set the globals
	if (FAILED(lpddsprimary->Lock(NULL,
		&ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)))
		return(0);

	primary_lpitch	= (int)ddsd.lPitch;
	primary_buffer	= (UCHAR *)ddsd.lpSurface;

	// return pointer
	return(primary_buffer);

} // end DDraw_Lock_Primary_Surface

//--------------------------------------------------|

int DDraw_Unlock_Primary_Surface(void)
{
	// test for locked
	if (!primary_buffer)
		return(0);

	// unlock
	if (FAILED(lpddsprimary->Unlock(NULL)))
		return(0);

	// reset globals
	primary_buffer	= NULL;
	primary_lpitch	= 0;

	return(1);

} // end DDraw_unlock_primary_surface

//--------------------------------------------------|

UCHAR *DDraw_Lock_Back_Surface(void)
{
// lock's the back surface and sends a pointer to it

	// test if surface is locked
	if (back_buffer)
		return(back_buffer);

	// clear surface desc struct
	DDRAW_INIT_STRUCT(ddsd);

	// lock
	if (FAILED(lpddsback->Lock(NULL, &ddsd,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
		return(0);

	// set the globals
	back_lpitch	= (int)ddsd.lPitch;
	back_buffer = (UCHAR *)ddsd.lpSurface;

	// return pointer
	return(back_buffer);

} // end Lock_Back_Buffer

//--------------------------------------------------|

int DDraw_Unlock_Back_Surface(void)
{
// unlock's the back surface

	// test for a valid surface
	if (!back_buffer)
		return(0);

	// unlock
	if (FAILED(lpddsback->Unlock(NULL)))
		return(0);

	// clear the globals
	back_lpitch	= 0;
	back_buffer	= NULL;

	return(1);

} // end DDraw_Unlock_Bak_Buffer

//--------------------------------------------------|

DWORD Get_Clock(void)
{
// this function returns the current tick count

	return(GetTickCount());

} // end Get_Clock

//--------------------------------------------------|

DWORD Start_Clock(void)
{
// start's the clock count. Saves the current count
//	on global start_clock_count from calls to Get_CLock

	return(start_clock_count = Get_Clock());

} // end Start_Clock

//--------------------------------------------------|

DWORD Wait_Clock(DWORD count)
{
// used to wait for a specific number of clicks

	while ((Get_Clock() - start_clock_count) < count);
	return(Get_Clock());

} // end Wait_Clock

//--------------------------------------------------|

int Draw_Line(int x0, int y0,		// starting position
			  int x1, int y1,		// end
			  UCHAR color,			// color index
			  UCHAR *vb_start,		// video buffer
			  int lpitch)			// memory bytes per line
{
// this function draws a line from x0,y0 to x1,y1
// using differtial
// error terms (based on Bresenham's work)

	int dx,				// difference in x
		dy,				// difference in y
		dx2,			// 2 * dx, dy
		dy2,
		x_inc,			// amount in pixel space to move
		y_inc,
		error,			// the disicion variable
		index;			// used for looping

	// precompute the start of the pixel address
	vb_start = vb_start + x0 + y0*lpitch;

	// compute horizontal and vertical delta's
	dx = x1 - x0;
	dy = y1 - y0;

	// test the angle of the slope
	if (dx >= 0)
	{
		x_inc = 1;

	} // end if moving right
	else
	{
		x_inc = -1;
		dx = -dx;		// need absolute value

	} // end if going left

	if (dy >= 0)
	{
		y_inc = lpitch;

	} // end if going down
	else
	{
		y_inc = -lpitch;
		dy = -dy;			// absolute value

	} // end if up

	// compute (dx, dy) * 2
	dx2 = dx << 1;
	dy2 = dy << 1;

	// now based on which delta is greater, we can draw the line
	if (dx > dy)
	{
		// intialize the error term
		error = dy2 - dx;

		// draw the line
		for (index = 0; index < dx; index++)
		{
			// set the pixel
			*vb_start = color;

			// test if error has overflowed
			if (error >= 0)
			{
				error -= dx2;

				// move to the next line
				vb_start += y_inc;

			} // end if error overflowed

			// adjust the error term
			error += dy2;

			// move to the next pixel
			vb_start += x_inc;

		} // end for index
	} // end if |slope| >= 1
	else
	{
		// initalize the error term
		error = dx2- dy;

		// draw the line
		for (index = 0; index < dy; index++)
		{
			// set the pixel
			*vb_start = color;

			// test if overflowed
			if (error >= 0)
			{
				error -= dy2;

				// move to next pixel
				vb_start += x_inc;

			} // end if overflowed

			// adjust the error
			error += dx2;

			// move to the next line
			vb_start += y_inc;

		} // end for index
	} // end else |slope| > 1

	// return success
	return(1);

} // end Draw_line

//--------------------------------------------------|

int Clip_Line(int &x1, int &y1, int &x2, int &y2)
{
// this function clips the sent line using the globally
//	defined clipping region, (based on Cohen-Sutherland's work)

// internal clipping codes
#define CLIP_CODE_C		0x0000
#define CLIP_CODE_N		0x0008
#define CLIP_CODE_S		0x0004
#define CLIP_CODE_E		0x0002
#define CLIP_CODE_W		0x0001

#define CLIP_CODE_NE	0x000A
#define CLIP_CODE_SE	0x0006
#define CLIP_CODE_NW	0x0009
#define CLIP_CODE_SW	0x0005

	int xc1 = x1,
		yc1 = y1,
		xc2 = x2,
		yc2 = y2;

	int p1_code = 0,
		p2_code = 0;

	// determine codes for p1 and p2
	if (y1 < min_clip_y)
		p1_code |= CLIP_CODE_N;
	else
		if (y1 > max_clip_y)
			p1_code |= CLIP_CODE_S;

	if (x1 < min_clip_x)
		p1_code |= CLIP_CODE_W;
	else 
		if (x1 > max_clip_x)
			p1_code |= CLIP_CODE_E;

	if (y2 < min_clip_y)
		p2_code |= CLIP_CODE_N;
	else
		if (y2 > max_clip_y)
			p2_code |= CLIP_CODE_S;

	if (x2 < min_clip_x)
		p2_code |= CLIP_CODE_W;
	else
		if (x2 > max_clip_x)
			p2_code |= CLIP_CODE_E;

	// try and trivially reject
	if ((p1_code & p2_code))
		return(0);

	// test for totally visible if so leave pts untoched
	if ((p1_code == 0 && p2_code == 0))
		return(1);

	// determine clip point for p1
	switch (p1_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
		{
			yc1 = min_clip_y;
			xc1 =(int)(x1 + 0.5 + (min_clip_y - y1) * (x2 - x1) / (y2 - y1));
		} break;
	case CLIP_CODE_S:
		{
			yc1 = max_clip_y;
			xc1 = (int)(x1 + 0.5+ (max_clip_y - y1) * (x2 - x1) / (y2 - y1));
		} break;
		
	case CLIP_CODE_W:
		{
			xc1 = min_clip_x;
			yc1 = (int)(y1 + 0.5 + (min_clip_x - x1) * (y2 - y1) / (x2 - x1));
		} break;
	case CLIP_CODE_E:
		{
			xc1 = max_clip_x;
			yc1 = (int)(y1 + 0.5 + (max_clip_x - x1) * (y2 - y1) / (x2 - x1));
		} break;

		// these are more complex must compute 2 intersections
	case CLIP_CODE_NE:
		{
			// north hline intersection
			yc1 = min_clip_y;
			xc1 = (int)(x1 + 0.5 + (min_clip_y - y1) * (x2 - x1) / (y2 - y1));

			// test if intersection is valid, if so
			// then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
			{
				// east vline intersection
				xc1 = max_clip_x;
				yc1 = (int)(y1 + 0.5 + (max_clip_x - x1) * (y2 - y1) / (x2 - x1));
			} // end if
		} break;

	case CLIP_CODE_SE:
		{
			// south hline intersection
			yc1 = max_clip_y;
			xc1 = (int)(x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1));
			
			// test if intersection is valid,
			// if so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
			{
				// east vline intersection
				xc1 = max_clip_x;
				yc1 = (int)(y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1));
			} // end if
		} break;
	
	case CLIP_CODE_NW:
		{
			// north hline intersection
			yc1 = min_clip_y;
			xc1 = (int)(x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1));
			
			// test if intersection is valid,
			// if so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
			{
				xc1 = min_clip_x;
				yc1 = (int)(y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1));
			} // end if
		} break;

	case CLIP_CODE_SW:
		{
			// south hline intersection
			yc1 = max_clip_y;
			xc1 = (int)(x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1));
			
			// test if intersection is valid,
			// if so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
			{
				xc1 = min_clip_x;
				yc1 = (int)(y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1));
			} // end if
		} break;
	
	default:break;
	
	} // end switch
	
	// determine clip point for p2
	switch(p2_code)
	{
	case CLIP_CODE_C: break;
	
	case CLIP_CODE_N:
		{
			yc2 = min_clip_y;
			xc2 = (int)(x2 + (min_clip_y-y2)*(x1-x2)/(y1-y2));
		} break;
	case CLIP_CODE_S:
		{
			yc2 = max_clip_y;
			xc2 = (int)(x2 + (max_clip_y-y2)*(x1-x2)/(y1-y2));
		} break;
	
	case CLIP_CODE_W:
		{
			xc2 = min_clip_x;
			yc2 = (int)(y2 + (min_clip_x-x2)*(y1-y2)/(x1-x2));
		} break;
	case CLIP_CODE_E:
		{
			xc2 = max_clip_x;
			yc2 = (int)(y2 + (max_clip_x-x2)*(y1-y2)/(x1-x2));
		} break;
		
		// these cases are more complex, must compute 2 intersections
	case CLIP_CODE_NE:
		{
			// north hline intersection
			yc2 = min_clip_y;
			xc2 = (int)(x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2));
			
			// test if intersection is valid,
			// if so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
			{
				// east vline intersection
				xc2 = max_clip_x;
				yc2 = (int)(y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2));
			} // end if
		} break;
	
	case CLIP_CODE_SE:
		{
			// south hline intersection
			yc2 = max_clip_y;
			xc2 = (int)(x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2));

			// test if intersection is valid,
			// if so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
			{
				// east vline intersection
				xc2 = max_clip_x;
				yc2 = (int)(y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2));
			} // end if
		} break;
	
	case CLIP_CODE_NW:
		{
			// north hline intersection
			yc2 = min_clip_y;
			xc2 = (int)(x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2));
			
			// test if intersection is valid,
			// if so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
			{
				xc2 = min_clip_x;
				yc2 = (int)(y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2));
			} // end if
		} break;

	case CLIP_CODE_SW:
		{
			// south hline intersection
			yc2 = max_clip_y;
			xc2 = (int)(x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2));

			// test if intersection is valid,
			// if so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
			{
				xc2 = min_clip_x;
				yc2 = (int)(y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2));
			} // end if
		} break;

	default:break;
	
	} // end switch

	// do bounds check
	if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
		(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
		(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
		(yc2 < min_clip_y) || (yc2 > max_clip_y) )
	{
		return(0);
	} // end if

	// store vars back
	x1 = xc1;
	y1 = yc1;
	x2 = xc2;
	y2 = yc2;

	return(1);

} // end Clip_Line

//--------------------------------------------------|

int Draw_Clip_Line(int x1, int y1, int x2, int y2, UCHAR color,
				   UCHAR *dest_buffer, int lpitch)
{
// this helper function draw's a clipped line
	int cxs, cys,
		cxe, cye;

	// copy parms
	cxs = x1;
	cys = y1;
	cxe = x2;
	cye = y2;

	// draw clipped line
	if (Clip_Line(cxs,cys,cxe,cye))
		Draw_Line(cxs,cys,cxe,cye,color,dest_buffer,lpitch);

	// return success
	return(1);

} // end Draw_Clip_Line

//--------------------------------------------------|

int Draw_Pixel(int x, int y, int color,
			   UCHAR *video_buffer, int lpitch)
{
// Plot's a pixel at location x,y using marker color

	video_buffer[x + y*lpitch] = color;

	return(1);

} // end Plot_Pixel

//--------------------------------------------------|

int Draw_Pixel16(int x, int y,int color,
                        UCHAR *video_buffer, int lpitch)
{
// this function plots a single pixel at x,y with color

	((USHORT *)video_buffer)[x + y*(lpitch >> 1)] = color;

	// return success
	return(1);

} // end Draw_Pixel16

//--------------------------------------------------|

int Draw_Rectangle(int x1, int y1, int x2, int y2, int color,
				   LPDIRECTDRAWSURFACE4 lpdds)
{
// Draw's a filled-rectangle using the points and color
//	on the sent surface

	RECT fill_area;		// contains the destination rectangle

	// clear the blitter fx structure
	DDRAW_INIT_STRUCT(ddbltfx);

	// set the color
	ddbltfx.dwFillColor = color;

	// now set the point's for the fill_area
	fill_area.top		= y1;
	fill_area.left		= x1;
	fill_area.bottom	= y2;
	fill_area.right		= x2;

	// blit
	if (FAILED(lpdds->Blt(&fill_area, NULL, NULL,
		DDBLT_WAIT | DDBLT_COLORFILL,&ddbltfx)))
		return(0);

	// 
	return(1);

} // end Draw_Rectangle

//--------------------------------------------------|

int Set_Palette_Entry(int color_index, LPPALETTEENTRY color)
{
// set a palette color in the palette

	lpddpal->SetEntries(0,color_index,1,color);

	// set data in shadow palette
	memcpy(&palette[color_index],color,sizeof(PALETTEENTRY));

	// return success
	return(1);
} // end Set_Palette_Entry

//--------------------------------------------------|

int Get_Palette_Entry(int color_index,LPPALETTEENTRY color)
{
// this function retrieves a palette entry from the color
// palette

	// copy data out from shadow palette
	memcpy(color, &palette[color_index],sizeof(PALETTEENTRY));

	// return success
	return(1);
} // end Get_Palette_Entry

//--------------------------------------------------|

int Save_Palette(LPPALETTEENTRY sav_palette)
{
// this function saves the current palette 

	memcpy(sav_palette, palette,MAX_COLORS_PALETTE*sizeof(PALETTEENTRY));

	// return success
	return(1);
} // end Save_Palette

//--------------------------------------------------|

int Set_Palette(LPPALETTEENTRY set_palette)
{
// this function writes the sent palette

	// first save the new palette in shadow
	memcpy(palette, set_palette,MAX_COLORS_PALETTE*sizeof(PALETTEENTRY));

	// now set the new palette
	lpddpal->SetEntries(0,0,MAX_COLORS_PALETTE,palette);

	// return success
	return(1);
} // end Set_Palette

//--------------------------------------------------|

int Draw_Text_GDI(char *text, int x, int y, COLORREF color,
				  LPDIRECTDRAWSURFACE4 lpdds)
{
// Draws the sent text on the sent surface, using GDI calls

	HDC xdc;		// working dc

	// request the dc from the surface
	if (FAILED(lpdds->GetDC(&xdc)))
	{
		// error
		return(0);
	} // end if

	// set the text color
	SetTextColor(xdc, color);

	// set bk mode to transparent
	SetBkMode(xdc, TRANSPARENT);

	// print the text
	TextOut(xdc, x, y, text, strlen(text));

	// release the dc
	lpdds->ReleaseDC(xdc);

	return(1);

} // end Draw_Text_GDI

//--------------------------------------------------|

int Draw_Text_GDI(char *text, int x,int y,int color, LPDIRECTDRAWSURFACE4 lpdds)
{
// this function draws the sent text on the sent surface 
// using color index as the color in the palette

	HDC xdc; // the working dc

	// get the dc from surface
	if (FAILED(lpdds->GetDC(&xdc)))
		return(0);

	// set the colors for the text up
	SetTextColor(xdc,RGB(palette[color].peRed,palette[color].peGreen,palette[color].peBlue) );
	
	// set background mode to transparent so black isn't copied
	SetBkMode(xdc, TRANSPARENT);

	// draw the text a
	TextOut(xdc,x,y,text,strlen(text));

	// release the dc
	lpdds->ReleaseDC(xdc);

	// return success
	return(1);

} // end Draw_Text_GDI

//--------------------------------------------------|

int Rotate_Colors(int start_index, int end_index)
{
// rotates the colors b/n the start and the end

	int colors = end_index - start_index + 1;

	PALETTEENTRY working_pal[MAX_COLORS_PALETTE];	// working palette

	// get the color palette
	lpddpal->GetEntries(0,start_index,colors,working_pal);

	// shift the colors
	lpddpal->SetEntries(0,start_index+1,colors-1,working_pal);

	// fix up the last color
	lpddpal->SetEntries(0,start_index,1,&working_pal[colors-1]);

	// update the shadow palette
	lpddpal->GetEntries(0,0,MAX_COLORS_PALETTE,palette);

	return(1);

} // end Rotate_colors

//--------------------------------------------------|

int Blink_Colors(int command, BLINKER_PTR new_light, int id)
{
// blinks a set of lights

	static BLINKER lights[256];
	static int initalized = 0;		// track's if function initalized

	// test if this is the first time the function has run
	if (!initalized)
	{
		// set initalized
		initalized = 1;

		// clear out all structures
		memset((void *)lights, 0, sizeof(lights));

	} // end if 

	// now test what command user is sending
	switch (command)
	{
	case BLINKER_ADD:			// add a light to the data base
		{
			// run thru the data base and find an open light
			for (int index=0; index < 256; index++)
			{
				// is this light avaliable
				if (lights[index].state == 0)
				{
					// set light up
					lights[index] = *new_light;

					// set internal feilds up
					lights[index].counter	= 0;
					lights[index].state		= -1;	// off

					// update palette entry
					lpddpal->SetEntries(0,lights[index].color_index,1,&lights[index].off_color);

					// return id to caller
					return(index);

				} // end if

			} // end for index

		} break;

	case BLINKER_DELETE:		// delete the light indicated by id
		{
			// delete the light sent in id
			if (lights[id].state != 0)
			{
				// kill the light
				memset((void *)&lights[id],0,sizeof(BLINKER));

				return(id);

			} // en dif
			else
				return(-1);

		} break;

	case BLINKER_UPDATE:			// update light indicated by id
		{
			// make sure light is active
			if (lights[id].state != 0)
			{
				// update on/off params only
				lights[id].off_color	= new_light->off_color;
				lights[id].on_color		= new_light->on_color;
				lights[id].on_time		= new_light->on_time;
				lights[id].off_time		= new_light->off_time;

				// update palette entry
				if (lights[id].state == -1)
					lpddpal->SetEntries(0,lights[id].color_index,1,&lights[id].off_color);
				else
					lpddpal->SetEntries(0,lights[id].color_index,1,&lights[id].on_color);

				return(id);

			} // end if
			else
				return(-1);

		} break;

	case BLINKER_RUN:			// run normal
		{
			// run thru the data base and process each light
			for (int index=0; index < 256; index++)
			{
				// is this active?
				if (lights[index].state == -1)
				{
					// update conuter
					if (++lights[index].counter >= lights[index].off_time)
					{
						// reset counter
						lights[index].counter = 0;

						// change states 
						lights[index].state = -lights[index].state;

						// update color
						lpddpal->SetEntries(0,lights[index].color_index,1,&lights[index].on_color);

					} // end if

				} // end if
				else
				if (lights[index].state == 1)
				{
					// update conter
					if (++lights[index].counter >= lights[index].on_time)
					{
						// reset counter
						lights[index].counter = 0;

						// change states
						lights[index].state = -lights[index].state;

						// update color
						lpddpal->SetEntries(0,lights[index].color_index,1,
							&lights[index].off_color);

					} // end if

				} // end if

			} // end for

		} break;

	default: break;

	} // end switch

	return(1);

} // end Blink_Colors

//--------------------------------------------------|

int Create_Bitmap(BITMAP_IMAGE_PTR image, int x, int y, int width, int height, int bpp)
{
// this function is used to intialize a bitmap, 8 or 16 bit

	// allocate the memory
	if (!(image->buffer = (UCHAR *)malloc(width*height*(bpp>>3))))
		return(0);

	// initialize variables
	image->state     = BITMAP_STATE_ALIVE;
	image->attr      = 0;
	image->width     = width;
	image->height    = height;
	image->bpp       = bpp;
	image->x         = x;
	image->y         = y;
	image->num_bytes = width*height*(bpp>>3);

	// clear memory out
	memset(image->buffer,0,width*height*(bpp>>3));

	// return success
	return(1);

} // end Create_Bitmap

//--------------------------------------------------|

int Destroy_Bitmap(BITMAP_IMAGE_PTR image)
{
// this function releases the memory associated with a bitmap

	if (image && image->buffer)
	{
		// free memory and reset vars
		free(image->buffer);

		// set all vars in structure to 0
		memset(image,0,sizeof(BITMAP_IMAGE));

		// return success
		return(1);

	} // end if
	else  // invalid entry pointer of the object wasn't initialized
		return(0);

} // end Destroy_Bitmap

//--------------------------------------------------|

int Draw_Bitmap(BITMAP_IMAGE_PTR source_bitmap,UCHAR *dest_buffer, int lpitch, int transparent)
{
// this function draws the bitmap onto the destination memory surface
// if transparent is 1 then color 0 (8bit) or 0.0.0 (16bit) will be transparent
// note this function does NOT clip, so be carefull!!!

	if (!source_bitmap)
	{
		MessageBox(main_window_handle, "Empty bitmap pointer", "Error",
			MB_OK | MB_ICONERROR);
		return(0);
	} //

	// test if this bitmap is loaded
	if (!(source_bitmap->attr & BITMAP_ATTR_LOADED))
		return(0);

	UCHAR *dest_addr,   // starting address of bitmap in destination
          *source_addr; // starting adddress of bitmap data in source

    UCHAR pixel;        // used to hold pixel value

    int index,          // looping vars
        pixel_x;

	// compute starting destination address
	dest_addr = dest_buffer + source_bitmap->y*lpitch + source_bitmap->x;

	// compute the starting source address
	source_addr = source_bitmap->buffer;

	// is this bitmap transparent
	if (transparent)
	{
		// copy each line of bitmap into destination with transparency
		for (index=0; index<source_bitmap->height; index++)
		{
			// copy the memory
			for (pixel_x=0; pixel_x<source_bitmap->width; pixel_x++)
			{
				if ((pixel = source_addr[pixel_x])!=0)
					dest_addr[pixel_x] = pixel;

			} // end if

			// advance all the pointers
			dest_addr   += lpitch;
			source_addr += source_bitmap->width;

		} // end for index
	} // end if
   else
   {
		// non-transparent version
		// copy each line of bitmap into destination

		for (index=0; index < source_bitmap->height; index++)
		{
			// copy the memory
			memcpy(dest_addr, source_addr, source_bitmap->width);

			// advance all the pointers
			dest_addr   += lpitch;
			source_addr += source_bitmap->width;

		} // end for index

	} // end else

	// return success
	return(1);

} // end Draw_Bitmap

//--------------------------------------------------|

int Load_Image_Bitmap(BITMAP_IMAGE_PTR image,  // bitmap image to load with data
                      BITMAP_FILE_PTR bitmap,  // bitmap to scan image data from
                      int cx,int cy,   // cell or absolute pos. to scan image from
                      int mode)        // if 0 then cx,cy is cell position, else 
                                       // cx,cy are absolute coords
{
// this function extracts a bitmap out of a bitmap file

	// is this a valid bitmap
	if (!image)
		return(0);

	UCHAR *source_ptr,   // working pointers
		*dest_ptr;

	// test the mode of extraction, cell based or absolute
	if (mode==BITMAP_EXTRACT_MODE_CELL)
	{
		// re-compute x,y
		cx = cx*(image->width+1) + 1;
		cy = cy*(image->height+1) + 1;
	} // end if

	// extract bitmap data
	source_ptr = bitmap->buffer +
		cy*bitmap->bitmapinfoheader.biWidth+cx;

	// assign a pointer to the bimap image
	dest_ptr = (UCHAR *)image->buffer;

	// iterate thru each scanline and copy bitmap
	for (int index_y=0; index_y<image->height; index_y++)
	{
		// copy next line of data to destination
		memcpy(dest_ptr, source_ptr,image->width);

		// advance pointers
		dest_ptr   += image->width;
		source_ptr += bitmap->bitmapinfoheader.biWidth;
    } // end for index_y

	// set state to loaded
	image->attr |= BITMAP_ATTR_LOADED;

	// return success
	return(1);

} // end Load_Image_Bitmap

//--------------------------------------------------|

int Scroll_Bitmap(BITMAP_IMAGE_PTR image, int dx, int dy)
{
// this function scrolls a bitmap

	BITMAP_IMAGE temp_image; // temp image buffer

	// are the parms valid 
	if (!image || (dx==0 && dy==0))
		return(0);


	// scroll on x-axis first
	if (dx!=0)
	{
		// step 1: normalize scrolling amount
		dx %= image->width;

		// step 2: which way?
	if (dx > 0)
	{
		// scroll right
		// create bitmap to hold region that is scrolled around
		Create_Bitmap(&temp_image, 0, 0, dx, image->height, image->bpp);

		// copy region we are going to scroll and wrap around
		Copy_Bitmap(&temp_image,0,0, 
			image, image->width-dx,0, 
                dx, image->height);

		// set some pointers up
		UCHAR *source_ptr = image->buffer;  // start of each line
		int shift         = (image->bpp >> 3)*dx;

		// now scroll image to right "scroll" pixels
		for (int y=0; y < image->height; y++)
		{
			// scroll the line over
			memmove(source_ptr+shift, source_ptr, (image->width-dx)*(image->bpp >> 3));
    
			// advance to the next line
			source_ptr+=((image->bpp >> 3)*image->width);
		} // end for
   
		// and now copy it back
		Copy_Bitmap(image, 0,0, &temp_image,0,0, 
               dx, image->height);           

		} // end if
		else
		{
			// scroll left
			dx = -dx; // invert sign

			// create bitmap to hold region that is scrolled around
			Create_Bitmap(&temp_image, 0, 0, dx, image->height, image->bpp);

			// copy region we are going to scroll and wrap around
			Copy_Bitmap(&temp_image,0,0, 
                image, 0,0, 
                dx, image->height);

			// set some pointers up
			UCHAR *source_ptr = image->buffer;  // start of each line
			int shift         = (image->bpp >> 3)*dx;

			// now scroll image to left "scroll" pixels
			for (int y=0; y < image->height; y++)
			{
				// scroll the line over
				memmove(source_ptr, source_ptr+shift, (image->width-dx)*(image->bpp >> 3));
    
				// advance to the next line
				source_ptr+=((image->bpp >> 3)*image->width);
			} // end for
   
			// and now copy it back
			Copy_Bitmap(image, image->width-dx,0, &temp_image,0,0, 
               dx, image->height);           

		} // end else
	} // end scroll on x-axis


	// return success
	return(1);

} // end Scroll_Bitmap

//--------------------------------------------------|

int Copy_Bitmap(BITMAP_IMAGE_PTR dest_bitmap, int dest_x, int dest_y, 
                BITMAP_IMAGE_PTR source_bitmap, int source_x, int source_y, 
                int width, int height)
{
// this function copies a bitmap from one source to another

	// make sure the pointers are at least valid
	if (!dest_bitmap || !source_bitmap)
		return(0);

	// do some computations
	int bytes_per_pixel = (source_bitmap->bpp >> 3);

	// create some pointers
	UCHAR *source_ptr = source_bitmap->buffer + (source_x + source_y*source_bitmap->width)*bytes_per_pixel;
	UCHAR *dest_ptr   = dest_bitmap->buffer   + (dest_x   + dest_y  *dest_bitmap->width)  *bytes_per_pixel;

	// now copying is easy :)
	for (int y = 0; y < height; y++)
	{
		// copy this line
		memcpy(dest_ptr, source_ptr, bytes_per_pixel*width);

		// advance the pointers
		source_ptr+=(source_bitmap->width*bytes_per_pixel);
		dest_ptr  +=(dest_bitmap->width*bytes_per_pixel);
	} // end for

	// return success
	return(1);

} // end Copy_Bitmap

//--------------------------------------------------|

int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename)
{
// Open's a bitmap file using Windows API and loads data into bitmap

	HANDLE file_handle=NULL;	// file handle
	DWORD bytes_read;			// indicate's the number of bytes read
	int index;					// general looping index

	// open file for reading
	file_handle = CreateFile(filename,		// filename
		GENERIC_READ,						// read
		FILE_SHARE_READ,					// sharing mode -- read
		NULL,								// no security
		OPEN_EXISTING,						// existing file
		FILE_ATTRIBUTE_NORMAL,				// normal file 
		NULL);								// no attribute template

	if (!file_handle)
		return(0);

	// now that file is open reading, then read-in the 
	//	BITMAPFILEHEADER
	ReadFile(file_handle,					// file_handle
		(LPVOID)&bitmap->bitmapfileheader,	// pointer to buffer
		sizeof(BITMAPFILEHEADER),			// bytes to read
		&bytes_read,						// bytes read
		NULL);		

	// now check if this is a bitmap file
	if (bitmap->bitmapfileheader.bfType != BITMAP_ID)
	{
		// close file
		CloseHandle(file_handle);

		return(0);
	} // end if not bitmap 

	// up next read-in the BITMAPINFOHEADER
	ReadFile(file_handle,					// file handle
		(LPVOID)&bitmap->bitmapinfoheader,	// buffer
		sizeof(BITMAPINFOHEADER),			// size to read
		&bytes_read,
		NULL);

	// test if bitmap is in 8-bit mode
	if (bitmap->bitmapinfoheader.biBitCount == 8)
	{
		// read the palette data
		ReadFile(file_handle,
			(LPVOID)bitmap->palette,
			sizeof(PALETTEENTRY)*MAX_COLORS_PALETTE,
			&bytes_read,
			NULL);

		// reverse the BGR order to RGB in data
		for (index=0; index < MAX_COLORS_PALETTE; index++)
		{
			// rearrange blue channel with red
			UCHAR temp = bitmap->palette[index].peRed;
			bitmap->palette[index].peRed = bitmap->palette[index].peBlue;
			bitmap->palette[index].peBlue = temp;

			// set peFlags to PC_NOCOLLAPSE
			bitmap->palette[index].peFlags = PC_NOCOLLAPSE;

		} // end for index

	} // end if 8-bit bitmap

	// last read the actual bitmap data itself

	// allocate memory for the buffer
	if (bitmap->buffer)
		free(bitmap->buffer);

	// allocate mem
	if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
	{
		CloseHandle(file_handle);

		return(0);
	} //end if

	// now read in the data
	ReadFile(file_handle,
		(LPVOID)bitmap->buffer,
		bitmap->bitmapinfoheader.biSizeImage,
		&bytes_read,
		NULL);

	// close the file
	CloseHandle(file_handle);

	// flip the bitmap
	Flip_Bitmap(bitmap->buffer, 
            bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8), 
            bitmap->bitmapinfoheader.biHeight);

	// return success
	return(1);

} // end Load_Bitmap_File

//--------------------------------------------------|

int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap)
{
// this function releases all memory associated with "bitmap"

	if (bitmap->buffer)
	{
		// release memory
		free(bitmap->buffer);

		// reset pointer
		bitmap->buffer = NULL;

	} // end if

	// return success
	return(1);

} // end Unload_Bitmap_File

//--------------------------------------------------|

int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height)
{
// this function is used to flip bottom-up .BMP images

	UCHAR *buffer; // used to perform the image processing
	int index;     // looping index

	// allocate the temporary buffer
	if (!(buffer = (UCHAR *)malloc(bytes_per_line*height)))
	  return(0);

	// copy image to work area
	memcpy(buffer,image,bytes_per_line*height);
	
	// flip vertically
	for (index=0; index < height; index++)
		memcpy(&image[((height-1) - index)*bytes_per_line],
			&buffer[index*bytes_per_line], bytes_per_line);

	// release the memory
	free(buffer);

	// return success
	return(1);

} // end Flip_Bitmap

//--------------------------------------------------|

int Collision_Test(int x1, int y1, int w1, int h1, 
                   int x2, int y2, int w2, int h2) 
{
// this function tests if the two rects overlap

	// get the radi of each rect
	int width1  = (w1>>1) - (w1>>3);
	int height1 = (h1>>1) - (h1>>3);

	int width2  = (w2>>1) - (w2>>3);
	int height2 = (h2>>1) - (h2>>3);

	// compute center of each rect
	int cx1 = x1 + width1;
	int cy1 = y1 + height1;

	int cx2 = x2 + width2;
	int cy2 = y2 + height2;

	// compute deltas
	int dx = abs(cx2 - cx1);
	int dy = abs(cy2 - cy1);

	// test if rects overlap
	if (dx < (width1+width2) && dy < (height1+height2))
		return(1);
	else	// else no collision
		return(0);

} // end Collision_Test

//--------------------------------------------------|

int Color_Scan(int x1, int y1, int x2, int y2, 
               UCHAR scan_start, UCHAR scan_end,
               UCHAR *scan_buffer, int scan_lpitch)
{
// this function implements a crude collision technique
// based on scanning for a range of colors within a rectangle

	// clip rectangle

	// x coords first    
	if (x1 >= screen_width)
		x1=screen_width-1;
	else
	if (x1 < 0)
		x1=0;

	if (x2 >= screen_width)
		x2=screen_width-1;
	else
	if (x2 < 0)
		x2=0;

	// now y-coords
	if (y1 >= screen_height)
		y1=screen_height-1;
	else
	if (y1 < 0)
		y1=0;

	if (y2 >= screen_height)
		y2=screen_height-1;
	else
	if (y2 < 0)
		y2=0;

	// scan the region
	scan_buffer +=y1*scan_lpitch;

	for (int scan_y=y1; scan_y<=y2; scan_y++)
    {
		for (int scan_x=x1; scan_x<=x2; scan_x++)
        {
			if (scan_buffer[scan_x] >= scan_start && scan_buffer[scan_x] <= scan_end )
				return(1);
        } // end for x

		// move down a line
		scan_buffer+=scan_lpitch;

    } // end for y

	// return failure
	return(0);

} // end Color_Scan

//--------------------------------------------------|

int Scan_Image_Bitmap(BITMAP_FILE_PTR bitmap,     // bitmap file to scan image data from
                      LPDIRECTDRAWSURFACE4 lpdds, // surface to hold data
                      int cx,int cy)              // cell to scan image from
{
// this function extracts a bitmap out of a bitmap file

	UCHAR *source_ptr,   // working pointers
		*dest_ptr;

	DDSURFACEDESC2 ddsd;  //  direct draw surface description 

	// get the addr to destination surface memory

	// set size of the structure
	ddsd.dwSize = sizeof(ddsd);

	// lock the display surface
	lpdds->Lock(NULL,
            &ddsd,
            DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
            NULL);

	// compute position to start scanning bits from
	cx = cx*(ddsd.dwWidth+1) + 1;
	cy = cy*(ddsd.dwHeight+1) + 1;
  
	// extract bitmap data
	source_ptr = bitmap->buffer + cy*bitmap->bitmapinfoheader.biWidth+cx;

	// assign a pointer to the memory surface for manipulation
	dest_ptr = (UCHAR *)ddsd.lpSurface;

	// iterate thru each scanline and copy bitmap
	for (int index_y=0; index_y < ddsd.dwHeight; index_y++)
    {
		// copy next line of data to destination
		memcpy(dest_ptr, source_ptr, ddsd.dwWidth);

		// advance pointers
		dest_ptr   += (ddsd.lPitch);
		source_ptr += bitmap->bitmapinfoheader.biWidth;
    } // end for index_y

	// unlock the surface 
	lpdds->Unlock(NULL);

	// return success
	return(1);

} // end Scan_Image_Bitmap

//--------------------------------------------------|

void Screen_Transitions(int effect, UCHAR *vbuffer, int lpitch)
{
// this function can be called to perform a myraid of the screen transitions
//	to the destination buffer, make sure to save and restore palette
//	when performing color transitions in 8-bit modes

	int pal_reg;									// used as loop counter
	int index;										// used as loop counter
	int red,green,blue;								// used in fad algorithm

	PALETTEENTRY color;								// temporary color
	PALETTEENTRY work_palette[MAX_COLORS_PALETTE];  // used as a working palette
	PALETTEENTRY work_color;						// used in color algorithms

	// test which screen effect is being selected
	switch(effect)
	{
	case SCREEN_DARKNESS:
		{
		// fade to black

			for (index=0; index<80; index++)
			{
				// get the palette 
				Save_Palette(work_palette);

				// process each color
				for (pal_reg=1; pal_reg<MAX_COLORS_PALETTE; pal_reg++)
				{
					// get the entry data
					color = work_palette[pal_reg];

					// test if this color register is already black
					if (color.peRed > 4) color.peRed-=3;
					else
						color.peRed = 0;

					if (color.peGreen > 4) color.peGreen-=3;
					else
						color.peGreen = 0;

					if (color.peBlue  > 4) color.peBlue-=3;
					else
						color.peBlue = 0;

					// set the color to a diminished intensity
					work_palette[pal_reg] = color;

				} // end for pal_reg

				// write the palette back out
				Set_Palette(work_palette);

				// wait a bit
               
				Start_Clock(); Wait_Clock(12);
               
			} // end for index

		} break;

	case SCREEN_WHITENESS:
		{
			// fade to white
			for (index=0; index<64; index++)
			{
				// get the palette 
				Save_Palette(work_palette);

				// loop thru all palette registers
				for (pal_reg=0; pal_reg < MAX_COLORS_PALETTE; pal_reg++)
				{
					// get the entry data
					color = work_palette[pal_reg];

					// make 32 bit copy of color
					red   = color.peRed;
					green = color.peGreen;
					blue  = color.peBlue; 

					if ((red+=4) >=255)
						red=255;

					if ((green+=4) >=255)
						green=255;

					if ((blue+=4) >=255)
						blue=255;
                          
					// store colors back
					color.peRed   = red;
					color.peGreen = green;
					color.peBlue  = blue;

					// set the color to a diminished intensity
					work_palette[pal_reg] = color;
                   
				} // end for pal_reg

				// write the palette back out
				Set_Palette(work_palette);

				// wait a bit
               
				Start_Clock(); Wait_Clock(12);

			} // end for index

		} break;

	case SCREEN_REDNESS:
		{
		// fade to red

			for (index=0; index<64; index++)
			{
				// get the palette 
				Save_Palette(work_palette);
               
				// loop thru all palette registers
				for (pal_reg=0; pal_reg < MAX_COLORS_PALETTE; pal_reg++)
				{
					// get the entry data
					color = work_palette[pal_reg];

					// make 32 bit copy of color
					red   = color.peRed;
					green = color.peGreen;
					blue  = color.peBlue; 

					if ((red+=6) >=255)
						red=255; 

					if ((green-=4) < 0)
						green=0;

					if ((blue-=4) < 0)
						blue=0;
                          
					// store colors back
					color.peRed   = red;
					color.peGreen = green;
					color.peBlue  = blue;
                  
					// set the color to a diminished intensity
					work_palette[pal_reg] = color;

				} // end for pal_reg

				// write the palette back out
				Set_Palette(work_palette);

				// wait a bit
               
				Start_Clock(); Wait_Clock(12);

			} // end for index

		} break;

	case SCREEN_BLUENESS:
		{
			// fade to blue

			for (index=0; index<64; index++)
			{
				// get the palette 
				Save_Palette(work_palette);
               
				// loop thru all palette registers
				for (pal_reg=0; pal_reg < MAX_COLORS_PALETTE; pal_reg++)
				{
					// get the entry data
					color = work_palette[pal_reg];

					// make 32 bit copy of color
					red   = color.peRed;
					green = color.peGreen;
					blue  = color.peBlue; 
	
					if ((red-=4) < 0)
						red=0;

					if ((green-=4) < 0)
						green=0;

					if ((blue+=6) >=255)
						blue=255;
                          
					// store colors back
					color.peRed   = red;
					color.peGreen = green;
					color.peBlue  = blue;
                  
					// set the color to a diminished intensity
					work_palette[pal_reg] = color;

				} // end for pal_reg

				// write the palette back out
				Set_Palette(work_palette);

				// wait a bit
               
				Start_Clock(); Wait_Clock(12);

			} // end for index

		} break;

	case SCREEN_GREENNESS:
		{
			// fade to green
			for (index=0; index<64; index++)
			{
				// get the palette 
				Save_Palette(work_palette);

				// loop thru all palette registers
				for (pal_reg=0; pal_reg < MAX_COLORS_PALETTE; pal_reg++)
				{
					// get the entry data
					color = work_palette[pal_reg];                  

					// make 32 bit copy of color
					red   = color.peRed;
					green = color.peGreen;
					blue  = color.peBlue; 

					if ((red-=4) < 0)
						red=0;

					if ((green+=6) >=255)
						green=255;

					if ((blue-=4) < 0)
						blue=0;
                          
					// store colors back
					color.peRed   = red;
					color.peGreen = green;
					color.peBlue  = blue;

					// set the color to a diminished intensity
					work_palette[pal_reg] = color; 

				} // end for pal_reg

				// write the palette back out
				Set_Palette(work_palette);

				// wait a bit
               
				Start_Clock(); Wait_Clock(12);


			} // end for index

		} break;

	case SCREEN_SWIPE_X:
		{
			// do a screen wipe from right to left, left to right
			for (index=0; index < (screen_width/2); index+=2)
			{
				// use this as a 1/70th of second time delay
               
				Start_Clock(); Wait_Clock(12);

				// test screen depth
				if (screen_bpp==8)
				{    
					// draw two vertical lines at opposite ends of the screen
					VLine(0,(screen_height-1),(screen_width-1)-index,0,vbuffer,lpitch);
					VLine(0,(screen_height-1),index,0,vbuffer,lpitch);
					VLine(0,(screen_height-1),(screen_width-1)-(index+1),0,vbuffer,lpitch);
					VLine(0,(screen_height-1),index+1,0,vbuffer,lpitch);
				} // end if 8-bit mode
				else
				if (screen_bpp==16)
				{    
					// 16-bit mode draw two vertical lines at opposite ends of the screen
					VLine16(0,(screen_height-1),(screen_width-1)-index,0,vbuffer,lpitch);
					VLine16(0,(screen_height-1),index,0,vbuffer,lpitch);
					VLine16(0,(screen_height-1),(screen_width-1)-(index+1),0,vbuffer,lpitch);
					VLine16(0,(screen_height-1),index+1,0,vbuffer,lpitch);
				} // end if 16-bit mode


			} // end for index

		} break;

	case SCREEN_SWIPE_Y:
		{
			// do a screen wipe from top to bottom, bottom to top
			for (index=0; index < (screen_height/2); index+=2)
			{
				// use this as a 1/70th of second time delay
               
				Start_Clock(); Wait_Clock(12);

				// test screen depth             
				if (screen_bpp==8)
				{
					// draw two horizontal lines at opposite ends of the screen
					HLine(0,(screen_width-1),(screen_height-1)-index,0,vbuffer,lpitch);
					HLine(0,(screen_width-1),index,0,vbuffer,lpitch);
					HLine(0,(screen_width-1),(screen_height-1)-(index+1),0,vbuffer,lpitch);
					HLine(0,(screen_width-1),index+1,0,vbuffer,lpitch);
				} // end if 8-bit mode
				else 
				if (screen_bpp==16)
				{
					// draw two horizontal lines at opposite ends of the screen
					HLine16(0,(screen_width-1),(screen_height-1)-index,0,vbuffer,lpitch);
					HLine16(0,(screen_width-1),index,0,vbuffer,lpitch);
					HLine16(0,(screen_width-1),(screen_height-1)-(index+1),0,vbuffer,lpitch);
					HLine16(0,(screen_width-1),index+1,0,vbuffer,lpitch);
				} // end if 16-bit mode

			} // end for index


		} break;

	case SCREEN_SCRUNCH:
		{
			// do a screen wipe from top to bottom, bottom to top
			for (index=0; index < (screen_width/2); index+=2)
			{
				// use this as a 1/70th of second time delay
               
				Start_Clock(); Wait_Clock(12);

				// test screen depth             
				if (screen_bpp==8)
				{ 
					// draw two horizontal lines at opposite ends of the screen
					HLine(0,(screen_width-1),(screen_height-1)-index%(screen_height/2),0,vbuffer,lpitch);
					HLine(0,(screen_width-1),index%(screen_height/2),0,vbuffer,lpitch);
					HLine(0,(screen_width-1),(screen_height-1)-(index%(screen_height/2)+1),0,vbuffer,lpitch);
					HLine(0,(screen_width-1),index%(screen_height/2)+1,0,vbuffer,lpitch);
	
					// draw two vertical lines at opposite ends of the screen
					VLine(0,(screen_height-1),(screen_width-1)-index,0,vbuffer,lpitch);
					VLine(0,(screen_height-1),index,0,vbuffer,lpitch);
					VLine(0,(screen_height-1),(screen_width-1)-(index+1),0,vbuffer,lpitch);
					VLine(0,(screen_height-1),index+1,0,vbuffer,lpitch);
				} // end if 8-bit mode
				else
				// test screen depth             
				if (screen_bpp==16)
				{ 
					// draw two horizontal lines at opposite ends of the screen
					HLine16(0,(screen_width-1),(screen_height-1)-index%(screen_height/2),0,vbuffer,lpitch);
					HLine16(0,(screen_width-1),index%(screen_height/2),0,vbuffer,lpitch);
					HLine16(0,(screen_width-1),(screen_height-1)-(index%(screen_height/2)+1),0,vbuffer,lpitch);
					HLine16(0,(screen_width-1),index%(screen_height/2)+1,0,vbuffer,lpitch);
	
					// draw two vertical lines at opposite ends of the screen
					VLine16(0,(screen_height-1),(screen_width-1)-index,0,vbuffer,lpitch);
					VLine16(0,(screen_height-1),index,0,vbuffer,lpitch);
					VLine16(0,(screen_height-1),(screen_width-1)-(index+1),0,vbuffer,lpitch);
					VLine16(0,(screen_height-1),index+1,0,vbuffer,lpitch);
				} // end if 8-bit mode

			} // end for index

		} break;


	case SCREEN_DISSOLVE:
		{
			// disolve the screen by plotting zillions of little black dots

			if (screen_bpp==8)
				for (index=0; index<=screen_width*screen_height*4; index++)
					Draw_Pixel(rand()%screen_width,rand()%screen_height,0,vbuffer,lpitch);
				else
				if (screen_bpp==16)
					for (index=0; index<=screen_width*screen_height*4; index++)
						Draw_Pixel16(rand()%screen_width,rand()%screen_height,0,vbuffer,lpitch);

		} break;

	default:break;

	} // end switch

} // end Screen_Transitions

//--------------------------------------------------|

void VLine(int y1,int y2,int x,int color,UCHAR *vbuffer, int lpitch)
{
// draw a vertical line, note that a memset function can no longer be
// used since the pixel addresses are no longer contiguous in memory
// note that the end points of the line must be on the screen

	UCHAR *start_offset; // starting memory offset of line

	int index, // loop index
		temp;  // used for temporary storage during swap


	// perform trivial rejections
	if (x > max_clip_x || x < min_clip_x)
		return;

	// make sure y2 > y1
	if (y1>y2)
	{
		temp = y1;
		y1   = y2;
		y2   = temp;
	} // end swap

	// perform trivial rejections
	if (y1 > max_clip_y || y2 < min_clip_y)
		return;

	// now clip
	y1 = ((y1 < min_clip_y) ? min_clip_y : y1);
	y2 = ((y2 > max_clip_y) ? max_clip_y : y2);

	// compute starting position
	start_offset = vbuffer + (y1*lpitch) + x;

	// draw line one pixel at a time
	for (index=0; index<=y2-y1; index++)
    {
		// set the pixel
		*start_offset = (UCHAR)color;

		// move downward to next line
		start_offset+=lpitch;

    } // end for index

} // end VLine

//--------------------------------------------------|

void HLine(int x1,int x2,int y,int color, UCHAR *vbuffer, int lpitch)
{
// draw a horizontal line using the memset function

	int temp; // used for temporary storage during endpoint swap

	// perform trivial rejections
	if (y > max_clip_y || y < min_clip_y)
		return;

	// sort x1 and x2, so that x2 > x1
	if (x1>x2)
	{
		temp = x1;
		x1   = x2;
		x2   = temp;
	} // end swap

	// perform trivial rejections
	if (x1 > max_clip_x || x2 < min_clip_x)
		return;

	// now clip
	x1 = ((x1 < min_clip_x) ? min_clip_x : x1);
	x2 = ((x2 > max_clip_x) ? max_clip_x : x2);

	// draw the row of pixels
	memset((UCHAR *)(vbuffer+(y*lpitch)+x1),
		(UCHAR)color,x2-x1+1);

} // end HLine

//--------------------------------------------------|

void VLine16(int y1,int y2,int x,int color,UCHAR *vbuffer, int lpitch)
{
// draw a vertical line, note that a memset function can no longer be
// used since the pixel addresses are no longer contiguous in memory
// note that the end points of the line must be on the screen

	USHORT *start_offset; // starting memory offset of line

	int index, // loop index
		temp;  // used for temporary storage during swap

	// convert lpitch to number of words
	lpitch = lpitch >> 1;

	// perform trivial rejections
	if (x > max_clip_x || x < min_clip_x)
		return;

	// make sure y2 > y1
	if (y1>y2)
	{
		temp = y1;
		y1   = y2;
		y2   = temp;
	} // end swap

	// perform trivial rejections
	if (y1 > max_clip_y || y2 < min_clip_y)
		return;

	// now clip
	y1 = ((y1 < min_clip_y) ? min_clip_y : y1);
	y2 = ((y2 > max_clip_y) ? max_clip_y : y2);

	// compute starting position
	start_offset = (USHORT *)vbuffer + (y1*lpitch) + x;

	// draw line one pixel at a time
	for (index=0; index<=y2-y1; index++)
    {
		// set the pixel
		*start_offset = color;

		// move downward to next line
		start_offset+=lpitch;

    } // end for index

} // end VLine16

//--------------------------------------------------|

void HLine16(int x1,int x2,int y,int color, UCHAR *vbuffer, int lpitch)
{
// draw a horizontal line using the memset function

	int temp; // used for temporary storage during endpoint swap

	USHORT *vbuffer2 = (USHORT *)vbuffer; // short pointer to buffer

	// convert pitch to words
	lpitch = lpitch >> 1;

	// perform trivial rejections
	if (y > max_clip_y || y < min_clip_y)
		return;

	// sort x1 and x2, so that x2 > x1
	if (x1>x2)
	{
		temp = x1;
		x1   = x2;
		x2   = temp;
	} // end swap

	// perform trivial rejections
	if (x1 > max_clip_x || x2 < min_clip_x)
		return;

	// now clip
	x1 = ((x1 < min_clip_x) ? min_clip_x : x1);
	x2 = ((x2 > max_clip_x) ? max_clip_x : x2);

	// draw the row of pixels
	Mem_Set_WORD((vbuffer2+(y*lpitch)+x1), color,x2-x1+1);

} // end HLine16

//--------------------------------------------------|

int Fast_Distance_2D(int x, int y)
{
// this function computes the distance from 0,0 to x,y with 3.5% error

	// first compute the absolute value of x,y
	x = abs(x);
	y = abs(y);

	// compute the minimum of x,y
	int mn = MIN(x,y);

	// return the distance
	return(x+y-(mn>>1)-(mn>>2)+(mn>>4));

} // end Fast_Distance_2D

//--------------------------------------------------|

float Fast_Distance_3D(float fx, float fy, float fz)
{
// this function computes the distance from the origin to x,y,z

	int temp;  // used for swaping
	int x,y,z; // used for algorithm

	// make sure values are all positive
	x = fabs(fx) * 1024;
	y = fabs(fy) * 1024;
	z = fabs(fz) * 1024;

	// sort values
	if (y < x) SWAP(x,y,temp)
	
	if (z < y) SWAP(y,z,temp)

	if (y < x) SWAP(x,y,temp)

	int dist = (z + 11*(y >> 5) + (x >> 2) );

	// compute distance with 8% error
	return((float)(dist >> 10));

} // end Fast_Distance_3D

//--------------------------------------------------|

inline void Mem_Set_WORD(void *dest, USHORT data, int count)
{
// this function fills or sets unsigned 16-bit aligned memory
// count is number of words

	_asm 
	{ 
		mov edi, dest   ; edi points to destination memory
		mov ecx, count  ; number of 16-bit words to move
		mov ax,  data   ; 16-bit data
		rep stosw       ; move data
    } // end asm
 
} // end Mem_Set_WORD

//--------------------------------------------------|

inline void Mem_Set_QUAD(void *dest, UINT data, int count)
{
// this function fills or sets unsigned 32-bit aligned memory
// count is number of quads

_asm 
    { 
    mov edi, dest   ; edi points to destination memory
    mov ecx, count  ; number of 32-bit words to move
    mov eax, data   ; 32-bit data
    rep stosd       ; move data
    } // end asm

} // end Mem_Set_QUAD

//--------------------------------------------------|