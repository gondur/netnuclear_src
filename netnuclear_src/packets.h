#define PACKET_ERROR		0
#define PACKET_VERSION		1
#define PACKET_WELCOME		2
#define PACKET_DISCONNECT	3
#define PACKET_PLAYER		4
#define PACKET_NAME		5
#define PACKET_MESSAGE		6
#define PACKET_START		7
#define PACKET_ACTION		8
#define PACKET_RESULT		9	/*	action result	*/

/* disconnect reasons */
#define ERROR_CONNECTION	0	/*	connection lost	*/
#define ERROR_ILLEGAL		1	/*	illegal packet	*/

struct packet_header {
   int type;
   int size;
};

struct packet_error {
   int error;
};

struct packet_version {
   int version;
};

struct packet_welcome {
   int player;	/*	you are player X...	*/
};

struct packet_disconnect {
   int player;
   int error;	/*	reason	*/
};

struct packet_player {
   int player;
   char name[MAX_NAME_LENGTH+1];
   int ai;
   int portrait;
   int cities[NUM_CITIES];
   int announce;	/*	"player has connected"	*/
};

struct packet_name {
   int player;	/*	ignored when client sends	*/
   char name[MAX_NAME_LENGTH+1];
};

struct packet_message {
   int player;	/*	ignored when client sends	*/
   char msg[MAX_MESSAGE_LENGTH+1];
};

struct packet_action {
   int turn;
   int player;	/*	ignored when client sends	*/
   int action;
   int target_player;
   int target_city;
};

struct packet_result {
   int turn;
   int player;
   int success;
   int res_num;
};

/* functions */
void read_buffered_packets();
int get_packet(struct socket_node *socket, char *data, int size);
void do_packet_version(struct packet_version *packet);
void do_packet_welcome(struct packet_welcome *packet);
void do_packet_disconnect(struct packet_disconnect *packet);
void do_packet_player(struct packet_player *packet);
void do_packet_name(struct socket_node *socket, struct packet_name *packet);
void do_packet_message(struct socket_node *socket, struct packet_message *packet);
void do_packet_start();
void do_packet_action(struct socket_node *socket, struct packet_action *packet);
void do_packet_result(struct packet_result *packet);
void send_player_info(struct socket_node *socket, int player, int announce);
void send_action(struct socket_node *socket, int player, int turn);
