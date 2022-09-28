//-----------------------------------------------------------------------------------------|
// PROJECT TITLE:		The Lazarus Project
//
// FILE NAME:			OGeez.h
// EXE NAME:			Optical Geez
// 
// PROGRAM DESC:		This is the main entry point of our OCR application. It uses Win32
//						function (API) calls to get the job done.
//
// DATE CREATED:		8th of Oct 2017, Saturday (aka the hand of fate)
// LAST UPDATE:
//
// PROGRAM AUTHOR:		Dr. Rediet Worku aka Code Red
//-----------------------------------------------------------------------------------------|
#define WIN32_LEAN_AND_MEAN			// just no MFC

//-----------------------------------------------------------------------------------------|
// DEFINES
//-----------------------------------------------------------------------------------------|

// these define the window properties

// window name and class name
#define CLASS_NAME		"My_Class"
#define WINDOW_NAME		"Optical Geez"

// dimensions of window
#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768

// starting points of the window
#define WINDOW_X		( (GetSystemMetrics(SM_CXSCREEN) >> 1) - (WINDOW_WIDTH >> 1) )
#define WINDOW_Y		( (GetSystemMetrics(SM_CYSCREEN) >> 1) - (WINDOW_HEIGHT >> 1) )

// these are the drawing modes used
#define DRAW_NONE		-1
#define DRAW_ALL		0
#define DRAW_SEGMENT	1
#define DRAW_LINE		2
#define DRAW_BOUNDS		3

// constant defining our edit control
#define EDIT_ID			1

// this is the number of trainer files
#define NUM_FILES		8

//-----------------------------------------------------------------------------------------|
// TYPEDEFS
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// INCLUDES
//-----------------------------------------------------------------------------------------|
#include <Windows.h>
#include <CommDlg.h>		// open & save dialog boxes

#include <stdio.h>
#include <malloc.h>
#include <math.h>

// the resource's header (contains icons, menus, and stuff...)
#include "resource.h"

// include in our cutsom made libraries
#include "Image.h"			// handle's the loading/unloading of bitmaps and image files
#include "My_OCR.h"			// OCR libraries; semgmentation, filtering and all
#include "Lazarus.h"		// The recognizer the modifed Network
#include "Geez_Unicode.h"	// contains a table of UNICODE Geez definitions

//-----------------------------------------------------------------------------------------|
// GLOBALS/EXTERNS
//-----------------------------------------------------------------------------------------|

// these globals refer to the application descriptor and main window handle respectively
HINSTANCE main_instance		= NULL;
HWND main_window_handle		= NULL;
HWND edit_handle			= NULL;

// global used for file name storage
char filename[MAX_PATH]		= { 0 };

// general buffers used for text printing
char a_text[MAX_PATH]			= { 0 };		// ASCII enocded text buffer
wchar_t w_text[NUM_PATTERNS]	= { 0 };		// UNICODE encoded text buffer used for Geez display

// this indicates the drawing mode
int draw_mode = DRAW_NONE;

// these are the bitmap files containing our training datas
char *training_files[NUM_FILES] = { "Trainer_Sets\\Nyala2.bmp", "Trainer_Sets\\PowerGeez1(low)2.bmp", 
	"Trainer_Sets\\Ethiopic(small).bmp", "Trainer_Sets\\PGU1.bmp", "Trainer_Sets\\PGU2.bmp", "Trainer_Sets\\VisualGeez.bmp", 
	"Trainer_Sets\\alphabetmono.bmp", "Trainer_Sets\\Mar_Handwriting.bmp" };
int training_data[NUM_PATTERNS][PATTERN_SPACE] = { 0 };		// an empty list of training vectors one per
							// each amharic fidel

// these are input handler variables
int input[PATTERN_SPACE];	// holds our character one at a time
int total_fids=0;			// the total of fidels in doc
wchar_t *unicodes = NULL;

// shows the state of the edit box
bool edit_on = false;

//-----------------------------------------------------------------------------------------|
// PROTOTYPES
//-----------------------------------------------------------------------------------------|

LRESULT CALLBACK Main_Window_Proc(HWND, UINT, WPARAM, LPARAM);

// sampling functions
void Nearest_Neighbour(int *vo, int *vi, int w, int h);

// drawing functions
void Draw_Buffer_Slowpod(HDC hdc, UCHAR *buffer, const int width, const int height);
void Draw_Segments(HDC hdc);
void Draw_Learning(HDC hdc, int *buf, int pattern);
int Display_Formatted_Text(void);

// input functions for recognizer's
int Learn_From_File(int training_data[][PATTERN_SPACE], char *filename);
int Load_Input(int *in_data, int x, int y, int w, int h);

// utilities
int Open_File_Dlg(void);

//-----------------------------------------------------------------------------------------|
// FUNCTIONS
//-----------------------------------------------------------------------------------------|

// this is our main entry point
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
	// Window Structures and Message Structures respectively
	WNDCLASSEX win;
	MSG msg;

	// save the application descritor, Vlad Pirogov's Style
	main_instance = hInst;

	// fill up the window structure
	win.cbClsExtra		= 0;									// extra parms
	win.cbSize			= sizeof(win);							// size of this class
	win.cbWndExtra		= 0;									// extra-window params
	win.hbrBackground	= CreateSolidBrush(RGB(150,150,150));	// the window brush color
	win.hCursor			= LoadCursor(NULL, IDC_ARROW);			// the window cursor
	win.hIcon			= LoadIcon(NULL, IDI_APPLICATION);		// the window icon showing in desktop
	win.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);		// the small window icon on system tray
	win.hInstance		= hInst;								// the app descriptor
	win.lpfnWndProc		= Main_Window_Proc;						// the main window event handler
	win.lpszClassName	= CLASS_NAME;							// the window class name
	win.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);			// the menu resource name
	win.style			= 0;									// the window style; we don't need it here

	// up next register the window class on the OS
	if (!RegisterClassEx(&win))
		return(0);

	// now create a window of the registered class
	main_window_handle = CreateWindowEx(0,						// the unnecessary window style
										CLASS_NAME,				// the class name
										WINDOW_NAME,			// the window name
										WS_OVERLAPPEDWINDOW,	// the window style
										WINDOW_X,				// starting points for drawing
										WINDOW_Y,
										WINDOW_WIDTH,			// window dimensions
										WINDOW_HEIGHT,
										NULL,					// handle to a parent window
										NULL,					// menu handle
										main_instance,			// the app-descriptor
										NULL);					// extra-params; we probably won't use for life

	// test for a valid handle
	if (!main_window_handle)
		return(0);			// no window

	// now update the drawing region and show the window
	ShowWindow(main_window_handle, nShowCmd);
	UpdateWindow(main_window_handle);

	int k = 0;
	// we are now ready for the two step rule of message proccesor
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// translate any key messages
		TranslateMessage(&msg);

		// dispatch this message, to our handler Main_Window_Proc
		DispatchMessage(&msg);

	} // end while there is messge

	// return to OS
	return((int)msg.wParam);

} // end WinMain

//-----------------------------------------------------------------------------------------|

LRESULT CALLBACK Main_Window_Proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
// this is our window event handler - handles all the events my OCR project needs; aka
//	the Lazarus Project by Dr. Red the Code Red

	// locals used
	int i;

	// what's the message?
	switch(msg)
	{
	case WM_CREATE:				// must be when the window is initializing
		{

			return(0);
		} break;

	case WM_CLOSE:				// sent when user is clicking close button
		{
			// unload bitmaps
			Unload_Bitmap(&bitmap);

			// close the OCR engine if still acive
			Shutdown_OCR();

			// kill our window
			DestroyWindow(hwnd);

			return(0);
		} break;

	case WM_DESTROY:			// the window is about to close; do clean up's
		{

			// send the WM_QUIT message
			PostQuitMessage(0);

			// back to dispatcher, the OS
			return(0);
		} break;

	case WM_SIZE:				// sent when maximizing or whatevea
		{
			RECT rc;
			GetClientRect(hwnd, &rc);

			// adjust the edit control size if already there
			if (edit_handle)
				SetWindowPos(edit_handle, NULL, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);

			return(0);
		} break;

	case WM_CHAR:
		{
			// test for an edit control
			if (edit_handle)
			{
				wchar_t text[2];
				switch (wparam)
				{
				case 65:
					{
					// process the a key
						text[0] = 0x12A0;
						text[1] = 0;
						
						// return's the number of text's in the edit control
						int len = GetWindowTextLength(edit_handle);

						// put it at the last spot 
						SendMessageW(edit_handle, EM_SETSEL, (WPARAM)len, (LPARAM)len);

						// add this text to our edit control
						SendMessageW(edit_handle, EM_REPLACESEL, 0, (LPARAM)text);

					} break;

				default:
					MessageBox(hwnd, "No More Amarz", "Info", MB_ICONINFORMATION);
				} // end switch

			} // end if

			// success
			return(0);
		} break;

	case WM_PAINT:				// this is sent when our app need's to re-draw
		{
			HDC hdc;		// a device-context? An abstraction of components s.a. display drivers by the OS
			PAINTSTRUCT ps;	// needed for drawing
			RECT rc;
			GetClientRect(hwnd, &rc);

			// get the dc
			hdc = BeginPaint(hwnd, &ps);

			// clear the rect
			FillRect(hdc, &rc, CreateSolidBrush(RGB(150,150,150)));

			// test which to draw based on the nature of edit control
			if (!edit_handle)
			{
				if (draw_mode == DRAW_ALL)
					Draw_Buffer_Slowpod(hdc, binary_buffer, buffer_width, buffer_height);
					
				if (draw_mode == DRAW_SEGMENT)
					Draw_Segments(hdc);

				if (draw_mode == DRAW_LINE)
					Draw_Line_Project(binary_buffer, buffer_width, hdc);

				if (draw_mode == DRAW_BOUNDS)
					Draw_All_Segments(binary_buffer, buffer_width, hdc);

			} // end if edit on

			// now free the dc
			EndPaint(hwnd, &ps);

			// back to OS then to our message handler thru get message
			return(0);
		} break;

		// proccess any command button or menu option selections here

	case WM_COMMAND:		// sent when user clicks a menu item or command button
		{
			switch(LOWORD(wparam))
			{
			case ID_FILE_EXIT:			// it's the exit button, close the window
				{
					PostMessage(hwnd, WM_CLOSE, 0, 0);	// close the window
				} break;

			case ID_FILE_OPEN40001:
				{
					if (Open_File_Dlg())
					{
						// draw only if there is no edit control
						if (!edit_handle)
						{
							// get the DC
							HDC hdc = GetDC(hwnd);

							// draw the bitmap on window
							Draw_Buffer_Slowpod(hdc, binary_buffer, buffer_width, buffer_height);

							// set the draing mode
							draw_mode = DRAW_ALL;

							// now release the DC
							ReleaseDC(hwnd, hdc);

						} // end if

					} // end if openfilename

				} break;

			case ID_DEMOS_SEGMENT:
				{
					// test if buffer contains anything valid
					if (binary_buffer)
					{
						// intialize the OCR engine
						Init_OCR(buffer_width, buffer_height);

						// beign the segmentation step
						// cut 'em up in lines
						Line_Project(binary_buffer, buffer_width, buffer_height);

						// cut 'em up in they width
						Extract_Fidel(binary_buffer, buffer_width, buffer_height);

						// trim any left over's
						Post_Segment(binary_buffer, buffer_width, buffer_height);

						// draw if there is no edit
						if (!edit_handle)
						{
							// get dc
							HDC hdc = GetDC(hwnd);

							// draw the buffer
							Draw_Segments(hdc);

							// relase the DC
							ReleaseDC(hwnd,hdc);
						} // end if draw

						draw_mode = DRAW_SEGMENT;

					} // end if
					else
					{
						MessageBox(hwnd, "Please open an image file first!", "Info", MB_ICONINFORMATION);
					} // end else
					
				} break;	// end segmnet button/mneu item

			case ID_DEMOS_LINESEGMENT:
				{
					// this is sent during clicking of line segment
					// do this only if there is no edit control
					if (!edit_handle && binary_buffer)
					{
						// init the OCR engine
						Init_OCR(buffer_width, buffer_height);

						// perform line segmentation and draw
						Line_Project(binary_buffer, buffer_width, buffer_height);

						// now draw it
						HDC hdc = GetDC(hwnd);

						Draw_Line_Project(binary_buffer, buffer_width, hdc);

						// release dc
						ReleaseDC(hwnd, hdc);

						// set drawing state
						draw_mode = DRAW_LINE;

					} // end if
					else
					{
						if (!binary_buffer)
							MessageBox(hwnd, "Please open an image file first!", "Info", MB_ICONINFORMATION);
					} // end else

				} break;

			case ID_DEMOS_CHARACTERBOUNDS:
				{
					// draw's the charcater bounding boxes
					// do this only if no edit control and binary buffer is initalized
					if (!edit_handle && binary_buffer)
					{
						// init OCR
						Init_OCR(buffer_width, buffer_height);

						// do line seg
						Line_Project(binary_buffer, buffer_width, buffer_height);

						// now character bounds
						Extract_Fidel(binary_buffer, buffer_width, buffer_height);

						// now draw it
						HDC hdc = GetDC(hwnd);

						Draw_All_Segments(binary_buffer, buffer_width, hdc);

						// relase
						ReleaseDC(hwnd, hdc);

						draw_mode = DRAW_BOUNDS;

					} // end if
					else
					{
						if (!binary_buffer)
							MessageBox(hwnd, "Please open an image file first!", "Info", MB_ICONINFORMATION);
					} // end else

				} break;

			case ID_DEMOS_T:	// this message is sent when user presses the train button
				{
					// set draiwing mode to none, this is a training session, require's
					//	a special drawing routine
					draw_mode = DRAW_NONE;

					// they're all locked and loaded

					// for each training file found in your vicinity learn, it!!!
					for (i = 0; i < NUM_FILES; i++)
					{
						// load the next file to learn from
						Load_Bitmap_File(&bitmap, training_files[i]);

						// convert the encoded monochrome to expanded integer form
						//	i.e. save it in our local binary_buffer, makes things easy
						Bitmap_Monochrome(&bitmap);	// basically initalizes our, binary_buffer

						// Initalize the OCR enngine built by the Code Red - Dr. Rediet Worku
						Init_OCR(buffer_width, buffer_height);

						// now segement the binary_buffer using our three step segmentation
						//	functions from My_OCR.h lib
						Line_Project(binary_buffer, buffer_width, buffer_height);	// finds text lines
						Extract_Fidel(binary_buffer, buffer_width, buffer_height);	// basically finds the vertical bounds of fidels
						Post_Segment(binary_buffer, buffer_width, buffer_height);	// trims off any extra-spaces, fine-tunes the box to fit each
							// indiviual fidel.

						// now that we've got the preliminaries, we can begin training our net
						Learn_From_File(training_data, training_files[i]);

						int loop_me = 0;
						while (loop_me < 2)
						{
							// now fire up the taininer
							Train(training_data);
							++loop_me;
						} // end while

						// release the binary buffer
						free(binary_buffer);
						binary_buffer = NULL;

						// close Red's OCR engine
						Shutdown_OCR();

					} // end for training files

					// we're done, inform!
					MessageBox(hwnd, "Darth Vader speaks: I See you have constructed a new Light Saber. Your skills are complete! Indeed you are powerful as the Emperor has foreseen.", 
						"Info", MB_ICONINFORMATION);

				} break; //*/

			case ID_DEMOS_TEST:
				{
					// we use this to test for our classifer's ablity to
					//	learn
					if (!binary_buffer)
					{
						RECT rc;					// used for acquiring the client window size
						
						// open up a file, bring up the file dialog
						//Open_File_Dlg();

						// get the size of our current client window
						GetClientRect(hwnd, &rc);

						// up next create an edit window control that's the size of our client
						//	but only if there was no edit control
						if (!edit_handle)
						{
							edit_handle = CreateWindowW(L"EDIT", NULL, 
								ES_AUTOVSCROLL | WS_HSCROLL | ES_MULTILINE | WS_VSCROLL | WS_VISIBLE | ES_LEFT | WS_CHILD,
								0, 0, rc.right, rc.bottom, hwnd, NULL, main_instance, NULL);

							// test for valid handle
							if (!edit_handle)
								return(0);
						} // end if

						// intialize the OCR engine
						//Init_OCR(buffer_width, buffer_height);

						// beign the segmentation step
						// cut 'em up in lines
						//Line_Project(binary_buffer, buffer_width, buffer_height);

						// cut 'em up in they width
						//Extract_Fidel(binary_buffer, buffer_width, buffer_height);

						// trim any left over's
						//Post_Segment(binary_buffer, buffer_width, buffer_height);

						// clear the edit control
						SendMessage(edit_handle, WM_SETTEXT, 0, 0);

						// display the texts
						Display_Formatted_Text();

						// no longer need our buffer free it
						free(binary_buffer);
						binary_buffer = NULL;

						// close the OCR engine
						Shutdown_OCR();

					} // end if
					else
						MessageBox(hwnd, "The buffer seems to be busy!", "Info", MB_ICONINFORMATION);
				} break; //*/

			} // end inner switch of WM_COMMAND

			// back to OSs
			return(0);
		} break; // 

	default:				// let the OS do the remaining stuff for us -- like it does any!
		return(DefWindowProc(hwnd,msg,wparam,lparam));

	} // end switch

} // end Main_Window_Proc

//-----------------------------------------------------------------------------------------|

void Draw_Buffer_Slowpod(HDC hdc, UCHAR *buffer, const int width, const int height)
{
// this function draw's the content's of the binary buffer, regardless of it position starting
//	from coordinate (0,0) of our window to the width and height specified.

	RECT rc;		// used for filling

	// get the current window dimensions
	GetClientRect(main_window_handle, &rc);

	// now fill it up with a white-brush
	FillRect(hdc, &rc, CreateSolidBrush(RGB(150,150,150)));	

	// now print binary picture, since background is black in our case
	//	reverse them
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (buffer[j+i*width])
				SetPixel(hdc, j, i, 0);
			else
				SetPixel(hdc, j, i, RGB(255, 255, 255));
		} // end for

	} // end for i

} // end Draw_Buffer_Slowpod

//-----------------------------------------------------------------------------------------|

void Draw_Segments(HDC hdc)
{
// this function draw's the segmented texts on the screen as they appear on the document

	// clearr region
	RECT rc;
	GetClientRect(main_window_handle, &rc);
	FillRect(hdc, &rc, CreateSolidBrush(RGB(150,150,150)));

	// loop thru all the lines
	for (int line = 0; line < num_lines; line++)
	{
		// go thru each fidel in this line
		for (int fidel = 0; fidel < num_chars[line]; fidel++)
		{

			// loop thru each row again
			for (int k = high[line][fidel]; k < low[line][fidel]; k++)
			{
				for (int z = left[line][fidel]; z < right[line][fidel]; z++)
				{
					if (binary_buffer[z+k*buffer_width])
						SetPixel(hdc, z, k, RGB(0, 0, 0));
					else
						SetPixel(hdc, z, k, RGB(255,255,255));
				} // end for z

			} // end for k

		} // end for fidel

	} // end for line

} // end Draw_Segments

//-----------------------------------------------------------------------------------------|

int Learn_From_File(int training_data[][PATTERN_SPACE], char *filename)
{
// this function load's the input from file

	int pattern = 0;		// the current pattern

	// we're also drawing get the device-context
	HDC hdc = GetDC(main_window_handle);

	// print the current file name and everything
	sprintf(a_text, "I am now learning from file: %s", filename);

	// set transparnet bkmode and Golden text color
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 201, 14));

	// now print the text
	TextOut(hdc, 0, 36, a_text, strlen(a_text));

	// loop thru every supposed line
	for (int line = 0; line < num_lines; line++)
	{
		// loop thru each fidel
		for (int fidel = 0; fidel < num_chars[line]; fidel++)
		{
			// clear the memory of training data
			memset(training_data[pattern], 0, sizeof(int) * PATTERN_SPACE);

			// load the next Fidel for training
			Load_Input(training_data[pattern], left[line][fidel], high[line][fidel], 
				right[line][fidel], low[line][fidel]);

			// dispaly the process of learning on screen for me
			Draw_Learning(hdc, training_data[pattern], pattern);

			pattern++;	// update the pattern count
			//Sleep(500);
		} // end for fidel

	} // end for line

	// print the current file name and everything
	sprintf(a_text, "I've finsihed learning from file: %s", filename);

	// set transparnet bkmode and Golden text color
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 201, 14));

	// now print the text
	TextOut(hdc, 0, 36, a_text, strlen(a_text));

	// done with drawing relase the DC
	ReleaseDC(main_window_handle, hdc);

	// success
	return(1);

} // end Learn_From_File

//-----------------------------------------------------------------------------------------|

void Nearest_Neighbour(int *vo, int *vi, int w, int h)
{
// this function re-samples the vector in param 2 into param 1 by using the nearset-neighbour
//	alogorithim, which basically duplicates values during scaling up or throws away
//	information when scaling-down.

	// these are the scaling factors of our bitmaps
	double x_ratio = ((double)w / (double)WIDTH);
	double y_ratio = ((double)h / (double)HEIGHT);
	double px, py;

	// now loop and do the nearst neighbour computation
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			px = floor((double)j * x_ratio);
			py = floor((double)i * y_ratio);
			vo[i*WIDTH+j] = vi[(int)((py*w)+px)];
		} // end for

	} // end for i

} // end nearest_neighbour

//-----------------------------------------------------------------------------------------|

void Draw_Learning(HDC hdc, int *buf, int pattern)
{
// this function simply display's the process of learning in sequencial manner

	RECT rc;		// used for filling the client window

	// get the current client window size
	GetClientRect(main_window_handle, &rc);

	// fill-it up with a gray brush of medium intensity
	FillRect(hdc, &rc, CreateSolidBrush(RGB(150,150,150)));

	// now loop thru the entire dimensions of buf and
	//	display its contents according to our convention; i.e. 0-black; 1-white
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			// test the current pixel value
			if (buf[j+i*WIDTH])	// is white
				SetPixel(hdc, j, i, RGB(0,0,0));		// print black
			else
				SetPixel(hdc, j, i, RGB(255,255,255));	// print white since is black

		} // end for j width

	} // end for i

	// now print some more infomation and all
	sprintf(a_text, "Current Pattern: %d\n", pattern);

	// now set the background mode to transparent and text color
	//	to red
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255,0,0));

	// now print the text using textout(...) api
	TextOut(hdc, 0, 52, a_text, strlen(a_text));

	// we're done!!!

} // end Draw_Learning

//-----------------------------------------------------------------------------------------|

int Load_Input(int *in_data, int x1, int y1, int x2, int y2)
{
// this function load's the next character in the squence on the passed buffer.

	int *copy_cat = NULL;		// temporary buffer used for storage
	int w = 1 + (x2 - x1);		// dimensions of vector
	int h = 1 + (y2 - y1);

	int i,j,k,l;				// looping vars

	// reject false calls
	if (!binary_buffer)
		return(0);

	// now allocate memory for the copy cat based on the dimensions passed
	if ( !(copy_cat = (int *)malloc(sizeof(int) * h * w)) )
		return(0);

	// loop thru the dimensions of the binary buffer, starting from
	//	the positions specified by params 2 & 3 to positions in param 4 & 5.
	for (i = y1, k = 0; i <= y2; i++, k++)
	{
		for (j = x1, l = 0; j <= x2; j++, l++)
			copy_cat[l+k*w] = (int)binary_buffer[j+i*buffer_width];

	} // end for i,k rows

	// finally scale the image to fit our recognizer'z input space
	//	using Nearest_Neighbour Interpolation technique.
	Nearest_Neighbour(in_data, copy_cat, w, h);

	// free the copy cat 
	free(copy_cat);
	copy_cat = NULL;

	// success
	return(1);

} // end Load_Input

//-----------------------------------------------------------------------------------------|

int Display_Formatted_Text(void)
{
// display's the text's sent by the unicode param to a more formatted style on the
//	edit window, i.e. it prints results

	// test for false calls
	if (!edit_handle)
		return(0);

	/*// loop thru each line
	for (int line = 0; line < num_lines; line++)
	{
		// allocate mem for this line's fidels
		unicodes = (wchar_t *)realloc(unicodes, sizeof(wchar_t) * num_chars[line]+3);

		// test
		if (!unicodes)
			return(0);

		// now loop thru each fidel
		for (int fidel = 0; fidel < num_chars[line]; fidel++)
		{
			// clear the input buffer
			memset(input, 0, sizeof(int) * PATTERN_SPACE);

			// get the next fidel into our classifers input
			Load_Input(input, left[line][fidel], high[line][fidel],
				right[line][fidel], low[line][fidel]);

			// now run the classifier for this input
			unicodes[fidel] = amharic_unicodes[Run_Classifer(input)];
		} // end for fidel

		// add the null in the end of this line
		unicodes[num_chars[line]]	= 0x000D;	// carrage return
		unicodes[num_chars[line]+1] = 0x000A;	// line feed
		unicodes[num_chars[line]+2] = 0;		// Null terminiator		

		// return's the number of text's in the edit control
		int len = GetWindowTextLength(edit_handle);

		// put it at the last spot 
		SendMessageW(edit_handle, EM_SETSEL, (WPARAM)len, (LPARAM)len);

		// add this text to our edit control
		SendMessageW(edit_handle, EM_REPLACESEL, 0, (LPARAM)unicodes);

		// free the unicode buffer
		//free(unicodes);

	} // end for*/

	short fidels[10] = { 0 };

	// loop and print chars
	for (int i = 4608, k = 0; i < 4880; i++, k++)
	{
		//for (int k = 0; k < 10; k ++)
		{
			//if (k < 7)
				fidels[k] = i;
				if (k == 9)
					k = 0;
			/*else
				if (k == 7)
					fidels[k] = 0x0D;
				else
				{
					if (k == 8)
						fidels[k] = 0x0A;
					else
					{
						fidels[k] = 0;
						k = 0;
					} // end else else*/
				//}
		} // end for

		int len = GetWindowTextLength(edit_handle);

		SendMessageW(edit_handle, EM_SETSEL, (WPARAM)len, (LPARAM)len);

		// add this text to our edit control
		SendMessageW(edit_handle, EM_REPLACESEL, 0, (LPARAM)fidels);
	} // end for

	// success
	return(1);

} // end Display_Formatted_Text

//-----------------------------------------------------------------------------------------|

int Open_File_Dlg(void)
{
// this function brings up the open file dialog from CommDlg.dll

	// bring's up the openfile dialog box
	OPENFILENAME ofn;

	// fill in the openfile strcture to open bitmaps
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= NULL;	
	ofn.hInstance			= GetModuleHandle(NULL);
	ofn.lpstrFilter			= "Bitmaps\0*.bmp\0";
	ofn.lpstrFile			= filename;
	ofn.nMaxFile			= MAX_PATH;
	ofn.nFilterIndex		= 1;
	ofn.Flags				= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		// file has been opened do processing 
		if (Load_Bitmap_File(&bitmap, filename) == 1)
		{
			// intialize the monochorme array
			Bitmap_Monochrome(&bitmap);
		} // end if normal

		return(1);	// success
	} // end if

	// fail
	return(0);

} // end Open_File_Dlg

//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|nmmmmmmmmmmmmmmmmmmmmmmmm