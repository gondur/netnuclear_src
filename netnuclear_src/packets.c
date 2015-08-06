#include "SDL_net.h"
#include "SDL_mixer.h"
#include "main.h"
#include "constants.h"
#include "gfx.h"
#include "input.h"
#include "sfx.h"
#include "misc.h"
#include "players.h"
#include "terminal.h"
#include "world.h"
#include "net.h"
#include "packets.h"

void read_buffered_packets() {
   void *data;
   struct socket_node *p, *next;
   struct packet_header header;
   struct packet_disconnect packet;
int ab=0;

   data = malloc(BUFFER_SIZE);
   memset(data, 0, BUFFER_SIZE);

   for (p = socket_list; p != NULL; p = next) {
      next = p->next;
      if (p->recv_buf_size >= sizeof(struct packet_header)) {
         memcpy(&header, p->recv_buf, sizeof(struct packet_header));	/*	header	*/
     
         header.size=swap_int(header.size);
         header.type=swap_int(header.type);
     
         if (get_packet(p,(char*) data, header.size)) {
            switch (header.type) {
               case PACKET_VERSION:
                  ((struct packet_version *)data)->version = swap_int(((struct packet_version *)data)->version);
                  do_packet_version((struct packet_version *)data);
                  break;
               case PACKET_WELCOME:
                  ((struct packet_welcome *)data)->player = swap_int(((struct packet_welcome *)data)->player);
                  do_packet_welcome((struct packet_welcome *)data);
                  break;
               case PACKET_DISCONNECT:
                  ((struct packet_disconnect *)data)->player = swap_int(((struct packet_disconnect *)data)->player);
                  ((struct packet_disconnect *)data)->error = swap_int(((struct packet_disconnect *)data)->error);
                  do_packet_disconnect((struct packet_disconnect *)data);
                  break;
               case PACKET_PLAYER:
                ((struct packet_player *)data)->player = swap_int(((struct packet_player *)data)->player);
                ((struct packet_player *)data)->ai = swap_int(((struct packet_player *)data)->ai);
                ((struct packet_player *)data)->portrait = swap_int(((struct packet_player *)data)->portrait);

                for (ab=0; ab<NUM_CITIES; ab++)
                ((struct packet_player *)data)->cities[ab] = swap_int(((struct packet_player *)data)->cities[ab]);

                ((struct packet_player *)data)->announce = swap_int(((struct packet_player *)data)->announce);
                  do_packet_player((struct packet_player *)data);
                  break;
               case PACKET_NAME:
                ((struct packet_name *)data)->player = swap_int(((struct packet_name *)data)->player);
                  do_packet_name(p,(struct packet_name *) data);
                  break;
               case PACKET_MESSAGE:
                ((struct packet_message *)data)->player = swap_int(((struct packet_message *)data)->player);
                  do_packet_message(p, (struct packet_message *)data);
                  break;
               case PACKET_START:
                  do_packet_start();
                  break;
               case PACKET_ACTION:
                ((struct packet_action *)data)->turn = swap_int(((struct packet_action *)data)->turn);
                ((struct packet_action *)data)->player = swap_int(((struct packet_action *)data)->player);
                ((struct packet_action *)data)->action = swap_int(((struct packet_action *)data)->action);
                ((struct packet_action *)data)->target_player = swap_int(((struct packet_action *)data)->target_player);
                ((struct packet_action *)data)->target_city = swap_int(((struct packet_action *)data)->target_city);
                  do_packet_action(p,(struct packet_action *)data);
                  break;
               case PACKET_RESULT:
                ((struct packet_result *)data)->turn = swap_int(((struct packet_result *)data)->turn);
                ((struct packet_result *)data)->player = swap_int(((struct packet_result *)data)->player);
                ((struct packet_result *)data)->success = swap_int(((struct packet_result *)data)->success);
                ((struct packet_result *)data)->res_num = swap_int(((struct packet_result *)data)->res_num);
                  do_packet_result((struct packet_result *)data);
                  break;
               default:
                  if (p->type == SOCKET_SERVER) {
                  /* bad packet from server */
                     server_disconnect();
                     telegram("\nIllegal server packet.\nDisconnecting.\n");
                  }
                  else {
                  /* bad packet from client */
                     telegram("\nIllegal packet from\n%s. Disconnecting.\n", GET_NAME(p->player));
                     init_player(p->player, INIT_PLAYER_INACTIVE);
                  /* tell other clients */
                     packet.player = swap_int(p->player);
                     packet.error = swap_int(ERROR_ILLEGAL);
                     send_all_sockets(PACKET_DISCONNECT, &packet, sizeof(struct packet_disconnect));
                     remove_socket(p);
                  }
                  break;
            }
         }
      }
   }

   free(data);
}

/* extract packet data from receive buffer */
int get_packet(struct socket_node *socket, char *data, int size) {
   int size2;
     
   if (socket->recv_buf_size >= sizeof(struct packet_header) + size) {
      memcpy(data, socket->recv_buf + sizeof(struct packet_header), size);

   /* delete what we just extracted */
      size2 = sizeof(struct packet_header) + size;	/*	size with header included	*/
      memcpy(socket->recv_buf, socket->recv_buf + size2, socket->recv_buf_size - size2);
      socket->recv_buf_size -= size2;

      return 1;
   }

   return 0;
}

void do_packet_version(struct packet_version *packet) {
   if (packet->version != PROGRAM_VERSION) {
      server_disconnect();
      telegram("\nVersion mismatch.\nDisconnecting.\n");
   }
}

void do_packet_welcome(struct packet_welcome *packet) {
   me = packet->player;
}

void do_packet_disconnect(struct packet_disconnect *packet) {
   telegram("\n%s has\ndisconnected from the\nserver.\n", GET_NAME(packet->player));
   init_player(packet->player, INIT_PLAYER_INACTIVE);
}

void do_packet_player(struct packet_player *packet) {
   int i;
   players[packet->player].active = 1;
   strcpy(GET_NAME(packet->player), packet->name);
   players[packet->player].ai = packet->ai;
   players[packet->player].portrait = packet->portrait;
   for (i = 0; i < NUM_CITIES; i++) {
      GET_POPULATION(packet->player, i) = packet->cities[i];
      GET_WORLD(packet->player, i) =packet->cities[i];
   }

   if (packet->player != me) {
      if (packet->announce) {
         if (!players[packet->player].ai)
            telegram("\n%s has\nconnected to the server.\n", GET_NAME(packet->player));
         else
            telegram("\nComputer opponent added.\n");
      }
   }
}

void do_packet_name(struct socket_node *socket, struct packet_name *packet) {
   int player;
   struct socket_node *p;
   struct packet_name packet2;

   if (server)
      player = socket->player;	/*	don't trust the client	*/
   else
      player = packet->player;

   telegram("\n%s changed name\nto: \"%s\".\n", GET_NAME(player), packet->name);
   strcpy(GET_NAME(player), packet->name);

   if (server) {
   /* send to all other clients */
      packet2.player = player;
      strcpy(packet2.name, packet->name);
      for (p = socket_list; p != NULL; p = p->next) {
         if (p->type == SOCKET_CLIENT && p->player != player)
            send_socket(p, PACKET_NAME, &packet2, sizeof(struct packet_name));
      }
   }
}

void do_packet_message(struct socket_node *socket, struct packet_message *packet) {
   int player, sound = 0;
   struct socket_node *p;
   struct packet_message packet2;

   if (server)
      player = socket->player;	/*	don't trust the client	*/
   else
      player = packet->player;

   if (screen == GAME_SCREEN_TERMINAL)
      sound = 1;
   player_message(player, packet->msg, sound);

   if (server) {
   /* send message to all other clients */
      packet2.player = player;
      strcpy(packet2.msg, packet->msg);
      for (p = socket_list; p != NULL; p = p->next) {
         if (p->type == SOCKET_CLIENT && p->player != player)
            send_socket(p, PACKET_MESSAGE, &packet2, sizeof(struct packet_message));
      }
   }
}

void do_packet_start() {
   init_game(1);
   telegram("\nWelcome to %s.\n%s\n", PROGRAM_NAME, msgs_welcome[randomize(0, 47)]);
}

void do_packet_action(struct socket_node *socket, struct packet_action *packet) {
   int player;
   struct action_node *action;
   struct socket_node *p;

   if (server)
      player = socket->player;	/*	don't trust the client	*/
   else
      player = packet->player;

   action = find_action(player, packet->turn);
   if (!action)
      action = add_action(player, packet->turn);    /*  create new  */
   action->action = packet->action;
   action->target_player = packet->target_player;
   action->target_city = packet->target_city;

   if (server) {
   /* server updates all the other clients with the selected action */
      for (p = socket_list; p != NULL; p = p->next) {
         if (p->type == SOCKET_CLIENT && p->player != player)
            send_action(p, player, packet->turn);
      }
   }
}

void do_packet_result(struct packet_result *packet) {
   struct action_node *action;

   action = find_action(packet->player, packet->turn);
   action->success = packet->success;
   action->res_num = packet->res_num;
   action->res_recv = 1;	/*	result has been received	*/
}
void send_player_info(struct socket_node *socket, int player, int announce) {
   struct packet_player packet;
   int i;
   packet.player =swap_int(player);
   strcpy(packet.name, GET_NAME(player));
   packet.ai = swap_int(players[player].ai);
   packet.portrait = swap_int(players[player].portrait);
   for (i = 0; i < NUM_CITIES; i++)
      packet.cities[i] = swap_int(GET_POPULATION(player, i));
   packet.announce = swap_int(announce);
   send_socket(socket, PACKET_PLAYER, &packet, sizeof(struct packet_player));
}

void send_action(struct socket_node *socket, int player, int turn) {
   struct action_node *action;
   struct packet_action packet;
    
   action = find_action(player, turn);
   if (action) {
      packet.turn = swap_int(turn);
      packet.player = swap_int(player);
      packet.action = swap_int(action->action);
      packet.target_player = swap_int(action->target_player);
      packet.target_city = swap_int(action->target_city);
      send_socket(socket, PACKET_ACTION, &packet, sizeof(struct packet_action));
   }
}
