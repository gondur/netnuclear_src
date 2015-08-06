#define MAX_MESSAGE_LENGTH	38

extern int mouse_x, mouse_y;
extern int typing; /*  typing message  */
extern char type_str[MAX_MESSAGE_LENGTH+1];

/* mouse zones */
#define NUM_ZONES		19
#define ZONE_NUCLEAR		0
#define ZONE_MONITOR_TL		1
#define ZONE_MONITOR_BL		2
#define ZONE_MONITOR_TR		3
#define ZONE_MONITOR_BR		4
#define ZONE_BUILD		5
#define ZONE_PROPAGANDA		6
#define ZONE_MISSILE_10MT	7
#define ZONE_MISSILE_20MT	8
#define ZONE_MISSILE_50MT	9
#define ZONE_MISSILE_100MT	10
#define ZONE_WARHEAD_10MT	11
#define ZONE_WARHEAD_20MT	12
#define ZONE_WARHEAD_50MT	13
#define ZONE_WARHEAD_100MT	14
#define ZONE_NP1		15
#define ZONE_GR2		16
#define ZONE_LNDS		17
#define ZONE_MEGA		18

/* functions */
void init_input();
void read_input();
void mouse_click();
void click_monitor(int num);
void click_build();
void click_propaganda();
void click_missile(int mt);
void click_bomber(int type);
void click_warhead(int mt);
void click_defense(int type);
void click_city(int player, int city);
void end_turn();
void add_typing(SDL_keysym *sym);
void draw_typing();
void draw_cursor();
