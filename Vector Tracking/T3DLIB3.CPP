//--------------------------------------------------|
// T3DLIB3.H -- Game Engine Sounds and Music Header	|
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

#include "T3DLIB3.H"

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
// GLOBALS
//--------------------------------------------------|

LPDIRECTSOUND			lpds			= NULL;		// direct sound
LPDIRECTSOUNDBUFFER		lpdsprimary		= NULL;		// primary sound buffer
DSBUFFERDESC			dsbd;						// sound buffer
DSCAPS					dscaps;						// sound HW capablities
HRESULT					dsresult;					// holds results for direct sound calls
DSBCAPS					dsbcaps;					// buffer caps
WAVEFORMATEX			pcmwf;						// Wave format struct
pcm_sound				sound_fx[MAX_SOUNDS];		// array of secondary sound buffers

// Direct Music
IDirectMusicPerformance	*dm_perf		= NULL;		// the performance object
IDirectMusicLoader		*dm_loader		= NULL;		// the loader
DMUSIC_MIDI				dm_midi[DM_NUM_SEGMENTS];	// holds all direct music MIDI objects
int						dm_active_id;				// currently active midi segment

//--------------------------------------------------|
// FUNCTIONS
//--------------------------------------------------|

int DSound_Load_WAV(char *filename, int control_flags)
{
// this function loads a .wav file, set up the directsound
//	buffer and loads the data into memory, the function returns
//	the id member of the sound

	HMMIO			hwav;		// handle to wave file
	MMCKINFO		parent,		// parent chunk
					child;		// child chunk
	WAVEFORMATEX	wfmtx;		// wave format structure

	int				sound_id = -1,	// id of sound to be loaded
					index;			// looping var

	UCHAR			*snd_buffer,			// temprary sound buffer to hold voc data
					*audio_ptr_1 = NULL,	// data ptr to first write buffer
					*audio_ptr_2 = NULL;	// data ptr to second write buffer

	DWORD			audio_length_1 = 0,		// length of the first write buffer
					audio_length_2 = 0;		// length of the second write buffer

	// step one: are there any open id's?
	for (index = 0; index < MAX_SOUNDS; index++)
	{
		// make sure this sound is unsued
		if (sound_fx[index].state == SOUND_NULL)
		{
			sound_id = index;
			break;
		} // end if

	} // end for index

	// did we get a free id
	if (sound_id == -1)
		return(-1);

	// set up chunk info structure
	parent.ckid			= (FOURCC)0;
	parent.cksize		= 0;
	parent.fccType		= (FOURCC)0;
	parent.dwDataOffset	= 0;
	parent.dwFlags		= 0;

	// copy data
	child = parent;

	// open the wav file
	if ((hwav = mmioOpen(filename, NULL, MMIO_READ | MMIO_ALLOCBUF)) == NULL)
		return(-1);

	// desecnd into the RIFF
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if (mmioDescend(hwav,&parent, NULL, MMIO_FINDRIFF))
	{
		// close the file
		mmioClose(hwav, 0);

		// retuen error
		return(-1);
	} // end if

	// descend into WAVEfmt
	child.ckid = mmioFOURCC('f','m','t',' ');

	if (mmioDescend(hwav, &child, &parent, 0))
	{
		// close the file
		mmioClose(hwav, 0);

		// error
		return(-1);
	} // end if

	// now read the wave format information from file
	if (mmioRead(hwav, (char *)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx))
	{
		// close file
		mmioClose(hwav, 0);

		// error
		return(-1);
	} // end if

	// make sure the data format is PCM
	if (wfmtx.wFormatTag != WAVE_FORMAT_PCM)
	{
		// close file
		mmioClose(hwav, 0);

		// error
		return(-1);
	} // end if

	// now ascend up one level, so we can access data chunk
	if (mmioAscend(hwav, &child, 0))
	{
		// close file
		mmioClose(hwav, 0);

		// return error
		return(-1);
	} // end if

	// desend to the data chunk
	child.ckid = mmioFOURCC('d','a','t','a');

	if (mmioDescend(hwav, &child, &parent, MMIO_FINDCHUNK))
	{
		//close file
		mmioClose(hwav, 0);

		//error
		return(-1);
	} // end if

	// finally, all we have to do is read the data in and 
	//	set up the direct sound buffer

	// allocate mem to load sound data
	snd_buffer = (UCHAR *)malloc(child.cksize);

	// read the wave data
	mmioRead(hwav, (char *)snd_buffer, child.cksize);

	// close the file
	mmioClose(hwav, 0);

	// set rate and size in data structure
	sound_fx[sound_id].rate = wfmtx.nSamplesPerSec;
	sound_fx[sound_id].size = child.cksize;
	sound_fx[sound_id].state = SOUND_LOADED;
	
	// set up the format data structure
	memset(&pcmwf, 0, sizeof(WAVEFORMATEX));

	pcmwf.wFormatTag = WAVE_FORMAT_PCM; // pulse code modulation
	pcmwf.nChannels = 1; // mono
	pcmwf.nSamplesPerSec = 11025; // always this rate
	pcmwf.nBlockAlign = 1;
	pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
	pcmwf.wBitsPerSample = 8;
	pcmwf.cbSize = 0;

	// prepare to create sounds buffer
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = control_flags | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes = child.cksize;
	dsbd.lpwfxFormat = &pcmwf;

	// create the sound buffer
	if (lpds->CreateSoundBuffer(&dsbd,&sound_fx[sound_id].dsbuffer,NULL)!=DS_OK)
	{
		// release memory
		free(snd_buffer);

		// return error
		return(-1);
	} // end if

	// copy data into sound buffer
	if (sound_fx[sound_id].dsbuffer->Lock(0,
		child.cksize,(void **) &audio_ptr_1,
		&audio_length_1,
		(void **)&audio_ptr_2,
		&audio_length_2,DSBLOCK_FROMWRITECURSOR)!=DS_OK)
		return(0);

	// copy first section of circular buffer
	memcpy(audio_ptr_1, snd_buffer, audio_length_1);

	// copy last section of circular buffer
	memcpy(audio_ptr_2, (snd_buffer+audio_length_1),audio_length_2);

	// unlock the buffer
	if (sound_fx[sound_id].dsbuffer->Unlock(audio_ptr_1,
		audio_length_1,
		audio_ptr_2,
		audio_length_2)!=DS_OK)
		return(0);
	
	// release the temp buffer
	free(snd_buffer);

	// return id
	return(sound_id);

} // end DSound_Load_WAV

//--------------------------------------------------|

int DSound_Replicate_Sound(int source_id)
{
// this function replicates the sent sound and sends back the
//	id of the replicated sound, you would use this function
//	to make multiple copies of gunshot or something that you want
//	play multiple times simulataneously, but you only 
//	want to load once

	if (source_id != -1)
	{
		// duplicate the sound buffer
		//	first hunt for an open id
		for (int id = 0; id < MAX_SOUNDS; id++)
		{
			// is this sound open
			if (sound_fx[id].state == SOUND_NULL)
			{
				// first make an identical copy
				sound_fx[id] = sound_fx[source_id];

				// now replicate the directsound buffer
				if (FAILED(lpds->DuplicateSoundBuffer(sound_fx[source_id].dsbuffer,
					&sound_fx[id].dsbuffer)))
				{
					// reset sound to NULL
					sound_fx[id].dsbuffer	= NULL;
					sound_fx[id].state		= SOUND_NULL;

					// return error 
					return(-1);
				} // end if

				// set id up
				sound_fx[id].id = id;

				// return duplicate id
				return(id);

			} // end if found
		} // end for id
	} // end if
	else
		return(-1);

	//return faliure
	return(-1);

} // end Replicate_Sound

//--------------------------------------------------|

int DSound_Init(void)
{
// this function intializes the sound system

	static int first_time = 1;	// used to track if it's the first time entered

	// test for the very first time
	if (first_time)
	{
		// clear everything out
		memset(sound_fx, 0, sizeof(pcm_sound)*MAX_SOUNDS);

		// reset the first time
		first_time = 0;

		// create a direct sound object
		if (FAILED(DirectSoundCreate(NULL, &lpds, NULL)))
			return(0);

		// set cooperative level
		if (FAILED(lpds->SetCooperativeLevel(main_window_handle, DSSCL_NORMAL)))
			return(0);

	} // end if first time

	// intialize the sound_fx array
	for (int index = 0; index < MAX_SOUNDS; index++)
	{
		// test if this sound has been loaded
		if (sound_fx[index].dsbuffer)
		{
			// stop the sound
			sound_fx[index].dsbuffer->Stop();

			// release the buffer
			sound_fx[index].dsbuffer->Release();
		} // end if loaded buffer

		// clear the record out
		memset(&sound_fx[index], 0, sizeof(pcm_sound));

		//  now set up the feilds
		sound_fx[index].id		= index;
		sound_fx[index].state	= SOUND_NULL;
	} // end for index

	return(1);	// success

} // end DSound_Init

//--------------------------------------------------|

int DSound_Shutdown(void)
{
// shut's down the sound system and relases resources

	// first turn all sounds off
	DSound_Stop_All_Sounds();

	// now release all sound buffers
	for (int index = 0; index < MAX_SOUNDS; index++)
		if (sound_fx[index].dsbuffer)
			sound_fx[index].dsbuffer->Release();

	// now shut the main sound 
	if (lpds)
		lpds->Release();

	return(1);	// success

} // end DSound_Shutdown

//--------------------------------------------------|

int DSound_Play(int id, int flags, int volume, int rate, int pan)
{
// this function plays a sound

	if (sound_fx[id].dsbuffer)
	{
		// reset position to start
		if (FAILED(sound_fx[id].dsbuffer->SetCurrentPosition(0)))
			return(0);

		// play sound
		if (FAILED(sound_fx[id].dsbuffer->Play(0,0,flags)))
			return(0);
	} // end if

	return(1);

} // end DSound_Play

//--------------------------------------------------|

int DSound_Set_Volume(int id, int vol)
{
// this function sets the volume on a sound 0-100

	if (sound_fx[id].dsbuffer->SetVolume(DSVOLUME_TO_DB(vol)) != DS_OK)
		return(0);

	// returns success
	return(1);
} // end DSound_Set_Volume

//--------------------------------------------------|

int DSound_Set_Freq(int id, int freq)
{
// set's the playback rate

	if (sound_fx[id].dsbuffer->SetFrequency(freq) != DS_OK)
		return(0);

	return(1);

} // end Dsound_set freq

//--------------------------------------------------|

int DSound_Set_Pan(int id, int pan)
{
// this function set's the pan, -10,000 - 10,000

	if (sound_fx[id].dsbuffer->SetPan(pan) != DS_OK)
		return(0);

	return(1);

} // end DSound_Set_pan

//--------------------------------------------------|

int DSound_Stop_Sound(int id)
{
// stops the sound if it's currently playing

	if (sound_fx[id].dsbuffer)
	{
		sound_fx[id].dsbuffer->Stop();
		sound_fx[id].dsbuffer->SetCurrentPosition(0);
	} // end if

	return(1);

} // DSound_Stop_Sound

//--------------------------------------------------|

int DSound_Delete_All_Sounds(void)
{
// this function deletes all sounds

	for (int index = 0; index < MAX_SOUNDS; index++)
		DSound_Delete_Sound(index);

	return(1);

} // end Delete_All_Sounds

//--------------------------------------------------|

int DSound_Delete_Sound(int id)
{
// this function delete a single sound

	// first stop it
	if (!DSound_Stop_Sound(id))
		return(0);

	// now delete it
	if (sound_fx[id].dsbuffer)
	{
		// release 
		sound_fx[id].dsbuffer->Release();
		sound_fx[id].dsbuffer = NULL;

	} // end if

	return(1);

} // end DSound_Delete_Sound

//--------------------------------------------------|

int DSound_Stop_All_Sounds(void)
{
//  stop's all sounds 

	for (int index=0; index < MAX_SOUNDS; index++)
		DSound_Stop_Sound(index);

	return(1);

} // end DSound Stop all sounds

//--------------------------------------------------|

int DSound_Status_Sound(int id)
{
//  returns the status of a sound

	if (sound_fx[id].dsbuffer)
	{
		ULONG status;

		// get the status
		sound_fx[id].dsbuffer->GetStatus(&status);

		// returns status
		return(status);
	} // end if
	else
		return(-1);	// failure

} // end DSound_Status_Sound

//--------------------------------------------------|

int DMusic_Load_MIDI(char *filename)
{
// this function loads a MIDI segment

	DMUS_OBJECTDESC	ObjDesc;
	HRESULT	hr;
	IDirectMusicSegment *pSegment = NULL;

	int index;	// looping var

	// look for open slot for MIDI segment
	int id = -1;

	for (index = 0; index < DM_NUM_SEGMENTS; index++)
	{
		if (dm_midi[index].state == MIDI_NULL)
		{
			// validate id
			id = index;
			break;
		} // end if

	} // end for index

	// found good id?
	if (id == -1)
		return(-1);

	// get current working directory
	char	szDir[_MAX_PATH];
	WCHAR	wszDir[_MAX_PATH];

	if (_getcwd(szDir, _MAX_PATH) == NULL)
		return(-1);

	MULTI_TO_WIDE(wszDir, szDir);

	// tell the loader where to look for files
	hr = dm_loader->SetSearchDirectory(GUID_DirectMusicAllTypes, wszDir, FALSE);

	if (FAILED(hr))
		return(-1);

	// convert file name to wide string
	WCHAR wfilename[_MAX_PATH];
	MULTI_TO_WIDE(wfilename, filename);

	// set up object description
	memset(&ObjDesc, 0, sizeof(DMUS_OBJECTDESC));
	ObjDesc.dwSize		= sizeof(DMUS_OBJECTDESC);
	ObjDesc.guidClass	= CLSID_DirectMusicSegment;
	wcscpy(ObjDesc.wszFileName, wfilename);
	ObjDesc.dwValidData	= DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;

	// load the object and query it for the IDirectMusic interface
	hr = dm_loader->GetObject(&ObjDesc, IID_IDirectMusicSegment, (void**)&pSegment);

	if (FAILED(hr))
		return(-1);

	// ensure that the segment plays a standard MIDI file
	// you need to set a parameter in the bandtrack
	hr = pSegment->SetParam(GUID_StandardMIDIFile, -1, 0, 0, (void *)dm_perf);

	if (FAILED(hr))
		return(-1);

	// This step is necessary because DirectMusic handles program changes and
	// bank selects differently for standard MIDI files than it does for MIDI
	// content authored specifically for DirectMusic.
	// The GUID_StandardMIDIFile parameter must
	// be set before the instruments are downloaded.

	// The next step is to download the instruments.
	// This is necessary even for playing a simple MIDI file
	// because the default software synthesizer needs the DLS data
	// for the General MIDI instrument set
	// If you skip this step, the MIDI file will play silently.
	// Again, you call SetParam on the segment,
	// this time specifying the GUID_Download parameter:

	hr = pSegment->SetParam(GUID_Download, -1, 0, 0, (void *)dm_perf);

	if (FAILED(hr))
		return(-1);

	// at this point we have a MIDI loaded and a valid object
	dm_midi[id].dm_segment	= pSegment;
	dm_midi[id].dm_segstate	= NULL;
	dm_midi[id].state		= MIDI_LOADED;
	dm_midi[id].id			= id;

	return(id);

} // end DMusic_Load_MIDI

//--------------------------------------------------|

int DMusic_Play(int id)
{
// play the loaded MIDI music based on id

	if (dm_midi[id].dm_segment && dm_midi[id].state != MIDI_NULL)
	{
		// if there is an active midi then stop it
		if (dm_active_id != -1)
			DMusic_Stop(dm_active_id);

		// play the segment and force tracking of state variable
		dm_perf->PlaySegment(dm_midi[id].dm_segment,0,0, &dm_midi[id].dm_segstate);
		dm_midi[id].state = MIDI_PLAYING;

		// set the active midi segment
		dm_active_id = id;

		return(1);
	} // end iif
	else
		return(0);

} // end DMusic_Play

//--------------------------------------------------|

int DMusic_Stop(int id)
{
// stop the current MIDI track

	if (dm_midi[id].dm_segment && dm_midi[id].state != MIDI_NULL)
	{
		// stop
		dm_perf->Stop(dm_midi[id].dm_segment, NULL, 0, 0);
		dm_midi[id].state = MIDI_STOPPED;

		// reset active id
		dm_active_id = -1;

		return(1);
	} // end if
	else
		return(0);

} // end DMusic_Stop

//--------------------------------------------------|

int DMusic_Delete_MIDI(int id)
{
// delete's one MIDI segment

	if (dm_midi[id].dm_segment)
	{
		dm_midi[id].dm_segment->SetParam(GUID_Unload, -1, 0, 0, NULL);

		// relase stuffs and reset
		dm_midi[id].dm_segment->Release();
		dm_midi[id].dm_segstate	= NULL;
		dm_midi[id].dm_segment	= NULL;
		dm_midi[id].state		= MIDI_NULL;

	} // end if

	return(1);

} // end DMusic_Delete_MIDI

//--------------------------------------------------|

int DMusic_Delete_All_MIDI(void)
{
// delete's all midi's using calls to DMusic_Delete_MIDI()
//	to delete one midi channel at a time

	for (int index = 0; index < DM_NUM_SEGMENTS; index++)
		DMusic_Delete_MIDI(index);

	return(1);

} // end Delete_All_MIDI

//--------------------------------------------------|

int DMusic_Status_MIDI(int id)
{
// htis checks the statis of the MIDI segment

	if (dm_midi[id].dm_segment && dm_midi[id].state != MIDI_NULL)
	{
		// get the status and translate to our defines
		if (dm_perf->IsPlaying(dm_midi[id].dm_segment, NULL) == S_OK)
			dm_midi[id].state = MIDI_PLAYING;
		else
			dm_midi[id].state = MIDI_STOPPED;

		return(dm_midi[id].state);
	} // end if
	else
		return(0);

} // end DMusic_Statis_MIDI

//--------------------------------------------------|

int DMusic_Init(void)
{
// this function intializes directmusic, it also checks if directsound has
//	been initialized, if so it connect the wave output to directsound, otherwise\
//	it create's it's own directsound object, hencce you must start directsound
//	up first if you want to used both directsound and directmusic

	int index;	// looping var

	// set up directmusic
	// intialize com
	if (FAILED(CoInitialize(NULL)))
		return(0);		// faild to start COM stuff up

	// create the performance
	if (FAILED(CoCreateInstance(CLSID_DirectMusicPerformance,
		NULL,
		CLSCTX_INPROC,
		IID_IDirectMusicPerformance,
		(void **)&dm_perf)))
	{
		return(0);
	} // end if

	// intialize the performance, 
	if (FAILED(dm_perf->Init(NULL, lpds, main_window_handle)))
		return(0);

	// add port to performance
	if (FAILED(dm_perf->AddPort(NULL)))
		return(0);

	// create the loader to load objects such as midi files
	if (FAILED(CoCreateInstance(CLSID_DirectMusicLoader,
		NULL,
		CLSCTX_INPROC,
		IID_IDirectMusicLoader,
		(void **)&dm_loader)))
		return(0);

	// reset all the midi and segment objects
	for (index = 0; index < DM_NUM_SEGMENTS; index++)
	{
		dm_midi[index].dm_segment	= NULL;
		dm_midi[index].dm_segstate	= NULL;
		dm_midi[index].state		= MIDI_NULL;
		dm_midi[index].id			= index;

	} // end for

	// reset active id
	dm_active_id = -1;

	// all good baby
	return(1);

} // end DMusic_Init

//--------------------------------------------------|

int DMusic_Shutdown(void)
{
// closes all instruments down

	if (dm_perf)
		dm_perf->Stop(NULL, NULL, 0, 0);

	// delete all MIDI's
	DMusic_Delete_All_MIDI();

	// close down and relase the performance object
	if (dm_perf)
	{
		dm_perf->CloseDown();
		dm_perf->Release();
	} // end if dm_perf

	// relase the loader object
	if (dm_loader)
		dm_loader->Release();

	// relase COM
	CoUninitialize();

	// succes
	return(1);

} // end DMusic_Shutdown

//--------------------------------------------------|
