//-----------------------------------------------------------------------------------------|
// MainSource.cpp -- Program entry point for iKon application used for converting bitmap 
//	files to icon files.
//
// By: Code Red
//
// Date created:	1st of April 2018, Sunday (Fool's day).
// Last update:		
//-----------------------------------------------------------------------------------------|
#define WIN32_LEAN_AND_MEAN

//-----------------------------------------------------------------------------------------|
// DEFINES
//-----------------------------------------------------------------------------------------|
/* ** 
 * these define window properties 
 * */
#define CLASS_NAME			"iKonv1.0"
#define WINDOW_NAME			"iKon v1.0"
#define WINDOW_WIDTH		800
#define WINDOW_HEIGHT		480

/* **
 * these define app specifics
 * */
// buffer sizes
#define TEXT_BUFFER_SIZE	256

//-----------------------------------------------------------------------------------------|
// INCLUDES
//-----------------------------------------------------------------------------------------|
#include <Windows.h>
#include <CommDlg.h>
#include <stdio.h>

#include "iKon.h"
#include "resource.h"

//-----------------------------------------------------------------------------------------|
// MACROS
//-----------------------------------------------------------------------------------------|
/* **
 * this displays an error message box with NULL handle
 * */
#define ERR_BOX(m) MessageBox(NULL, m, "Error Box", MB_ICONERROR)

//-----------------------------------------------------------------------------------------|
// GLOBALS
//-----------------------------------------------------------------------------------------|
/* **
 * basic win32 stuff
 * */
HINSTANCE main_instance	= NULL;
HWND main_window_handle = NULL;

/* **
 * app specifics
 * */
// these hold window starting positions
const unsigned int window_x = ( (GetSystemMetrics(SM_CXSCREEN) >> 1) - (WINDOW_WIDTH >> 1) );
const unsigned int window_y = ( (GetSystemMetrics(SM_CYSCREEN) >> 1) - (WINDOW_HEIGHT >> 1) );

char text_buffer[TEXT_BUFFER_SIZE];
char file_path[MAX_PATH];

bool drawing = false;		// current state of drawing

// this is the converter class
iKon ikon;

//-----------------------------------------------------------------------------------------|
// PROTOTYPES
//-----------------------------------------------------------------------------------------|

//-----------------------------------------------------------------------------------------|
// FUNCTIONS
//-----------------------------------------------------------------------------------------|
// main window handler
LRESULT CALLBACK Main_Window_Proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// locals
	HDC hdc;	
	PAINTSTRUCT ps;

	// decode message
	switch(msg)
	{
	case WM_CREATE:
		{
			// good
			return(0);
		} break;

	case WM_COMMAND:
		{
			// which menu button
			switch (LOWORD(wparam))
			{
			case ID_MENU_EXIT:
				{
					PostMessage(hwnd, WM_DESTROY, 0, 0);
				} break;

			case ID_MENU_CONVERTBITMAP:
				{
					OPENFILENAME ofn;

					// open up a bitmap file
					// fill in the openfile strcture to open bitmaps
					ZeroMemory(&ofn, sizeof(ofn));

					ofn.lStructSize			= sizeof(ofn);
					ofn.hwndOwner			= NULL;					// no window is console app
					ofn.hInstance			= main_instance;
					ofn.lpstrFilter			= "Bitmaps\0*.bmp\0";
					ofn.lpstrFile			= file_path;
					ofn.nMaxFile			= MAX_PATH;
					ofn.nFilterIndex		= 1;
					ofn.Flags				= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					// get the file path and name if opend
					if (GetOpenFileName(&ofn))
					{
						// draw on screen
						HDC hdc = GetDC(hwnd);

						// set drawing mode
						drawing = true;

						// a true, draw bitmap on screen
						ikon.Bitmap_To_Icon(file_path, hwnd, hdc);

						ReleaseDC(hwnd, hdc);
					} // end if

				} break;

			} // end switch 2

			// good
			return(0);
		} break;

	case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);

			if (drawing)
				ikon.Draw_Bitmap(file_path, hwnd, hdc);

			EndPaint(hwnd, &ps);

			// good
			return(0);
		} break;

	case WM_SIZE:
		{
			HDC hdc = GetDC(hwnd);

			// test and draw
			if (drawing)
				ikon.Draw_Bitmap(file_path, hwnd, hdc);

			ReleaseDC(hwnd, hdc);

			return(0);
		} break;

	case WM_DESTROY:
		{
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			
			// good
			return(0);
		} break;

		// alas...
	default: 
		return(DefWindowProc(hwnd, msg, wparam, lparam));

	} // end switch

} // end Main_Window_Proc

//-----------------------------------------------------------------------------------------|
// Program entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	// locals
	WNDCLASSEX win;
	MSG msg;

	// fill up win structure
	ZeroMemory( &win, sizeof(win) );
	win.cbSize				= sizeof(win);
	win.hbrBackground		= CreateSolidBrush(RGB(0,0,0));
	win.hCursor				= LoadCursor(NULL, IDC_ARROW);
	win.hIcon				= LoadIcon(NULL, IDI_APPLICATION);
	win.hIconSm				= LoadIcon(NULL, IDI_APPLICATION);
	win.hInstance			= hInstance;
	win.lpfnWndProc			= Main_Window_Proc;
	win.lpszClassName		= CLASS_NAME;
	win.lpszMenuName		= MAKEINTRESOURCE(IDR_MENU1);

	// register the win class
	if (!RegisterClassEx(&win))
	{
		sprintf(text_buffer, "I'm sorry but, 'RegisterClass()' retured: %d.", 
			GetLastError());
		ERR_BOX(text_buffer);
		return(0);
	} // end if

	// create window of registered class
	main_window_handle = CreateWindowEx(0, CLASS_NAME, WINDOW_NAME, 
		WS_OVERLAPPEDWINDOW,
		window_x, window_y,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL, NULL, hInstance, NULL);

	// test
	if (!main_window_handle)
	{
		sprintf(text_buffer, "I'm sorry but, 'CreateWindowEx()' returned: %d.",
			GetLastError());
		ERR_BOX(text_buffer);
		return(0);
	} // end if

	// save the app descriptor while at it 
	main_instance = hInstance;

	ShowWindow(main_window_handle, nCmdShow);
	UpdateWindow(main_window_handle);

	// main message loop
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		// the two step rule
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end while

	// back to OZ
	return(0);

} // end winmain

//-----------------------------------------------------------------------------------------|
// END OF FILE
//-----------------------------------------------------------------------------------------|