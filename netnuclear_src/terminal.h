/* terminal window */
#define WINDOW_X		200
#define WINDOW_Y		120
#define WINDOW_W		248
#define WINDOW_H		160
#define VIEW_PLAYER0	0
#define VIEW_PLAYER1	1
#define VIEW_PLAYER2	2
#define VIEW_PLAYER3	3
#define VIEW_PLAYER4	4
#define VIEW_SILO	5	/*	missile silo	*/
#define VIEW_BUTTON	6
#define VIEW_DISH	7	/*	defense satellite	*/
#define VIEW_RUNWAY	8	/*	bomber runway	*/
extern int view, queued_view;  /*  current view	   */

extern int flash;
extern float flash_timer;

/* door */
#define DOOR_H		(WINDOW_H / 2)
#define DOOR_SPEED	3.5f
extern int door_dir;   /*  direction   */
extern float door_y;

/* red portrait arrow */
extern int arrow_x, arrow_dir;
extern float arrow_timer;

/* telegraph */
#define MAX_TELEGRAPH_LINES	7
#define TELEGRAPH_SPEED	0.5f
#define TELEGRAPH_X	156
#define TELEGRAPH_Y	320
extern char tele_buf[BUFFER_SIZE], tele_printed[BUFFER_SIZE];
extern float tele_timer;
extern int prev_tele_sample;

/* functions */
void draw_chat();
void draw_terminal();
void draw_player_country();
void init_door();
void door(int dir);
void draw_door();
void player_message(int player, char *str, int sound);
void telegram(char *str, ...);
void draw_telegraph();
void init_telegraph();
int get_player_monitor(int player);
int get_monitor_player(int num);
