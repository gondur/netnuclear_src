#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		480
#define FRAMES_PER_SECOND	60	/*	target frame rate	*/

/* font */
#define FONT_WIDTH	12
#define FONT_HEIGHT	16
#define FONT_LEADING	18	/*	used by the telegraph	*/
#define COL_BLACK	0
#define COL_WHITE	1
#define COL_YELLOW	2
#define COL_RED		3

#define NUM_BITMAPS	26
#define BMP_CURSOR	0
#define BMP_FONT	1
#define BMP_TERMINAL	2
#define BMP_MONITOR	3
#define BMP_LEADERS	4
#define BMP_DOOR	5
#define BMP_LED		6
#define BMP_PRINTHEAD	7
#define BMP_INPUT	8
#define BMP_MESSAGE	9
#define BMP_TARGET	10	/*	target arrows	*/
#define BMP_ARROW	11	/*	portrait arrow	*/
#define BMP_SILO_BIG	12
#define BMP_BUTTON_BIG	13
#define BMP_RUNWAY_BIG	14
#define BMP_DISH_BIG	15
#define BMP_WORLD	16
#define BMP_CITIES	17
#define BMP_FACTORY	18
#define BMP_RADIO	19
#define BMP_MISSILE	20
#define BMP_MISSILE2	21	/*	flying	*/
#define BMP_BOMBER	22
#define BMP_DISH	23
#define BMP_DEFECTOR	24
#define BMP_EXPLOSION	25

extern SDL_Surface *mainscr, *surfaces[NUM_BITMAPS];

#define ANIM_MODE_NONE		0
#define ANIM_MODE_NORMAL	1
#define ANIM_MODE_PINGPONG	2
#define ANIM_MODE_ONESHOT	3

#define GET_W(num)		sprites[num].w
#define GET_H(num)		sprites[num].h
#define GET_FRAME(num)		sprites[num].frame

struct Ssprites{
   int w;	/*	frame width	*/
   int h;	/*	frame height	*/
/* position (used by missiles, bombers and defectors) */
   float x;
   float y;
   int dst_x;
   int dst_y;
   int start_x;
   int start_y;
   float angle;		/*	direction faced	*/
/* animation */
   int frames;	/*	number of frames	*/
   int speed;	/*	delay until next frame	*/
   int mode;
   int frame;	/*	current frame	*/
   int anim_dir;	/*	if pingpong mode	*/
   float timer;
};
extern struct Ssprites sprites[NUM_BITMAPS];
extern Uint32 tick;
extern float timestep;

/* functions */
void init_sprites();
void set_sprite(int num, int w, int h, int frames, int speed, int mode);
void do_animation();
void init_screen();
void load_graphics();
void blit(int num, int x, int y, int w, int h, int x2, int y2);
void draw_text(int x, int y, int col, char *str, ...);
void render();
void save_screenshot();
Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
