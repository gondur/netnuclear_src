#include "SDL_net.h"
#include "main.h"
#include "constants.h"
#include "input.h"
#include "misc.h"
#include "players.h"
#include "terminal.h"
#include "net.h"
#include "packets.h"

SDLNet_SocketSet sockset;
int online;	/*	are we online?	*/
int server;	/*	are we the server?	*/
struct socket_node *socket_list;
void init_net() {
   if (SDLNet_Init() < 0)
      do_error("Failed initializing SDL_net (\"%s\")", SDLNet_GetError());

   sockset = SDLNet_AllocSocketSet(NUM_PLAYERS);
   if (!sockset)
      do_error(SDLNet_GetError());

   online = 0;
   server = 0;
}

void close_server() {
   while (socket_list)
      remove_socket(socket_list);

   online = 0;
   server = 0;
   init_door();
   if (screen == GAME_SCREEN_WORLD)
      init_telegraph();
   init_game(0);
}

/* starts server if address is NULL */
void server_connect(char *address, int port) {
   IPaddress ip;
   TCPsocket sock;
   struct socket_node *socket;

   if (port < 0 || port > 65535)
      telegram("\nIllegal port.\n");
   else  if (SDLNet_ResolveHost(&ip, address, port) < 0) {
      if (!address)
         do_error(SDLNet_GetError());
      else
         telegram("\nFailed resolving host.\n");
   }
   else {
      sock = SDLNet_TCP_Open(&ip);
      if (!sock) {
         if (!address)
            do_error(SDLNet_GetError());
         else
            telegram("\nFailed connecting to the\nserver.\n");
      }
      else {
         socket = new_socket(SOCKET_SERVER);
         socket->sock = sock;
         if (SDLNet_TCP_AddSocket(sockset, socket->sock) < 0)
            do_error(SDLNet_GetError());

         online = 1;	/*	we are online!	*/
         if (!address) {
            server = 1;		/*	and we are the server	*/
            telegram("\nServer opened on port\n%d. Waiting for other\nplayers.\n", port);
         }
         else
            telegram("\nConnected to server.\n");
      }
   }
}

void check_sockets() {
   int num;
   struct socket_node *p, *next;
   struct packet_disconnect packet;
   
   if (online) {
      num = SDLNet_CheckSockets(sockset, 0);
      if (num < 0)
         do_error("Failed checking socket set for activity");
      else if (num) { 
         for (p = socket_list; p != NULL; p = next) {
            next = p->next;
            if (SDLNet_SocketReady(p->sock)) {
               if (p->type == SOCKET_SERVER && server)
                  new_connection(p->sock);
               else {
                  if (read_socket(p->sock, p->recv_buf, &p->recv_buf_size) <= 0) {
                     if (p->type == SOCKET_SERVER) {
                        server_disconnect();
                        telegram("\nThe server disconnected.\n");
                     }
                     else {
                        telegram("\n%s has\ndisconnected from the\nserver.\n", GET_NAME(p->player));
                        init_player(p->player, INIT_PLAYER_INACTIVE);
                     /* tell other clients */
                        packet.player =swap_int(p->player);
                        packet.error =swap_int(ERROR_CONNECTION);
                        send_all_sockets(PACKET_DISCONNECT, &packet, sizeof(struct packet_disconnect));
                        remove_socket(p);
                     }
                  }
               }
            }
         }
      }
   }
}

int read_socket(TCPsocket sock, char *recv_buf, int *recv_buf_size) {
   int len;
   char tmp[BUFFER_SIZE];
    memset(tmp, 0, BUFFER_SIZE);
   len = SDLNet_TCP_Recv(sock, &tmp, BUFFER_SIZE);
   if (len > 0) {
      if (*recv_buf_size + len > BUFFER_SIZE)
         do_error("Receive buffer overflow");	/*	shouldn't happen	*/
      memcpy(recv_buf + *recv_buf_size, tmp, len);
      *recv_buf_size += len;
   }

   return len;
}

void new_connection(TCPsocket sock) {
   TCPsocket tmp;
   struct socket_node *socket;
   IPaddress *ip;
 
/* accept the connection temporarily */
   tmp = SDLNet_TCP_Accept(sock);
   if (!tmp)
      do_error(SDLNet_GetError());

/* are we full or game already started? */
   if (get_active_players() + 1 > NUM_PLAYERS || current_turn)
      SDLNet_TCP_Close(tmp);
   else {
   /* nope! */
      socket = new_socket(SOCKET_CLIENT);
      socket->sock = tmp;
      if (SDLNet_TCP_AddSocket(sockset, tmp) < 0)
         do_error(SDLNet_GetError());

      ip = SDLNet_TCP_GetPeerAddress(tmp);
      if (!ip)
         do_error(SDLNet_GetError());
      if (SDLNet_ResolveIP(ip))
         strcpy(socket->host, SDLNet_ResolveIP(ip));
      else
         sprintf(socket->host, "Unknown IP");

      join_player(socket);	/*	add player to game	*/
   }
}

/* add the recently connected client to the game as a player */
void join_player(struct socket_node *socket) {
   struct packet_version packet;
   struct packet_welcome packet2;
   int i;
   struct action_node *action;
   struct socket_node *p;
   socket->player = get_free_player();
   init_player(socket->player, INIT_PLAYER_HUMAN);
/* send version */
   packet.version = swap_int(PROGRAM_VERSION);
   send_socket(socket, PACKET_VERSION, &packet, sizeof(struct packet_version));
   
/* send welcome */
   packet2.player = swap_int(socket->player);
   send_socket(socket, PACKET_WELCOME, &packet2, sizeof(struct packet_welcome));

/* send all previous players to the new client */
   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i) && i != socket->player) {
         send_player_info(socket, i, 0);
      /* send any already selected action */
         action = find_action(i, current_turn);
         if (action && action->action != ACTION_NOTHING)
            send_action(socket, i, current_turn);
      }
   }

/* send the new player to ALL clients */
   for (p = socket_list; p != NULL; p = p->next) {
      if (p->type == SOCKET_CLIENT)
         send_player_info(p, socket->player, 1);
   }

   telegram("\n%s has\nconnected to the server.\n", GET_NAME(socket->player));
}

void send_socket(struct socket_node *socket, int type, void *data, int size) {
   struct packet_header header;
 
   if (socket->send_buf_size + sizeof(struct packet_header) + size > BUFFER_SIZE)
      do_error("Send buffer overflow");	/*	shouldn't happen	*/
   header.type =swap_int(type);
   header.size =swap_int(size);
/* copy header to buffer */
   memcpy(socket->send_buf + socket->send_buf_size, &header, sizeof(struct packet_header));
   socket->send_buf_size += sizeof(struct packet_header);
/* copy packet data to buffer */
   memcpy(socket->send_buf + socket->send_buf_size, data, size);
   socket->send_buf_size += size;
}

/* only used by the server */
void send_all_sockets(int type, void *data, int size) {
   struct socket_node *p;

   if (server) {
      for (p = socket_list; p != NULL; p = p->next) {
         if (p->type != SOCKET_SERVER)
            send_socket(p, type, data, size);
      }
   }
}

void send_buffered_packets() {
   struct socket_node *p;
   int len;

   for (p = socket_list; p != NULL; p = p->next) {
      if (p->send_buf_size) {
         len = SDLNet_TCP_Send(p->sock, p->send_buf, p->send_buf_size);
         if (len > 0) {
         /* delete sent data from buffer */
            memcpy(p->send_buf, p->send_buf + len, p->send_buf_size - len);
            p->send_buf_size -= len;
         }
      }
   }
}

/* disconnect from the server (if client) */
void server_disconnect() {
   struct socket_node *p;

   if (online && !server) {
      p = socket_list;
      while (p->type != SOCKET_SERVER)
         p = p->next;

      remove_socket(p);
      online = 0;
      init_door();
      if (screen == GAME_SCREEN_WORLD)
         init_telegraph();
      init_game(0);
   }
}

struct socket_node *new_socket(int type) {
   struct socket_node *newn, *p;

   newn = (struct socket_node *)malloc(sizeof(struct socket_node));
   memset(newn, 0,sizeof(struct socket_node));

   newn->type = type;
   newn->recv_buf_size = 0;
   newn->send_buf_size = 0;

   newn->next = NULL;

   if (socket_list == NULL)
      socket_list = newn;
   else {
      p = socket_list;
      while (p->next != NULL)
         p = p->next;
      p->next = newn;
   }

   return newn;
}

void remove_socket(struct socket_node *socket) {
   struct socket_node *p, *prev=NULL;

   SDLNet_TCP_DelSocket(sockset, socket->sock);
   SDLNet_TCP_Close(socket->sock);

   p = socket_list;
   if (p == socket) {
      socket_list = socket_list->next;
      free(p);
   }
   else {
      while (p != NULL && p != socket) {
         prev = p;
         p = p->next;
      }
      if (p == NULL)
         do_error("remove_socket()");	/*	this shouldn't happen	*/
      if (prev) prev->next = p->next;
      free(p);
   }
}

struct socket_node *find_server_socket() {
   struct socket_node *p;

   if (online && !server) {
      for (p = socket_list; p != NULL; p = p->next) {
         if (p->type == SOCKET_SERVER)
            return p;
      }
   }

   return NULL;
}

struct socket_node *find_socket(int player) {
   struct socket_node *p;

   for (p = socket_list; p != NULL; p = p->next) {
      if (p->type == SOCKET_CLIENT && p->player == player)
         return p;
   }

   return NULL;
}
