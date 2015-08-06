#define DEFAULT_SERVER_PORT	1989

extern SDLNet_SocketSet sockset;
extern int online; /*  are we online?  */
extern int server; /*  are we the server?  */

#define SOCKET_SERVER	0
#define SOCKET_CLIENT	1
struct socket_node {
   int type;	/*	server or client	*/
   TCPsocket sock;
   char host[256];
/* buffers */
   char recv_buf[BUFFER_SIZE];
   char send_buf[BUFFER_SIZE];
   int recv_buf_size;
   int send_buf_size;
   int player;	/*	player number (if client)	*/

   struct socket_node *next;
};

extern struct socket_node *socket_list;

/* functions */
void init_net();
void close_server();
void server_connect(char *address, int port);
void check_sockets();
int read_socket(TCPsocket sock, char *recv_buf, int *recv_buf_size);
void new_connection(TCPsocket sock);
void join_player(struct socket_node *socket);
void send_socket(struct socket_node *socket, int type, void *data, int size);
void send_all_sockets(int type, void *data, int size);
void send_buffered_packets();
void server_disconnect();
struct socket_node *new_socket(int type);
void remove_socket(struct socket_node *socket);
struct socket_node *find_server_socket();
struct socket_node *find_socket(int player);
