//--------------------------------------------------|
// T3DLIB2.H -- Game Engine Input Header			|
//--------------------------------------------------|

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

#include "T3DLIB.H"
#include "T3DLIB2.H"

//--------------------------------------------------|
// DEFINES
//--------------------------------------------------|

//--------------------------------------------------|
// MACROS
//--------------------------------------------------|

//--------------------------------------------------|
// TYPES
//--------------------------------------------------|

//--------------------------------------------------|
// PRTOTYPES
//--------------------------------------------------|

//--------------------------------------------------|
// EXTERNS
//--------------------------------------------------|

// directinput globals
LPDIRECTINPUT8				lpdi		= NULL;			
LPDIRECTINPUTDEVICE8		lpdikey		= NULL;
LPDIRECTINPUTDEVICE8		lpdimouse	= NULL;
LPDIRECTINPUTDEVICE8		lpdijoy		= NULL;
GUID						joystickGUID;
char						joystickname[80];

// these contain target records for all di input devices
UCHAR keyboard_state[256];		// contains keyboard state table
DIMOUSESTATE mouse_state;		// contains the mouse states
DIJOYSTATE joy_state;			// contains the joystick states
int joystick_found;				// tracks if joustick is plugged in

//--------------------------------------------------|
// FUNCTIONS
//--------------------------------------------------|

int DInput_Init(void)
{
// Initalizes a direct input

	if (FAILED(DirectInput8Create(main_instance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void **)&lpdi, NULL)))
		return(0);

	// success
	return(1);

} // end DInput_Init

//--------------------------------------------------|

void DInput_Shutdown(void)
{
// shut's down direct input

	if (lpdi)
		lpdi->Release();

} // end shutdown

//--------------------------------------------------|

//int DInput_Init_Joystick(int min_x=-256, int max_x=256,
//						 int min_y=-256, int max_y=256, int dead_band=10);

//--------------------------------------------------|

int DInput_Init_Mouse(void)
{
// initalizes the mouse

	// create the mouse
	if (FAILED(lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL)))
		return(0);

	// set the cooperation level
	if (lpdimouse->SetCooperativeLevel(main_window_handle,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return(0);

	// set the data format
	if (lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		return(0);

	// acquire the mouse
	if (lpdimouse->Acquire() != DI_OK)
		return(0);

	// return success
	return(1);

} // DInput_Init_Mouse

//--------------------------------------------------|

int DInput_Init_Keyboard(void)
{
// initalizes the keyboard device

	// create the keyboard device
	if (lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL) != DI_OK)
		return(0);

	// set the cooperation level
	if (lpdikey->SetCooperativeLevel(main_window_handle,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return(0);

	// set the data format
	if (lpdikey->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		return(0);

	// acquire the keyboard device
	if (lpdikey->Acquire() != DI_OK)
		return(0);

	// success
	return(0);

} // end DInput_Init_Keyboard

//--------------------------------------------------|

//int DInput_Read_Joystick(void);

//--------------------------------------------------|

int DInput_Read_Mouse(void)
{
// Read's the state of the mouse

	if (lpdimouse)
	{
		if (lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state) != DI_OK)
			return(0);
	} // end if ldpimouse
	else
	{
		// mouse isn't plugged in zero-out state
		memset(&mouse_state, 0, sizeof(DIMOUSESTATE));

		return(0);
	} // end else

	// success
	return(1);

} // end DInput_Read_Mouse

//--------------------------------------------------|

int DInput_Read_Keyboard(void)
{
// read's the keyboard state

	if (lpdikey)
	{
		if (lpdikey->GetDeviceState(256, (LPVOID)keyboard_state) != DI_OK)
			return(0);
	}
	else
	{
		// keyboard isn't plugged in, but not on my laptop
		//	zero it out
		memset(keyboard_state, 0, 256);

		return(0);
	} // end else

	// success
	return(0);

} // end DInput_Read_Keyboard

//--------------------------------------------------|

//void DInput_Release_Joystick(void);

//--------------------------------------------------|

void DInput_Release_Mouse(void)
{
// unacquire's and relases the mouse

	if (lpdimouse)
	{
		lpdimouse->Unacquire();
		lpdimouse->Release();
	} // end if

} // end DInput_Relase_MOuse

//--------------------------------------------------|

void DInput_Release_Keyboard(void)
{
// unacquire's and relases the keyboard

	if (lpdikey)
	{
		lpdikey->Unacquire();
		lpdikey->Release();
	} // end if

} // end DInput_Release_Keyboard

//--------------------------------------------------|