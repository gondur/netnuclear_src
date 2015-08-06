/* action message */
#define NUM_MESSAGE_LINES	5
#define MESSAGE_X	8
#define MESSAGE_Y	386
extern char action_str[BUFFER_SIZE];

#define GET_WORLD(player, city)		world_state[player].cities[city]
#define GET_WORLD_MISSILE(player)	world_state[player].missile_prepared
#define GET_WORLD_BOMBER(player)	world_state[player].bomber_prepared

/* world state */
struct Sworld_state{
   int cities[NUM_CITIES];
   int missile_prepared;
   int bomber_prepared;
};
extern struct Sworld_state world_state[NUM_PLAYERS];

extern int world_turn;	   /*  game turn currently displayed   */
extern int world_player;   /*  player to draw actions for  */
extern int world_step;

/* functions */
void next_player();
void draw_cities(int player, int x, int y);
void draw_world();
void draw_factory(int x, int y);
void draw_radio(int x, int y);
void draw_missile_pad(int x, int y, int animated);
void init_missile_sprite(int player);
void draw_missile(int player);
void init_bomber_sprite(int player, int dir);
void draw_bomber(int player);
int get_bomber_frame(float angle);
void init_defector_sprite(int player);
void draw_defector(int x, int y);
void draw_dish(int x, int y);
void draw_explosion(int player, int city);
void draw_action_message();
void move_sprite_to_destination(int num, float speed);
int get_country_x(int player);
int get_country_y(int player);
int get_city_x(int player, int city);
int get_city_y(int player, int city);
