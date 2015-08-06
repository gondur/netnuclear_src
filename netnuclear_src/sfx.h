#define NUM_CHANNELS	1	/*	only one needed	*/

#define NUM_SAMPLES	17
#define WAV_TELE1	0
#define WAV_TELE2	1
#define WAV_TELE3	2
#define WAV_TELE4	3
#define WAV_TELE5	4
#define WAV_TELESTOP	5
#define WAV_DOOR	6
#define WAV_CHAT	7
#define WAV_BUILD	8
#define WAV_RADIO	9
#define WAV_FANFARE	10
#define WAV_MISSILE	11
#define WAV_ENGINE	12
#define WAV_ENGINE2	13
#define WAV_BOOM	14
#define WAV_DISH	15
#define WAV_FEET	16

extern Mix_Chunk *samples[NUM_SAMPLES];

/* functions */
void init_sound();
void load_samples();
void play_sample(int num);
void stop_samples();
