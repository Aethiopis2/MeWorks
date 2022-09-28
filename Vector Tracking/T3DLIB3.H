//--------------------------------------------------|
// T3DLIB3.H -- Game Engine Sounds and Music Header	|
//--------------------------------------------------|
#ifndef T3DLIB3_H
#define T3DLIB3_H

//--------------------------------------------------|
// INCLUDES
//--------------------------------------------------|

//--------------------------------------------------|
// DEFINES
//--------------------------------------------------|

#define DM_NUM_SEGMENTS		64		// number of segments for a MIDI sequence

#define	MIDI_NULL			0		// this MIDI is not loaded
#define MIDI_LOADED			1		// MIDI loaded
#define MIDI_PLAYING		2		// this MIDI is loaded and playing
#define MIDI_STOPPED		3		// this MIDI is loaded, but stopped
#define MAX_SOUNDS			256		// max number of sounds in the system at once

// digital sound object state defines
#define SOUND_NULL			0
#define SOUND_LOADED		1
#define SOUND_PLAYING		2
#define SOUND_STOPPED		3

// directX 7.0 compatablitiy
#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME)
#endif

//--------------------------------------------------|
// MACROS
//--------------------------------------------------|

#define DSVOLUME_TO_DB(volume)	((DWORD)(-30*(100-volume)))
#define MULTI_TO_WIDE(x,y)		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, y, -1, x, _MAX_PATH);

//--------------------------------------------------|
// TYPES
//--------------------------------------------------|

// this holds a single sound
typedef struct pcm_sound_typ
{
	LPDIRECTSOUNDBUFFER dsbuffer;	// the ds buffer contaning the sound
	int state;						// state of sound
	int rate;						// playback rate
	int size;						// size of sound
	int id;							// id number of sound
} pcm_sound, *pcm_sound_ptr;

// this structure holds a MIDI segment
typedef struct DMUSIC_MIDI_TYP
{
	IDirectMusicSegment			*dm_segment;	// dmusic segment
	IDirectMusicSegmentState	*dm_segstate;	// segment state
	int							id;				// the id of this segment
	int							state;			// state of MIDI song

} DMUSIC_MIDI,	*DMUSIC_MIDI_PTR;

//--------------------------------------------------|
// PRTOTYPES
//--------------------------------------------------|

// direct sound
int DSound_Load_WAV(char *filename, int control_flags = DSBCAPS_CTRLDEFAULT);
int DSound_Replicate_Sound(int source_id);
int DSound_Play(int id, int flags=0, int volume=0, int rate=0, int pan=0);
int DSound_Stop_Sound(int id);
int DSound_Stop_All_Sounds(void);
int DSound_Init(void);
int DSound_Shutdown(void);
int DSound_Delete_Sound(int id);
int DSound_Delete_All_Sounds(void);
int DSound_Status_Sound(int id);
int DSound_Set_Volume(int id, int vol);
int DSound_Set_Freq(int id, int freq);
int DSound_Set_Pan(int id, int pan);

// direct music
int DMusic_Load_MIDI(char *filename);
int DMusic_Play(int id);
int DMusic_Stop(int id);
int DMusic_Shutdown(void);
int DMusic_Delete_MIDI(int id);
int DMusic_Delete_All_MIDI(void);
int DMusic_Status_MIDI(int id);
int DMusic_Init(void);

//--------------------------------------------------|
// EXTERNS
//--------------------------------------------------|

extern HWND	main_window_handle;								// save the window handle
extern HINSTANCE main_instance;								// save the instance handle

extern LPDIRECTSOUND			lpds;						// direct sound
extern LPDIRECTSOUNDBUFFER		lpdsprimary;				// primary sound buffer
extern DSBUFFERDESC				dsbd;						// sound buffer
extern DSCAPS					dscaps;						// sound HW capablities
extern HRESULT					dsresult;					// holds results for direct sound calls
extern DSBCAPS					dsbcaps;					// buffer caps
extern WAVEFORMATEX				pcmwf;						// Wave format struct
extern pcm_sound				sound_fx[MAX_SOUNDS];		// array of secondary sound buffers

// Direct Music
extern IDirectMusicPerformance	*dm_perf;					// the performance object
extern IDirectMusicLoader		*dm_loader;					// the loader
extern DMUSIC_MIDI				dm_midi[DM_NUM_SEGMENTS];	// holds all direct music MIDI objects
extern int						dm_active_id;				// currently active midi segment

#endif