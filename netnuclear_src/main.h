#define PROGRAM_NAME		"NetNuclear"
#define PROGRAM_VERSION		4
#define PROGRAM_AUTHOR		"John David Karlgren"
#define AUTHOR_EMAIL		"david@studiostok.se"
#define AUTHOR_WEBSITE		"http://www.studiostok.se"

#define PROGRAM_ERRORLOG	"errorlog.txt"

extern int run;
#define GAME_SCREEN_TERMINAL	0
#define GAME_SCREEN_WORLD	1
extern int screen, queued_screen;  /*  current game screen */
extern int current_turn;
/* functions */
void init_game(int);
void set_screen(int num);
void check_queued_screen();

#ifdef __amigaos4__
#include <proto/utility.h>
#define memcpy(x,y,z) IUtility->MoveMem(y,x,z);
#endif

