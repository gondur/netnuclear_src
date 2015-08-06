#define NUM_PLAYERS		5
#define NUM_CITIES		5
#define MAX_NAME_LENGTH		14

#define INIT_PLAYER_INACTIVE	0	/*	not active in game	*/
#define INIT_PLAYER_HUMAN	1
#define INIT_PLAYER_AI		2
#define INIT_PLAYER_RESET	3	/*	only reset cities and weapons	*/

#define NUM_ACTIONS		15	/*	includes doing nothing	*/
#define ACTION_NOTHING		0	/*	no action selected yet	*/
#define ACTION_BUILD		1
#define ACTION_PROPAGANDA	2
#define ACTION_MISSILE_10MT	3
#define ACTION_MISSILE_20MT	4
#define ACTION_MISSILE_50MT	5
#define ACTION_MISSILE_100MT	6
#define ACTION_WARHEAD_10MT	7
#define ACTION_WARHEAD_20MT	8
#define ACTION_WARHEAD_50MT	9
#define ACTION_WARHEAD_100MT	10
#define ACTION_NP1		11
#define ACTION_GR2		12
#define ACTION_LNDS		13
#define ACTION_MEGA		14

struct action_node {
   int turn;
   int action;	/*	selected action this turn	*/
   int target_player;
   int target_city;
   int success;		/*	success or failure	*/
   int res_num;
   int res_recv;	/*	result received from server?	*/
   int res_build[NUM_WEAPONS];
/* propaganda only */
   int res_city;
   int res_target_city;

   struct action_node *next;
};

#define IS_ACTIVE(player)		players[player].active
#define GET_NAME(player)		players[player].name
#define GET_POPULATION(player, city)	players[player].cities[city]
#define GET_STOCK(player, weapon)	players[player].weapons[weapon]
#define GET_PREVIOUS(player)		players[player].prev_action
#define GET_MISSILE(player)		players[player].missile_prepared
#define GET_BOMBER(player)		players[player].bomber_prepared

/* player structure */
struct Splayers{
   int active;
   char name[MAX_NAME_LENGTH+1];
   int ai;	/*	computer opponent?	*/
   int portrait;
   int cities[NUM_CITIES];
   int weapons[NUM_WEAPONS];
   char msg[MAX_MESSAGE_LENGTH+1];	/*	chat message	*/
   float msg_timer;
   struct action_node *action_list;
   int prev_action;	/*	action selected last turn	*/
   int missile_prepared;
   int bomber_prepared;
   int mt_left;		/*	used by bombers	*/
};
extern struct Splayers players[NUM_PLAYERS];
extern int me;	   /*  local player	   */

/* functions */
int recv_all_actions(int turn);
void check_turn_ready();
int check_gameover(int world);
void do_gameover();
int get_winner();
struct action_node *add_action(int player, int turn);
struct action_node *find_action(int player, int turn);
void server_prepare_actions();
void do_actions();
void do_build(int player);
void do_propaganda(int player);
void do_missile(int player);
void do_bomber(int player);
void do_defense(int player);
void do_nuke(int player);
void init_player(int player, int option);
int has_cities(int player, int world);
int get_smallest_city(int player);
int get_biggest_city(int player);
int get_active_players();
int get_free_player();
int player_name_available(char *name);
