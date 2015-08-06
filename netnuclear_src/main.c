#include <time.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_net.h"
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
int run;
int screen, queued_screen;	/*	current game screen	*/
int current_turn;
int main(int argc, char *argv[]) {
   srand(time(NULL));
   init_screen();
   init_sound();
   init_input();
   load_graphics();
   load_samples();

   init_sprites();
   init_door();
   init_telegraph();

   init_net();
   if (argc > 1) {
      if (compare_string(argv[1], "--server")) {
      /* start server */
         if (argc > 2)
            server_connect(NULL, atoi(argv[2]));
         else
            server_connect(NULL, DEFAULT_SERVER_PORT);
      }
      else {
      /* connect as client */
         if (argc > 2)
            server_connect(argv[1], atoi(argv[2]));
         else
            server_connect(argv[1], DEFAULT_SERVER_PORT);
      }
   }
   init_game(0);

   tick = SDL_GetTicks();	/*	program starts... NOW!	*/
   run = 1;
   while (run) {
      check_sockets();
      read_buffered_packets();
      read_input();
      check_turn_ready();
      check_queued_screen();
      send_buffered_packets();
   /* draw screen */
      switch (screen) {
         case GAME_SCREEN_TERMINAL:
            draw_terminal();
            draw_door();
            draw_telegraph();
            draw_chat();
            break;
         case GAME_SCREEN_WORLD:
            draw_world();
            draw_action_message();
            break;
      }
      draw_typing();
      if (!door_dir || screen == GAME_SCREEN_WORLD)
         do_animation();	/*	update frames	*/
      draw_cursor();
      render();
   }

   if (server)
      close_server();

   return 0;
}

/* initialize or start game */
void init_game(int start) {
   int i;
   struct socket_node *p;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (!start)
         init_player(i, INIT_PLAYER_INACTIVE);
      else {
         if (IS_ACTIVE(i)) {
            init_player(i, INIT_PLAYER_RESET);
            if (server) {
            /* send to all clients */
               for (p = socket_list; p != NULL; p = p->next) {
                  if (p->type == SOCKET_CLIENT)
                     send_player_info(p, i, 0);
               }
            }
         }
      }
   }
   if (!start) {
      if (server) {
         me = 4;
         init_player(me, INIT_PLAYER_HUMAN);
      }
      else
         me = -1;
      current_turn = 0;
   }
   else
      current_turn = 1;
   world_turn = current_turn;

/* change screen */
   if (screen == GAME_SCREEN_WORLD)
      queued_screen = GAME_SCREEN_TERMINAL;
   else
      set_screen(GAME_SCREEN_TERMINAL);
}

void set_screen(int num) {
   stop_samples();

   switch (num) {
      case GAME_SCREEN_TERMINAL:
         if (current_turn && me > -1)
            view = VIEW_PLAYER0 + me;
         else
            view = -1;
         queued_view = view;
         flash = 0;
         flash_timer = 0;
         break;
      case GAME_SCREEN_WORLD:
         world_player = -1;
         world_step = 0;
         memset(action_str, 0, BUFFER_SIZE);
         next_player();
         break;
   }

   screen = num;
   queued_screen = num;
}

/* check if game screen is queued to change */
void check_queued_screen() {
   int ok = 1;

   if ((!door_dir && !door_y) || screen == GAME_SCREEN_WORLD) {
      if (screen != queued_screen) {
         if (queued_screen == GAME_SCREEN_WORLD)
            ok = recv_all_actions(world_turn);	/*	check if all players are ready	*/
         if (ok)
            set_screen(queued_screen);
      }
      else {
      /* change country view */
         if (screen == GAME_SCREEN_TERMINAL) {
            view = queued_view;
            if (online && view > -1) {
               if (get_active_players() > 1) {
                  init_sprites();
               /* reset portrait arrow */
                  arrow_x = 0;
                  arrow_dir = 1;
                  arrow_timer = 0;
                  door(1);
               }
            }
         }
      }
   }
}
