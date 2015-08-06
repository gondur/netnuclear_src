#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_net.h"
#include "main.h"
#include "constants.h"
#include "gfx.h"
#include "input.h"
#include "commands.h"
#include "sfx.h"
#include "misc.h"
#include "players.h"
#include "ai.h"
#include "terminal.h"
#include "world.h"
#include "net.h"
#include "packets.h"

int mouse_x, mouse_y;
int typing;	/*	typing message	*/
char type_str[MAX_MESSAGE_LENGTH+1];
/* clickable areas on the screen */
const int mouse_zones[] = {
   212, 10,  216, 84,	/*	nuclear war!	*/
   8,   6,   122, 74,	/*	player top left	*/
   8,   380, 122, 74,	/*	player bottom left	*/
   510, 6,   122, 74,	/*	player top right	*/
   510, 380, 122, 74,	/*	player bottom right	*/
   142, 14,  64,  68,	/*	build	*/
   434, 14,  64,  68,	/*	propaganda	*/
   132, 114, 56,  18,	/*	10 mt missile	*/
   132, 132, 56,  18,	/*	20 mt missile	*/
   132, 150, 56,  18,	/*	50 mt missile	*/
   132, 168, 56,  18,	/*	100 mt missile	*/
   132, 212, 56,  18,	/*	10 mt warhead	*/
   132, 230, 56,  18,	/*	20 mt warhead	*/
   132, 248, 56,  18,	/*	50 mt warhead	*/
   132, 266, 56,  18,	/*	100 mt warhead	*/
   464, 120, 46,  18,	/*	np1	*/
   464, 156, 46,  18,	/*	gr2	*/
   464, 216, 46,  18,	/*	lnds	*/
   464, 252, 46,  18,	/*	mega	*/
   -1
};

void init_input() {
   SDL_EnableUNICODE(1);
   SDL_EnableKeyRepeat(300, 25);
   typing = 0;
}

void read_input() {
   Uint8 *keystate;
   SDL_Event event;

   keystate = SDL_GetKeyState(NULL);	/*	held down keys	*/
   SDL_GetMouseState(&mouse_x, &mouse_y);

   while (SDL_PollEvent(&event)) {
      switch (event.type) {
         case SDL_QUIT:
            run = 0;
            break;
         case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
               case SDLK_ESCAPE:
                  if (typing)
                     typing = 0;
                  break;
               case SDLK_F4:
                  if (keystate[SDLK_LALT])
                     run = 0;
                  break;
               case SDLK_PRINT:
                  save_screenshot();
                  break;
               case SDLK_RETURN:
               case SDLK_KP_ENTER:
                  if (!typing) {
                     memset(type_str, 0, MAX_MESSAGE_LENGTH+1);
                     typing = 1;
                  }
                  else {
                     if (strlen(type_str))
                        do_command(type_str);
                     typing = 0;
                  }
                  break;
               case SDLK_BACKSPACE:
                  if (typing && strlen(type_str))
                     type_str[strlen(type_str) - 1] = '\0';
                  break;
               default:
                  add_typing(&event.key.keysym);
                  break;
            }
            break;
         case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
               if (online) {
               /* telegraph must be silent and door fully open */
                  if ((!strlen(tele_buf) && !door_dir && door_y == DOOR_H) || screen == GAME_SCREEN_WORLD)
                     mouse_click();
               }
            }
            break;
      }
   }
}

int get_mouse_zone(int x, int y) {
   int i, x2, y2, w, h;

   for (i = 0; i < NUM_ZONES; i++) {
      x2 = mouse_zones[i * 4];
      y2 = mouse_zones[(i * 4) + 1];
      w = mouse_zones[(i * 4) + 2];
      h = mouse_zones[(i * 4) + 3];
      if (collision(x, y, 1, 1, x2, y2, w, h))
         return i;
   }

   return -1;
}

void mouse_click() {
   int zone, player, i, offset, junk = 0;

   zone = get_mouse_zone(mouse_x, mouse_y);

   switch (screen) {
      case GAME_SCREEN_TERMINAL:
         if (collision(mouse_x, mouse_y, 1, 1, WINDOW_X, WINDOW_Y, WINDOW_W, WINDOW_H)) {
         /* did we click a city? */
            if (view >= VIEW_PLAYER0 && view <= VIEW_PLAYER4) {
               player = view - VIEW_PLAYER0;
               for (i = 0; i < NUM_CITIES; i++) {
                  offset = (player * 10) + (i * 2);
                  if (collision(mouse_x, mouse_y, 1, 1, WINDOW_X + city_positions[offset], WINDOW_Y + city_positions[offset + 1], 32, 32))
                     click_city(player, i);
               }
            }
         }
         else {
            switch (zone) {
               case ZONE_NUCLEAR:
                  end_turn();
                  break;
               case ZONE_MONITOR_TL:
               case ZONE_MONITOR_BL:
               case ZONE_MONITOR_TR:
               case ZONE_MONITOR_BR:
                  click_monitor(zone - ZONE_MONITOR_TL);
                  break;
               case ZONE_BUILD:
                  click_build();
                  junk = 1;
                  break;
               case ZONE_PROPAGANDA:
                  click_propaganda();
                  junk = 1;
                  break;
               case ZONE_MISSILE_10MT:
               case ZONE_MISSILE_20MT:
               case ZONE_MISSILE_50MT:
               case ZONE_MISSILE_100MT:
                  click_missile(zone - ZONE_MISSILE_10MT);
                  if (GET_STOCK(me, WEAPON_MISSILE_10MT + zone - ZONE_MISSILE_10MT)) {
                     junk = 1;
                     ai_clear_messages();
                     queued_view = VIEW_SILO;
                     door(-1);
                  }
                  break;
               case ZONE_WARHEAD_10MT:
               case ZONE_WARHEAD_20MT:
               case ZONE_WARHEAD_50MT:
               case ZONE_WARHEAD_100MT:
                  click_warhead(zone - ZONE_WARHEAD_10MT);
                  break;
               case ZONE_NP1:
               case ZONE_GR2:
                  click_bomber(zone - ZONE_NP1);
                  if (GET_STOCK(me, WEAPON_NP1 + zone - ZONE_NP1)) {
                     junk = 1;
                     queued_view = VIEW_RUNWAY;
                     door(-1);
                  }
                  break;
               case ZONE_LNDS:
               case ZONE_MEGA:
                  click_defense(zone - ZONE_LNDS);
                  if (GET_STOCK(me, WEAPON_LNDS + zone - ZONE_LNDS)) {
                     junk = 1;
                     ai_clear_messages();
                     queued_view = VIEW_DISH;
                     door(-1);

                  }
                  break;
            }

         /* display message if weapon will be junked */
            if (junk) {
               if (GET_MISSILE(me) > -1)
                  telegram("This will junk a missile\n");
               else if (GET_BOMBER(me) > -1 && players[me].mt_left)
                  telegram("This will junk a bomber\n");
            }
         }
         break;
      case GAME_SCREEN_WORLD:
         if (world_player > -1)
            next_player();		/*	advance to next player	*/
         break;
   }
}

void click_monitor(int num) {
   int player;

   player = get_monitor_player(num);

   if (player > -1) {
      ai_clear_messages();
      if (view == VIEW_PLAYER0 + player)
         queued_view = VIEW_PLAYER0 + me;	/*	reset back to our own country	*/
      else {
         queued_view = VIEW_PLAYER0 + player;
         if (players[player].ai)
            ai_random_message(player);
      }
      door(-1);
   }
}

void click_build() {
   struct action_node *action;

   action = find_action(me, current_turn);
   if (!action)
      action = add_action(me, current_turn);	/*	create new	*/
   action->action = ACTION_BUILD;
   telegram("\nWeapons will be built.\nClick on Nuclear War.\n");
   if (GET_PREVIOUS(me) == ACTION_BUILD)
      telegram("NOTE: You're stockpiling\n");
}

void click_propaganda() {
   struct action_node *action;

   action = find_action(me, current_turn);
   if (!action)
      action = add_action(me, current_turn);	/*	create new	*/
   action->action = ACTION_PROPAGANDA;
   if (action->target_city < 0)
      telegram("\nSelect a target city\nfor your propaganda.\n");
   else
      telegram("\n%s targeted\nfor propaganda.\n", GET_NAME(action->target_player));
}

void click_missile(int mt) {
   struct action_node *action;

   if (!GET_STOCK(me, WEAPON_MISSILE_10MT + mt))
      telegram("\nYou have no missiles\nof that size.\n");
   else {
      action = find_action(me, current_turn);
      if (!action)
         action = add_action(me, current_turn);	/*	create new	*/
      action->action = ACTION_MISSILE_10MT + mt;
      telegram("\n%s\n%d megaton capacity.\n", missile_names[mt], megatons[mt]);
   }
}

void click_bomber(int type) {
   struct action_node *action;

   if (!GET_STOCK(me, WEAPON_NP1) && !GET_STOCK(me, WEAPON_GR2))
      telegram("\nYour arsenal contains\nno bombers at this time.\n");
   else if (!GET_STOCK(me, WEAPON_NP1 + type))
      telegram("\nYou have no bombers\nof that class.\n");
   else {
      action = find_action(me, current_turn);
      if (!action)
         action = add_action(me, current_turn);	/*	create new	*/
      action->action = ACTION_NP1 + type;
      telegram("\n%s\n%d megaton capacity.\n", bomber_names[type], megatons[MEGATON_50 + type]);
   }
}

void click_warhead(int mt) {
   struct action_node *action;
   int ok = 0;

   if (!GET_STOCK(me, WEAPON_WARHEAD_10MT + mt))
      telegram("\nYou have no warheads\nof that size.\n");
   else {
      if (GET_MISSILE(me) < 0 && GET_BOMBER(me) < 0)
         telegram("\nYou must first deploy\na delivery system.\n");
      else {
         action = find_action(me, current_turn);
         if (!action)
            action = add_action(me, current_turn);	/*	create new	*/
         if (action->target_player < 0)
            telegram("\nSelect a target first!\n");
         else {
            if (GET_MISSILE(me) > -1) {
               if (mt > GET_MISSILE(me))
                  telegram("\nWarhead is too large.\nThe missile can only\ncarry %d megatons.\n", megatons[GET_MISSILE(me)]);
               else {
                  action->action = ACTION_WARHEAD_10MT + mt;
                  telegram("\n%s targeted\nby your missile.\n%d megaton payload.\n", GET_NAME(action->target_player), megatons[mt]);
                  ok = 1;
               }
            }
            else {
            /* bomber */
               if (!players[me].mt_left)
                  telegram("\nYour bomber has\nused all its bombs.\nDeploy a new weapon.\n");
               else if (megatons[mt] > players[me].mt_left)
                  telegram("\nWarhead is too large.\nThe bomber can only\ndrop %d megatons.\n", players[me].mt_left);
               else {
                  action->action = ACTION_WARHEAD_10MT + mt;
                  telegram("\n%s targeted\nby your bomber:\n%d megaton payload.\n", GET_NAME(action->target_player), megatons[mt]);
                  ok = 1;
               }
            }
         }
      }
   }

   if (ok) {
      ai_clear_messages();
      queued_view = VIEW_BUTTON;
      door(-1);
   }
}

void click_defense(int type) {
   struct action_node *action;

   if (!GET_STOCK(me, WEAPON_LNDS) && !GET_STOCK(me, WEAPON_MEGA))
      telegram("\nYour arsenal contains\nno defensive weaponry\nat this time.\n");
   else if (!GET_STOCK(me, WEAPON_LNDS + type))
      telegram("\nYour stockpile contains\nno defensive weaponry\nof that type.\n");
   else {
      action = find_action(me, current_turn);
      if (!action)
         action = add_action(me, current_turn);	/*	create new	*/
      action->action = ACTION_LNDS + type;
      telegram("\n%s system\nready for action.\n", type ? "MegaCannon" : "L.N.D.S");
   }
}

void click_city(int player, int city) {
   struct action_node *action;

   if (GET_POPULATION(player, city)) {
      if (player == me)
         telegram("\nYou may not target\nyour own country!\n");
      else {
         action = find_action(me, current_turn);
         if (!action)
            action = add_action(me, current_turn);	/*	create new	*/
         action->target_player = player;
         action->target_city = city;
         if (action->action == ACTION_PROPAGANDA)
            telegram("\n%s targeted\nfor propaganda.\n", GET_NAME(player));
      }
   }
}

void end_turn() {
   struct action_node *action;
   int ok = 0;
   struct socket_node *p;

   action = find_action(me, current_turn);

/* check if we may end turn */
   if (!action)
      telegram("\nSelect an action\nbefore ending turn.\n");
   else {
      switch (action->action) {
         case ACTION_BUILD:
         case ACTION_MISSILE_10MT:
         case ACTION_MISSILE_20MT:
         case ACTION_MISSILE_50MT:
         case ACTION_MISSILE_100MT:
         case ACTION_NP1:
         case ACTION_GR2:
         case ACTION_LNDS:
         case ACTION_MEGA:
            ok = 1;
            break;
         case ACTION_PROPAGANDA:
         case ACTION_WARHEAD_10MT:
         case ACTION_WARHEAD_20MT:
         case ACTION_WARHEAD_50MT:
         case ACTION_WARHEAD_100MT:
            if (action->target_player < 0)
               telegram("\nChoose a target city\nbefore ending turn.\n");
            else
               ok = 1;
            break;
         default:
            telegram("\nSelect an action\nbefore ending turn.\n");
            break;
      }
   }

   if (ok) {
      ai_clear_messages();
      queued_screen = GAME_SCREEN_WORLD;
      door(-1);

   /* send action to server or clients */
      if (server) {
         for (p = socket_list; p != NULL; p = p->next) {
            if (p->type == SOCKET_CLIENT)
               send_action(p, me, current_turn);
         }
      }
      else
         send_action(find_server_socket(), me, current_turn);
   }
}

void add_typing(SDL_keysym *sym) {
   if (typing) {
      if (sym->unicode >= 32 && sym->unicode <= 126) {
         if (strlen(type_str) + 1 < MAX_MESSAGE_LENGTH)
            type_str[strlen(type_str)] = sym->unicode;
      }
   }
}

void draw_typing() {
   char str[BUFFER_SIZE];
   int x, y, i;

   if (typing) {
      sprintf(str, ">%s", type_str);
      if (flash)
         strcat(str, "_");
      else
         strcat(str, " ");

      x = center(0, SCREEN_WIDTH, strlen(str) * FONT_WIDTH);
      y = TELEGRAPH_Y + ((MAX_TELEGRAPH_LINES - 2) * FONT_LEADING);

   /* draw box */
      blit(BMP_INPUT, 0, 0, 8, 20, x - 8, y);
      for (i = 0; i < strlen(str); i++)
         blit(BMP_INPUT, 8, 0, FONT_WIDTH, 20, x + (i * FONT_WIDTH), y);
      blit(BMP_INPUT, 8 + FONT_WIDTH, 0, 8, 20, x + (strlen(str) * FONT_WIDTH), y);

      draw_text(x, y + 2, COL_YELLOW, str);
   }
}

void draw_cursor() {
   switch (SDL_GetAppState()) {
      case SDL_APPMOUSEFOCUS:
      case SDL_APPACTIVE + SDL_APPMOUSEFOCUS:
      case SDL_APPACTIVE + SDL_APPINPUTFOCUS + SDL_APPMOUSEFOCUS:
         blit(BMP_CURSOR, 0, 0, GET_W(BMP_CURSOR), GET_H(BMP_CURSOR), mouse_x, mouse_y);
         break;
   }
}
