#include "SDL_net.h"
#include "main.h"
#include "constants.h"
#include "input.h"
#include "commands.h"
#include "misc.h"
#include "players.h"
#include "terminal.h"
#include "world.h"
#include "net.h"
#include "packets.h"

/* typeable commands */
struct {
   char *str;
   void (*function)(char *args);
} command_list[] = {
   { "add",        cmd_add },
   { "clear",      cmd_clear },
   { "close",      cmd_close },
   { "connect",    cmd_connect },
   { "disconnect", cmd_disconnect },
   { "help",       cmd_help },
   { "host",       cmd_host },
   { "name",       cmd_name },
   { "quit",       cmd_quit },
   { "start",      cmd_start },
   { "turn",       cmd_turn },
   { "version",    cmd_version },
   { "who",        cmd_who },
   { "",           NULL }
};

void do_command(char *str) {
   int i, ok = 0;
   char cmd[MAX_MESSAGE_LENGTH+1], args[MAX_MESSAGE_LENGTH+1];
   struct packet_message packet;

   if (str[0] == '/') {
      if (strlen(str) > 1) {
      /* get command from string */
         memset(cmd, 0, MAX_MESSAGE_LENGTH+1);
         for (i = 1; i < strlen(str) && str[i] != ' '; i++)
            cmd[i - 1] = tolower(str[i]);

      /* find it in the list and call the assigned function */
         for (i = 0; strlen(command_list[i].str) && !ok; i++) {
            if (compare_string(cmd, command_list[i].str)) {
               memset(args, 0, MAX_MESSAGE_LENGTH+1);
               if (strlen(str) > strlen(cmd) + 2)
                  sprintf(args, str + strlen(cmd) + 2);		/*	copy arguments	*/
               command_list[i].function(args);
               ok = 1;
            }
         }
         if (!ok)
            telegram("\nUnknown command. Type\n/help to see a list.\n");
      }
   }
   else {
   /* send chat message */
      if (!online)
         telegram("\nYou're not connected to\nany server.\n");
      else {
         if (get_active_players() < 2)
            telegram("\nThere are no other\nplayers listening.\n");
         else {
            packet.player = swap_int(me);
            strcpy(packet.msg, str);
            if (!server)
               send_socket(find_server_socket(), PACKET_MESSAGE, &packet, sizeof(struct packet_message));
            else
               send_all_sockets(PACKET_MESSAGE, &packet, sizeof(struct packet_message));
            telegram("\nMessage sent.\n");
         }
      }
   }
}

COMMAND(cmd_add) {
   int player;
   struct socket_node *p;

   if (!server)
      telegram("\nYou're not the server.\n");
   else if (current_turn)
      telegram("\nThe game has already\nstarted.\n");
   else {
      player = get_free_player();
      if (player < 0)
         telegram("\nThe game is full!\n");
      else {
         init_player(player, INIT_PLAYER_AI);
         telegram("\nComputer opponent added.\n");

      /* send player to all clients */
         for (p = socket_list; p != NULL; p = p->next) {
            if (p->type == SOCKET_CLIENT)
               send_player_info(p, player, 1);
         }
      }
   }
}

COMMAND(cmd_clear) {
   init_telegraph();
}

COMMAND(cmd_close) {
   if (online && !server)
      telegram("\nYou're not the server.\n");
   else if (!server)
      telegram("\nServer is already closed.\n");
   else {
      close_server();
      telegram("\nYou have closed your\nserver.\n");
   }
}

COMMAND(cmd_connect) {
   char address[BUFFER_SIZE];

   if (server)
      telegram("\nYou must close your\nserver before you can\nconnect to another.\n");
   else {
      if (!strlen(args))
         telegram("\nYou must specify a host\naddress to connect to.\n");
      else {
         if (strlen(get_word(args, 1))) {
            strcpy(address, get_word(args, 0));
            server_connect(address, atoi(get_word(args, 1)));
         }
         else
            server_connect(args, DEFAULT_SERVER_PORT);
         init_game(0);
      }
   }
}

COMMAND(cmd_disconnect) {
   if (!online)
      telegram("\nYou're not connected to\nany server.\n");
   else {
      if (server)
         telegram("\nYou are the server. Type\n/close to close it.\n");
      else {
         server_disconnect();
         telegram("\nYou have disconnected.\n");
      }
   }
}

COMMAND(cmd_help) {
   int i = 0, i2;
   char str[BUFFER_SIZE];

   telegram("\nAvailable commands:");

   while (strlen(command_list[i].str)) {
      i2 = 0;
      while (i2 < 2 && strlen(command_list[i].str)) {
         if (!i2)
            sprintf(str, "/%s", command_list[i].str);
         else {
            while (strlen(str) <= 12)
               strcat(str, " ");
            strcat(str, "/");
            strcat(str, command_list[i].str);
         }
         i2++;	/*	next column	*/
         i++;	/*	next command	*/
      }
      telegram("\n%s", str);
      
   }
}

COMMAND(cmd_host) {
   if (server)
      telegram("\nYou are already hosting\na server.\n");
   else if (online)
      telegram("\nYou must disconnect\nbefore you can host a\nserver.\n");
   else {
      if (strlen(args))
         server_connect(NULL, atoi(args));
      else
         server_connect(NULL, DEFAULT_SERVER_PORT);
      init_game(0);
   }
}

COMMAND(cmd_name) {
   struct packet_name packet;

   if (!online)
      telegram("\nYou must be connected to\na server or host one to\nchange your name.\n");
   else {
      if (!strlen(args))
         telegram("\nYou must specify a name.\n");
      else if (strlen(args) > MAX_NAME_LENGTH)
         telegram("\nThe name is too long.\n");
      else if (!player_name_available(args))
         telegram("\nThat name is already in\nuse by another player.\n");
      else {
         strcpy(GET_NAME(me), args);
         telegram("\nYour name is now:\n\"%s\".\n", args);

      /* send to server or clients */
         packet.player = swap_int(me);
         strcpy(packet.name, args);
         if (!server)
            send_socket(find_server_socket(), PACKET_NAME, &packet, sizeof(struct packet_name));
         else
            send_all_sockets(PACKET_NAME, &packet, sizeof(struct packet_name));
      }
   }
}

COMMAND(cmd_quit) {
   run = 0;
}

COMMAND(cmd_start) {
   if (!server)
      telegram("\nYou're not the server.\n");
   else if (get_active_players() < 2)
      telegram("\nAtleast one more player\nis required.\n");
   else if (current_turn)
      telegram("\nThe game has already\nstarted.\n");
   else {
      send_all_sockets(PACKET_START, NULL, 0);	/*	send start indicator	*/
      init_game(1);
      telegram("\nWelcome to %s.\n%s\n", PROGRAM_NAME, msgs_welcome[randomize(0, 47)]);
   }
}

COMMAND(cmd_turn) {
   if (!current_turn)
      telegram("\nThe game hasn't started\nyet.\n");
   else
      telegram("\nThis is turn %d.\n", current_turn);
}

COMMAND(cmd_version) {
   telegram("\n\"%s\" Version %d\nby %s\n<%s>\n%s\nAmigaOS4 Port\nby Frank Menzel\n", PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_AUTHOR, AUTHOR_EMAIL, AUTHOR_WEBSITE);
}

COMMAND(cmd_who) {
   int i, num = 0;

   if (online) {
      for (i = 0; i < NUM_PLAYERS; i++) {
         if (IS_ACTIVE(i) && i != me) {
            if (!num)
               telegram("\nConnected players:\n");
            telegram(" %s", GET_NAME(i));
            if (players[i].ai)
               telegram(" (computer)");
            telegram("\n");
            num++;
         }
      }
   }

   if (!num)
      telegram("\nNo other players are\nconnected.\n");
}
