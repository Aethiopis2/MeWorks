//----------------------------------------------------------------------------------------------|
// PROGRAM NAME:				Tone_Generator.exe
//
// FILE NAME:					Entry.cpp
//
// PROGRAM AUTHOR:				Code Red
//
// DATE CREATED:				26th of December 2016, Monday
// LAST UPDATE:
//
// DESCRIPTION:	This a simple console application that creates' a "*.wav" file that contains
//	pure tones generated at various frequencies using only mathematical models; i.e. the "*.wav"
//	file contains results of the equation y = A sin(nx); where A is desired amplitude, n is
//	the frequency is given by 2 * PI * f and x is the sound sample itself divided by the total sample.
//----------------------------------------------------------------------------------------------|
#include <Windows.h>
#include <mmsystem.h>

#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

//----------------------------------------------------------------------------------------------|
// STRUCTURES
//----------------------------------------------------------------------------------------------|
// this structure reprsents the header information found on every wave file in planet Earth.
typedef struct WAVE_CHUNK_TAG
{
	char type[4];		// the characters "RIFF"
	int file_size;		// the size of the entire file in bytes
	char id[4];			// the characters "WAVE"
	char fmt[4];		// the characters "fmt "
	int fmt_size;		// the size of the WAVEFORMATEX structure down below
} WAVE_CHUNK, *WAVE_CHUNK_PTR; // end 

// this structre is created again reprsents the information found next to WAVEFORMATEX structure
//	in a "*.wav" Wave file.
typedef struct SUB_CHUNK_TAG
{
	char data[4];		// the characters "data"
	int data_size;		// the size of the actual audio data in bytes
} SUB_CHUNK, SUB_CHUNK_PTR; // end

// typedefs
typedef struct RESONANT_SLOPES_TAG
{
	int count;				// store's the count of resonant frequiencies prsent in a signal
	int num_slopes;			// the number of total slope combination
	int *slopes;			// the slopes themselves
} RESONANT_SLOPES, *RESONANT_SLOPES_PTR;

//----------------------------------------------------------------------------------------------|
// PROTOTYPES
//----------------------------------------------------------------------------------------------|

int Save_Wave_File(char *filename, short *audio, int len);
int Synthesize_Sound(int frequency, int amplitude);
int My_Freq_Count(short *buffer, const int samples, int length);
int Fast_Open_Wave_File(char *filename);
int Beat_Detect(short *audio, int total_samples, int start);
void Test_Fun(short *audio, int total_samples);
//int DFT(int dir, int m,double *x1,double *y1);
int DFT(int N,float *x1, int *p);

bool Is_Power_Two(const unsigned int x);
unsigned int Bits_Needed(const unsigned int x);
unsigned int Reverse_Bits(unsigned int x, unsigned int bits);
int FFT(float *x, int N, int *p);
int Find_Resonant_Frequencies(const int *power_spectrum, const int size, int *resonant_frequency, int *resonant_location);
int Get_Resonant_Envelope(const int *power_spectrum, const int size, RESONANT_SLOPES_PTR speech_data);

//----------------------------------------------------------------------------------------------|
// GLOBALS
//----------------------------------------------------------------------------------------------|

// wave structures
WAVE_CHUNK		file_chunk;				// contains info on wave file header
WAVEFORMATEX	wft;						// contains info on waveform audio data
SUB_CHUNK		sub_chunk;					// contains the partition info on wave files
RESONANT_SLOPES my_speech;

// others
const double	PI				= 3.142;	// the mathematical pi
char			*audio_buffer	= NULL;		// stores the sythesized audio data

//----------------------------------------------------------------------------------------------|
// FUNCTIONS
//----------------------------------------------------------------------------------------------|
// Program entry point
int main()
{
	int key_press;				// holds the key press value of the user
	int freq;					// holds the frequency of a sound in hertz
	int amp;					// the amplitude of the sample
	char filename[MAX_PATH];	// stores file names

	printf("Tone Generator.exe ver 1.2\nCreated By: Dr. Rediet Worku!\n\n");

	do
	{
		printf("\nWhat would you like to do?\n-> Press 1 to synthesize pure tone\n");
		printf("-> Press 2 to open a \"*.wav\" file to study\n-> Press 3 to listen\n");
		printf("-> Press 0 to exit\n");

		// now get the key from the user
		key_press = _getch();

		// switch and run the correct function
		switch (key_press)
		{
		case 49:		// ASCII for 1
			{
				printf("\nAt what frequency (in hertz) shall I synthesize the sound: ");
				scanf("%d", &freq);
				printf("\nWhat shall it's amplitude be: ");
				scanf("%d", &amp);

				// call synthesize sound
				Synthesize_Sound(freq, amp);
			} break;

		case 50:		// ASCII for 2 meaning open in tabular form
			{
				printf("\nWhat file would you like me to open: ");
				scanf("%s", filename);

				// use open fast to read data into buffer
				if (!Fast_Open_Wave_File(filename))
					break;

				short *au = (short *)audio_buffer;

				float *a = (float *)malloc(sizeof(float)*sub_chunk.data_size);
				for (int i = 0; i < 32768; i++)
				{
					if (i < sub_chunk.data_size)
						a[i] = (float)au[i];
					else
						a[i] = 0.0;
				}

				int *p = (int *)malloc(sizeof(int)*32768);

				//int s = Beat_Detect((short *)audio_buffer, sub_chunk.data_size / 2, 0);
				//DFT(sub_chunk.data_size, a, p);
				//Get_Resonant_Envelope(p, sub_chunk.data_size>>1, &my_speech);

				FFT(a, 32768, p);
				Save_Wave_File("dft.wav", (short*)p, sizeof(int)*32768);

				//Test_Fun((short *)audio_buffer, sub_chunk.data_size / 2);

			} break;

		case 51:		// ASCII for 3 meaning listen 
			{
				printf("\nPlease type in the file name to play: ");
				scanf("%s", filename);

				// stop all previous sounds
				PlaySound(NULL, NULL, SND_PURGE);

				// open playsound function to play the sound
				if (!PlaySound(filename, NULL, SND_ASYNC | SND_NODEFAULT | SND_FILENAME))
					printf("\nUnable to playfile: %s\n", filename);

			} break;

		default: break;

		} // end switch

	} while (key_press != 48);		// 48 is ASCII for 0

	// exit main
	return(0);
} // end main

//----------------------------------------------------------------------------------------------|

int My_Freq_Count(short *buffer, const int samples, int length)
{
// calculates the frequency of the sound data by mere counting of the alteration b/n positive
//	and negative side of the y-axis.

	bool is_positive = true;		// keeps track of location
	int counter = 0;				//count's the curves
	int n = 0;

	// loop and count the frequency
	for (int i = 0; i < samples; i++)
	{
		if (buffer[i] >= 0 && is_positive)
		{
			++n;
			is_positive = false;
		} // end if
		else
		{
			if (buffer[i] < 0 && !is_positive)
			{
				++n;
				is_positive = true;
			}
		} // end else

		if (n < 2)
			++counter;
		else
			break;

	} // end for

	// convert to hertz and return; note lenght must be in seconds
	return(counter);

} // end My_Freq_Count

//----------------------------------------------------------------------------------------------|

int Synthesize_Sound(int frequency, int amplitude)
{
// this function synthesizes pure tones at the specified amilitude and frequency and writes the
//	info into a "*.wav" file for later playback.

	int length;						// stores audio data length in seconds
	char filename[MAX_PATH];		// file name

	// fill in the waveformatex structure
	wft.cbSize				= sizeof(wft);
	wft.nAvgBytesPerSec		= 22050;
	wft.nBlockAlign			= 2;				
	wft.nChannels			= 1;				// mono
	wft.nSamplesPerSec		= 22050;			// samples per second
	wft.wBitsPerSample		= 16;				// sample bits
	wft.wFormatTag			= WAVE_FORMAT_PCM;

	// prompt the user in audio data length
	printf("\nHow long (in seconds) should the audio data be: ");
	scanf("%d", &length);

	// calculate the length in bytes
	sub_chunk.data_size = wft.nSamplesPerSec * length * wft.nBlockAlign;
	int N = sub_chunk.data_size;

	// allocate memory for the buffer
	audio_buffer = (char *)malloc(sub_chunk.data_size);

	// test for success
	if (!audio_buffer)
	{
		printf("\nError: Unable to allocate memory for the buffer!\n\n");
		return(0);
	} // end if no buffer
	
	// now set and use an alias pointer to navigate through data in 16-bit mode
	short *buffer = (short *)audio_buffer;
	double p = PI/ 2;			// phase 90
	bool t = true;
	int a = 1000 + (rand()%amplitude);
	int s = 100;

	// now loop
	for (int sample = 0; sample < (sub_chunk.data_size / wft.nBlockAlign); sample++)
	{
		double t = (double)sample / wft.nSamplesPerSec;
		double x = PI * 2 * t * (double)frequency;
		double y = (PI*2) * t * 700;
		//double z = (PI*2) * t * 880;
		//double u = (PI*2) * t * 220;

		//buffer[sample] = amplitude * sin(x);
		//buffer[sample] += 5000 * sin(y);
		a = 2000;
		for (int k = 1; k < 6; k++)
		{
			x *= k;
			a /= k;
			buffer[sample]+= a*sin(x);
		}
	} // end for
	
	for (int i = 0; i < 4; i++)
		printf("%d\n", buffer[i]);

	// finally save this audio data into a file
	printf("\nWhat shall I call the file: ");
	scanf("%s", filename);

	// now write
	Save_Wave_File(filename, (short *)audio_buffer, sub_chunk.data_size);

	// free buffer
	free(audio_buffer);
	audio_buffer = NULL;

	// success
	return(1);

} // end Synthesize_Sound

//----------------------------------------------------------------------------------------------|

int Save_Wave_File(char *filename, short *audio, int len)
{
// saves the global "audio_buffer" as a standard "*.wav" file for later playback

	// open file
	int file_handle = _open(filename, _O_CREAT | _O_BINARY | _O_RDWR, _S_IWRITE | _S_IREAD);

	// fill up the file chunk and sub chunk structures
	memcpy(file_chunk.fmt, "fmt ", 4);
	memcpy(file_chunk.id, "WAVE", 4);
	memcpy(file_chunk.type, "RIFF", 4);
	memcpy(sub_chunk.data, "data", 4);
	file_chunk.file_size = sizeof(file_chunk)+sizeof(sub_chunk)+
		sizeof(wft)+len;
		
	sub_chunk.data_size = len;
	file_chunk.fmt_size = sizeof(wft);

	// now write
	_write(file_handle, &file_chunk, sizeof(file_chunk));
	_write(file_handle, &wft, sizeof(wft));
	_write(file_handle, &sub_chunk, sizeof(sub_chunk));

	// finally write the save_buffer
	_write(file_handle, audio, len);

	// once done close the file handle
	_close(file_handle);

	// exit
	return(1);

} // end Save_Wave_File

//----------------------------------------------------------------------------------------------|

int Fast_Open_Wave_File(char *filename)
{
// jumps and quickly opens a wavefile returns the data in the audio_buffer

	HANDLE file_handle = NULL;

	file_handle = CreateFile(filename,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// test handle
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		printf("\nError: Unable to open file: %s\n", filename);
		return(0);
	} // end if

	DWORD bytes_read = 0;

	// read in the header chunk
	ReadFile(file_handle, &file_chunk, sizeof(file_chunk), &bytes_read, NULL);

	// now test for a wave file
	if (memcmp(file_chunk.id, "WAVE", 4))
	{
		printf("\nError: %s, is not a wave file!\n", filename);
		return(0);
	} // end if

	// read WAVEFORMATEX
	ReadFile(file_handle, &wft, sizeof(wft), &bytes_read, NULL);

	// read the subchunk
	ReadFile(file_handle, &sub_chunk, sizeof(sub_chunk), &bytes_read, NULL);

	// allocate memory
	audio_buffer = (char *)malloc(sub_chunk.data_size);

	// test
	if (!audio_buffer)
	{
		printf("\nError: Unable to allocate memory!\n");
		return(0);
	} // end if*/

	// read data
	ReadFile(file_handle, audio_buffer, sub_chunk.data_size, &bytes_read, NULL);

	// close file
	CloseHandle(file_handle);

	// success
	return(1);

} // end Fast_Open_Wave_File

//----------------------------------------------------------------------------------------------|

int Beat_Detect(short *audio, int total_samples, int start)
{
// 
	
	const int NUM_BLOCKS = 512;							// divide's the sound data into a region
	int block_size = total_samples / NUM_BLOCKS;		// assuming block size is always less than or equal to total samples

	// so basically we view our sound data into a region of "NUM_BLOCKS", hence in each block
	//	we get total_samples / NUM_BLOCKS samples. Next we calculate the energy-level contained
	//	in each block by squaring and summing each sample in each block.
	int i_block, i_sample;					// looping var's
	float block_en[NUM_BLOCKS] = {0.0};		// the energy level contained in each block

	i_block = 0;
	while (i_block < NUM_BLOCKS)
	{
		// next sum up all the samples in this block and take the average
		for (i_sample = 0; i_sample < block_size; i_sample++)
			block_en[i_block] += (abs(audio[i_sample + i_block * block_size]));

		// now take the freaking average
		block_en[i_block] /= (float)block_size;

		// print value
		printf("Block: %d\tAverage: %f\n", i_block + 1, block_en[i_block]);

		// move to the next block
		++i_block;
	} // end while

	short *trimed = NULL;
	int counter = 0;
	int size;
	int i;
	// now trim the most relevant energy content save to file
	i_block = 0;
	i_sample = 0;
	float max = block_en[0];
	while (i_block < NUM_BLOCKS)
	{
		if (block_en[i_block] > max)
		{
			i_sample = i_block;
			max = block_en[i_block];
		}
		++i_block;

	} // end while

	// copy this into array
	trimed = new short [block_size];
	memcpy(trimed, audio + (i_sample * block_size), block_size);

	// write to file
	printf("\nSaving to file \"Temp1.wav\"...\n");

	Save_Wave_File("temp1.wav", trimed, block_size);
	return block_size;
} // end fundamentalist

//----------------------------------------------------------------------------------------------|

void Test_Fun(short *audio, int total_samples)
{
// testing functions

	// a periodic signal is defined by: x[n] = x[n+N]; for some N
	//	determine this point if there ever is...
	int N = 0;			// perodicity
	short *temp = (short *)malloc(sizeof(short)*22050);
	memcpy(temp, audio, sizeof(short)*22050);
	Save_Wave_File("super_temp.wav", temp, sizeof(short)*22050);
} 

//----------------------------------------------------------------------------------------------|

int DFT(int N,float *x1, int *p)
{
   int n, k;
   float angle = 2.0 * PI;

   float *x = (float *)malloc(N*sizeof(float));
   float *y = (float *)malloc(N*sizeof(float));
   p = (int *)malloc(N*sizeof(int));

   for (k = 0; k < N/2; k++)
   {
	   x[k] = 0.0;
	   y[k] = 0.0;
	   p[k] = 0;

	   for (n = 0; n < N; n++)
	   {
		   float complex = angle * k * ((float)n / N);
		   //printf("x[%d]: %f\n", n, x1[n]);

		   x[k] += (x1[n] * cos(complex));
		   y[k] += (x1[n] * sin(-complex));
	   } // end for

	   x[k] /= N;
	   y[k] /= N;

	   //printf("X[%d] = %f + i%f\n", k, x[k], y[k]);
	   //printf("Frequency in hertz: %f hz\n", (float)k * wft.nSamplesPerSec / N);
	   p[k] = (int)(x[k]*x[k] + y[k]*y[k]);
	   //printf("p[%d] = %d\n", k, p[k]);
   } // end for

   int max = p[0];
   int j=0;
   for (int i = 1; i < N/2; i++)
   {
	   if (p[i] > max)
	   {
		   max = p[i];
		   j = i;
	   }
   }

   printf("Possible frequency: %d hz\nPower rating at the frequency: %f db\n", j * (wft.nSamplesPerSec) / N, 10 *log((double)p[j]/12000000));

   // write to file
   //Save_Wave_File("dft.wav", (short*)p, N/2);
   free(x);
   free(y);
   free(p);

   return 0;
} //*/

/*int DFT(int dir,int m,double *x1,double *y1)
{
   long i,k;
   double arg;
   double cosarg,sinarg;
   double *x2=NULL,*y2=NULL;

   x2 = (double *)malloc(m*sizeof(double));
   y2 = (double *)malloc(m*sizeof(double));
   if (x2 == NULL || y2 == NULL)
      return(FALSE);

   for (i=0;i<m;i++) {
      x2[i] = 0;
      y2[i] = 0;
      arg = - dir * 2.0 * 3.141592654 * (double)i / (double)m;
      for (k=0;k<m;k++) {
         cosarg = cos(k * arg);
         sinarg = sin(k * arg);
         x2[i] += (x1[k] * cosarg - y1[k] * sinarg);
         y2[i] += (x1[k] * sinarg + y1[k] * cosarg);
      }
   }

   if (dir == 1) {
      for (i=0;i<m;i++) {
         x1[i] = x2[i] / (double)m;
         y1[i] = y2[i] / (double)m;
      }
   } else {
      for (i=0;i<m;i++) {
         x1[i] = x2[i];
         y1[i] = y2[i];
      }
   }

   free(x2);
   free(y2);
   return(TRUE);
} */

//----------------------------------------------------------------------------------------------|

bool Is_Power_Two(const unsigned int x)
{
// returns a true if the parameter is power of two.
	
	// no natural number less than 2 can be its power
	if (x < 2) return(false);

	// powers of two never return a true when "and"ed with
	//	the previous number
	if (x & x-1) return(false);

	// this number is a power of two
	return(true);

} // end Is_Power_Two

//----------------------------------------------------------------------------------------------|

unsigned int Bits_Needed(const unsigned int x)
{
// this function returns the number of bits needed to make the bit 
//	reversal

	int i;	// loop var

	// trivirally reject data
	if (x < 2)
		return(0);		// no bits needed algo dont work

	for (i = 0; ; i++)
		if (x & (1 << i)) return(i);

	// should'nt get here but
	return(0);
} // end Bits_Needed

//----------------------------------------------------------------------------------------------|

unsigned int Reverse_Bits(unsigned int x, unsigned int bits)
{
// returns the reverse bit order

	unsigned int i, rev;

	for (i=rev=0; i < bits; i++)
	{
		rev = (rev << 1) | (x & 1);
		x >>= 1;
	}

	return(rev);

} // end Reverse_Bits

//----------------------------------------------------------------------------------------------|

int FFT(float *x, int N, int *p)
{
// the fast fourier transform based on cooly-tukey's work

	unsigned int num_bits;		// the number of bits needed for bit reversal
	unsigned int i, j, k, n;
	unsigned int block_size, block_end;
	float tr, ti;				// temporary vars

	float angle_num = 2 * PI;		// pre-compute the result of 2PI
	
	// prepare memory for fourirer coffecients
	float *r = (float *)malloc(sizeof(float)*N);
	float *y = (float *)malloc(sizeof(float)*N);

	// test if the sample size is power of two
	if (!Is_Power_Two(N))
	{
		printf("Is not power of two!\n");
		return(0);
	} // end if

	// get the number of bits needed for bit reversals
	num_bits = Bits_Needed(N);

	// reverse the bits
	for (i = 0; i < N; i++)
	{
		j = Reverse_Bits(i, num_bits);
		r[j] = x[i];
		y[i] = 0.0;
	} // end for

	block_end = 1;
	for (block_size = 2; block_size <= N; block_size <<= 1)
	{
		float d_ang = angle_num / (float)block_size;
		float sm2 = sin(-2 * d_ang);
		float sm1 = sin(-d_ang);
		float cm2 = cos(-2 * d_ang);
		float cm1 = cos(-d_ang);
		float w = 2 * cm1;
		float ar[3], ai[3];

		for (i = 0; i < N; i += block_size)
		{
			ar[2] = cm2;
			ar[1] = cm1;

			ai[2] = sm2;
			ai[1] = sm1;

			for (j = i, n = 0; n < block_end; j++, n++)
			{
				ar[0] = w*ar[1] - ar[2];
				ar[2] = ar[1];
				ar[1] = ar[0];

				ai[0] = w*ai[1] - ai[2];
				ai[2] = ai[1];
				ai[1] = ai[0];

				k = j + block_end;
				tr = ar[0]*r[k] - ai[0]*y[k];
				ti = ar[0]*y[k] + ai[0]*r[k];

				r[k] = r[j] - tr;
				y[k] = y[j] - ti;

				r[j] += tr;
				y[j] += ti;
			} // end for
		} // end outer for

		block_end = block_size;

	} // end for

	// average
	for (i = 0; i < N; i++)
	{
		r[i] /= N;
		y[i] /= N;
	}

	// finally compute the power spectrum
	for (i = 0; i < N; i++)
		p[i] = r[i]*r[i]+y[i]*y[i];

	return(0);
}

int Find_Resonant_Frequencies(const int *power_spectrum, const int size, int *resonant_frequency, int *resonant_location)
{
// this function finds the strongest points in the power_spectrum (i.e. resonant frequencies) and 
//	return their values and count.

	bool marker1 = false, marker2 = false;			// used for marking points
	int count = 0;									// store's the total count of the resonants
	int *temp = NULL;								// temporary buffer
	int i = 1;										// loop var

	// allocate memory for the temorary buffer
	temp = (int *)malloc(sizeof(int)*size);

	// clear the temp memroy
	memset(temp, 0, sizeof(int)*size);

	// test if the first element is also strong
	while (power_spectrum[0] > power_spectrum[i] && i < size)
		++i;

	// test if i is on the last spot
	if (i == size || i == size+1)
	{
		++count;
		temp[0] = power_spectrum[0];
	} // end if

	// loop and find the remaining relative maxima's
	for (i = 1; i < size; i++)
	{
		int j = i + 1;		// more looping vars
		int k = i - 1;

		// test
		if (power_spectrum[i] > power_spectrum[j] && power_spectrum[i] > power_spectrum[k])
		{
			++count;
			temp[i] = power_spectrum[i];
		} // end if
		else
		{
			// test if the adjecent sides are equal
			while(power_spectrum[i] == power_spectrum[j] && j < size)
				++j;

			// now test this value
			if (power_spectrum[i] > power_spectrum[j])
				marker1 = true;

			// do the same for pervious values
			while (power_spectrum[i] == power_spectrum[k] && k > 0)
				--k;

			// test
			if (power_spectrum[i] > power_spectrum[k])
				marker2 = true;
		} // end else

		// finally test markers and set this value as resonant
		if (marker1 && marker2)
		{
			++count;
			marker1 = marker2 = false;
			temp[i] = power_spectrum[i];
		} // end if

	} // end for

	i = size - 2;

	// test if the last element is also strong
	while (power_spectrum[size - 1] > power_spectrum[i] && i > 0)
		--i;

	if (i == 0 || i == -1)
	{
		++count;
		temp[size-1] = power_spectrum[size-1];
	} // end if

	// allocate memory for the buffers
	resonant_frequency = (int *)malloc(sizeof(int)*count);
	resonant_location = (int *)malloc(sizeof(int)*count);

	// now copy everything from the temp
	for (i = 0; i < size; i++)
	{
		if (temp[i])
		{
			resonant_frequency[i] = temp[i];
			resonant_location[i] = i;
		} // end if
	} // end for

	// free the temp buffer
	free(temp);

	// return the count
	return(count);
} // end Find_Resonant_Frequencies

//-----------------------------------------------

int Get_Resonant_Envelope(const int *power_spectrum, const int size, RESONANT_SLOPES_PTR speech_data)
{
// this function fills in the data for the RESONANT_SLOPES structure

	// trivially reject false data
	if (!speech_data)
		return(0);

	int *res_freq = NULL;		// the resonant frequencies
	int *res_loc = NULL;		// the respective locations in the signal
	int count;					// the count of resonant frequencies
	int i, j, k;				// looping vars

	// find the number of relative maxima's
	count = Find_Resonant_Frequencies(power_spectrum, size, res_freq, res_loc);

	// calculate the number of slopes using Fredrick Gauss formula for
	//	the sum of the first N-1 integers
	speech_data->num_slopes = (count * (count - 1)) >> 1;

	// next allocate memory for the slopes
	speech_data->slopes = (int *)malloc(sizeof(int)*speech_data->num_slopes);
	
	// loop and calculate the slope from each point
	for (k = 0; k < speech_data->num_slopes; k++)
	{
		for (i = 0; i < count-1; i++)
		{
			for (j = i + 1; j < count; j++)
				speech_data->slopes[k] = (res_freq[j] - res_freq[i]) / (res_loc[j] - res_loc[i]);
		} // end for i

	} // end for k

	// finally store the count of the resonants
	speech_data->count = count;
	
	// write the speech data to file
	HANDLE file_handle = NULL;

	file_handle = CreateFile("myfile.snd",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// now save the buffer
	int nu_size = sizeof(RESONANT_SLOPES);
	DWORD bytes = 0;
	WriteFile(file_handle, (void *)&nu_size, sizeof(int), &bytes, NULL);
	WriteFile(file_handle, (void *)speech_data, nu_size, &bytes, NULL);

	// close file
	CloseHandle(file_handle);


	return(0);

} // end Get_Resonant_Envelope

//----------------------------------------------------------------------------------------------|
// END OF FILE
//----------------------------------------------------------------------------------------------|