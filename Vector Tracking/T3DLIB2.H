//--------------------------------------------------|
// T3DLIB2.H -- Game Engine Input Header			|
//--------------------------------------------------|
#ifndef T3DLIB2_H
#define T3DLIB2_H

//--------------------------------------------------|
// INCLUDES
//--------------------------------------------------|

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

int DInput_Init(void);
void DInput_Shutdown(void);

int DInput_Init_Joystick(int min_x=-256, int max_x=256,
						 int min_y=-256, int max_y=256, int dead_band=10);

int DInput_Init_Mouse(void);
int DInput_Init_Keyboard(void);
int DInput_Read_Joystick(void);
int DInput_Read_Mouse(void);
int DInput_Read_Keyboard(void);
void DInput_Release_Joystick(void);
void DInput_Release_Mouse(void);
void DInput_Release_Keyboard(void);

//--------------------------------------------------|
// EXTERNS
//--------------------------------------------------|

extern HWND			main_window_handle;		// save the window handle
extern HINSTANCE	main_instance;			// save the instance

// directinput globals
extern LPDIRECTINPUT8			lpdi;			
extern LPDIRECTINPUTDEVICE8		lpdikey;
extern LPDIRECTINPUTDEVICE8		lpdimouse;
extern LPDIRECTINPUTDEVICE8		lpdijoy;
extern GUID						joystickGUID;
extern char						joystickname[80];

// these contain target records for all di input devices
extern UCHAR keyboard_state[256];		// contains keyboard state table
extern DIMOUSESTATE mouse_state;		// contains the mouse states
extern DIJOYSTATE joy_state;			// contains the joystick states
extern int joystick_found;				// tracks if joustick is plugged in

#endif