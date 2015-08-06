#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_net.h"
#include "constants.h"
#include "main.h"
#include "input.h"
#include "sfx.h"
#include "misc.h"
#include "players.h"
#include "ai.h"
#include "terminal.h"
#include "world.h"
#include "net.h"
#include "packets.h"

struct Splayers players[NUM_PLAYERS];
int me;		/*	local player	*/
int recv_all_actions(int turn) {
   int i;
   struct action_node *action;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i) && has_cities(i, 0)) {
         action = find_action(i, turn);
         if (!action || action->action == ACTION_NOTHING || (!server && !action->res_recv)) {
            if (!server || !players[i].ai)
               return 0;
         }
      }
   }

   return 1;
}

void check_turn_ready() {
   if (current_turn && !check_gameover(0)) {
      if (recv_all_actions(current_turn)) {
         if ((queued_screen == GAME_SCREEN_WORLD && !door_dir) || screen == GAME_SCREEN_WORLD) {
            if (server) {
               do_ai();		/*	computer players select their actions	*/
               send_ai_actions();
               server_prepare_actions();
            }
            do_actions();
            current_turn++;
         }
      }
   }
}

int check_gameover(int world) {
   int i, num = 0;

   if (current_turn) {
   /* count alive players */
      for (i = 0; i < NUM_PLAYERS; i++) {
         if (IS_ACTIVE(i) && has_cities(i, world))
            num++;
      }

      if (num <= 1)
         return 1;
   }

   return 0;
}

void do_gameover() {
   int winner, i, cities = 0, pop = 0;

/* return to terminal */
   current_turn = 0;
   queued_screen = GAME_SCREEN_TERMINAL;
   ai_clear_messages();
   init_telegraph();

/* message */
   winner = get_winner();
   if (winner < 0)
      telegram("\nNo one wins!\n");
   else {
   /* count cities and population */
      for (i = 0; i < NUM_CITIES; i++) {
         if (GET_WORLD(winner, i)) {
            cities++;
            pop += GET_WORLD(winner, i);
         }
      }
      telegram("\n%s wins with\n%d cities and %d million\ncitizens!\n", GET_NAME(winner), cities, pop);
   }
}

/* assumes we've already checked if the game is over */
int get_winner() {
   int i;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i) && has_cities(i, 1))
         return i;
   }

   return -1;
}

struct action_node *add_action(int player, int turn) {
   struct action_node *newn, *p;

   newn =(struct action_node *)malloc(sizeof(struct action_node ));
   memset(newn, 0, sizeof(struct action_node));

   newn->turn = turn;
   newn->action = ACTION_NOTHING;
   newn->target_player = -1;
   newn->target_city = -1;
   newn->res_recv = 0;

   newn->next = NULL;

   if (players[player].action_list == NULL)
      players[player].action_list = newn;
   else {
      p = players[player].action_list;
      while (p->next != NULL)
         p = p->next;
      p->next = newn;
   }

   return newn;
}

struct action_node *find_action(int player, int turn) {
   struct action_node *p;

   for (p = players[player].action_list; p != NULL; p = p->next) {
      if (p->turn == turn)
         return p;
   }

   return NULL;
}

/* server decides if selected actions are successful or not */
void server_prepare_actions() {
   int i, mt;
   struct action_node *action, *action2;
   struct packet_result packet;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i)) {
         action = find_action(i, current_turn);
         if (action) {
            switch (action->action) {
               case ACTION_PROPAGANDA:
                  action->success = randomize(1, 1);
                  action->res_num = randomize(1, 10);	/*	population number	*/
                  break;
               case ACTION_WARHEAD_10MT:
               case ACTION_WARHEAD_20MT:
               case ACTION_WARHEAD_50MT:
               case ACTION_WARHEAD_100MT:
                  mt = action->action - ACTION_WARHEAD_10MT;
               /* check target for defense */
                  action2 = find_action(action->target_player, current_turn);
                  if (!action2)
                     do_error("server_prepare_actions()");	/*	shouldn't happen	*/
                  if (GET_MISSILE(i) > -1 && (action2->action == ACTION_LNDS || action2->action == ACTION_MEGA))
                     action->success = 0;
                  else if (GET_BOMBER(i) > -1 && action2->action == ACTION_MEGA)
                     action->success = 0;
                  else
                     action->success = 1;
                  action->res_num = randomize(damage[mt * 2], damage[(mt * 2) + 1]);	/*	damage	*/
                  break;
            }

         /* send result to other players */
            packet.turn = swap_int(current_turn);
            packet.player = swap_int(i);
            packet.success = swap_int(action->success);
            packet.res_num = swap_int(action->res_num);
            send_all_sockets(PACKET_RESULT, &packet, sizeof(struct packet_result));
         }
      }
   }
}

/* execute selected actions */
void do_actions() {
   int i, missile, bomber, i2;
   struct action_node *action;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i)) {
         action = find_action(i, current_turn);
         if (action) {
            missile = -1;
            bomber = -1;
            switch (action->action) {
               case ACTION_BUILD:
                  if (has_cities(i, 0))
                     do_build(i);
                  break;
               case ACTION_PROPAGANDA:
                  if (has_cities(i, 0)) {
                     do_propaganda(i);
                  /* random message */
                     if (i == me && players[action->target_player].ai)
                        ai_random_message(action->target_player);
                  }
                  break;
               case ACTION_MISSILE_10MT:
               case ACTION_MISSILE_20MT:
               case ACTION_MISSILE_50MT:
               case ACTION_MISSILE_100MT:
                  if (has_cities(i, 0)) {
                     do_missile(i);
                     missile = action->action - ACTION_MISSILE_10MT;
                  }
                  break;
               case ACTION_WARHEAD_10MT:
               case ACTION_WARHEAD_20MT:
               case ACTION_WARHEAD_50MT:
               case ACTION_WARHEAD_100MT:
                  do_nuke(i);
                  if (!players[i].ai)
                     bomber = GET_BOMBER(i);	/*	don't unset so we can reuse the bomber	*/
                  else {
                     if (players[i].mt_left) {
                     /* check if the ai player has a warhead and enough capacity to reuse the bomber */
                        for (i2 = 0; i2 < 4; i2++) {
                           if (GET_STOCK(i, WEAPON_WARHEAD_10MT + i2) && megatons[i2] <= players[i].mt_left)
                              bomber = GET_BOMBER(i);
                        }
                     }
                  }
               /* random message */
                  if (i == me && players[action->target_player].ai && has_cities(me, 0))
                     ai_random_message(action->target_player);
                  break;
               case ACTION_NP1:
               case ACTION_GR2:
                  if (has_cities(i, 0)) {
                     do_bomber(i);
                     bomber = action->action - ACTION_NP1;
                  }
                  break;
               case ACTION_LNDS:
               case ACTION_MEGA:
                  if (has_cities(i, 0))
                     do_defense(i);
                  break;
            }

            GET_PREVIOUS(i) = action->action;	/*	remember action for next turn	*/
            GET_MISSILE(i) = missile;
            GET_BOMBER(i) = bomber;
         }
      }
   }
}

void do_build(int player) {
   struct action_node *action;
   int i, rnd, num;

   action = find_action(player, current_turn);
   if (!action)
      do_error("do_build()");	/*	shouldn't happen	*/

/* build weapons */
   for (i = 0; i < NUM_WEAPONS; i++)
      action->res_build[i] = 0;
   for (i = 0; i < randomize(3, 4); i++) {
      while (1) {
         rnd = randomize(0, NUM_WEAPONS - 1);
         if (!action->res_build[rnd]) {
            switch (rnd) {
               case WEAPON_MISSILE_10MT:
               case WEAPON_MISSILE_20MT:
               case WEAPON_MISSILE_50MT:
               case WEAPON_MISSILE_100MT:
               case WEAPON_NP1:
               case WEAPON_GR2:
               case WEAPON_LNDS:
                  num = randomize(1, 2);
                  break;
               case WEAPON_WARHEAD_10MT:
               case WEAPON_WARHEAD_20MT:
               case WEAPON_WARHEAD_50MT:
               case WEAPON_WARHEAD_100MT:
                  num = randomize(1, 3);
                  break;
               default:
                  num = 1;
                  break;
            }
            GET_STOCK(player, rnd) += num;	/*	increase stock	*/
            action->res_build[rnd] = num;	/*	remember result	*/
            break;	/*	end while-loop	*/
         }
      }
   }

   if (player == me) {
      telegram("\n%s\n", msgs_build_result[randomize(0, 1)]);
      num = 0;
      for (i = 0; i < NUM_WEAPONS; i++) {
         if (action->res_build[i]) {
            telegram(" %d %s", action->res_build[i], weapon_names[i]);
            if (action->res_build[i] > 1) {
               if (i == WEAPON_LNDS)
                  telegram("'");
               telegram("s");
            }
            telegram("\n");
            num++;
         }
      }
   }
}

void do_propaganda(int player) {
   struct action_node *action;
   int num;

   action = find_action(player, current_turn);
   if (has_cities(player, 0) && has_cities(action->target_player, 0))
      num = action->res_num;	/*	population number set by server	*/
   else
      num = 0;	/*	player or target player is defeated	*/

   if (action->success) {
      action->res_city = get_smallest_city(player);	/*	remember for next_player()	*/
   /* do increase/decrease */
      if (GET_POPULATION(action->target_player, action->target_city) < num)
         num = GET_POPULATION(action->target_player, action->target_city);
      GET_POPULATION(action->target_player, action->target_city) -= num;
      GET_POPULATION(player, action->res_city) += num;

      if (player == me) {
         telegram("\nAfter a propaganda\nstrike, ");
         if (num)
            telegram("%d million", num);
         else
            telegram("No one");
         telegram(" left\n%s for you.\n", GET_NAME(action->target_player));
      }
   }
   else {
   /* remember for next_player() */
      action->res_city = get_biggest_city(player);	
      action->res_target_city = get_smallest_city(action->target_player);
   /* do increase/decrease */
      if (GET_POPULATION(player, action->res_city) < num)
         num = GET_POPULATION(player, action->res_city);
      GET_POPULATION(player, action->res_city) -= num;
      GET_POPULATION(action->target_player, action->res_target_city) += num;

      if (player == me)
         telegram("\nYour propaganda strike\nfailed. %d million\ndefected to %s.\n", num, GET_NAME(action->target_player));
   }

   action->res_num = num;	/*	final result	*/
}

void do_missile(int player) {
   struct action_node *action;
   int mt;

   action = find_action(player, current_turn);
   if (!action)
      do_error("do_missile()");	/*	shouldn't happen	*/
   mt = action->action - ACTION_MISSILE_10MT;

   GET_STOCK(player, WEAPON_MISSILE_10MT + mt)--;	/*	decrease stock	*/

   if (player == me)
      telegram("\n%s\n%d Megatons capacity\nawaits orders.\n", missile_names[mt], megatons[mt]);
}

void do_bomber(int player) {
   struct action_node *action;
   int type;

   action = find_action(player, current_turn);
   if (!action)
      do_error("do_bomber()");	/*	shouldn't happen	*/
   type = action->action - ACTION_NP1;

   GET_STOCK(player, WEAPON_NP1 + type)--;	/*	decrease stock	*/
   players[player].mt_left = 50 + (50 * type);	/*	50 or 100 mt	*/

   if (player == me)
      telegram("\nBomber standing by,\n%d megatons remain.\n", players[player].mt_left);
}

void do_defense(int player) {
   struct action_node *action;

   action = find_action(player, current_turn);
   if (!action)
      do_error("do_defense()");	/*	shouldn't happen	*/

   GET_STOCK(player, WEAPON_LNDS + action->action - ACTION_LNDS)--;

   if (player == me)
      telegram("\nYou deployed a defense\nsystem to protect\nyour population.\n");
}

void do_nuke(int player) {
   struct action_node *action;
   int mt, num=0;

   action = find_action(player, current_turn);
   mt = action->action - ACTION_WARHEAD_10MT;

   GET_STOCK(player, WEAPON_WARHEAD_10MT + mt)--;	/*	decrease stock	*/
   if (GET_BOMBER(player) > -1) {
      if (action->success)
         players[player].mt_left -= megatons[mt];	/*	decrease bomber capacity	*/
      else
         GET_BOMBER(player) = -1;	/*	bomber shot down	*/
   }

   if (action->success) {
      num = action->res_num;
      if (GET_POPULATION(action->target_player, action->target_city) < num)
         num = GET_POPULATION(action->target_player, action->target_city);
      GET_POPULATION(action->target_player, action->target_city) -= num;

      action->res_num = num;	/*	final result	*/
   }

/* telegram */
   if (player == me) {
      if (GET_MISSILE(player) > -1) {
         telegram("\nThe %s\nexploded on %s.\n", missile_names[mt], GET_NAME(action->target_player));
         if (action->success)
            telegram("%d million dead.\n", num);
         else
            telegram("No casualties.\n");
      }
      else {
         telegram("\nYou bombed %s\n", GET_NAME(action->target_player));
         if (action->success && num)
            telegram("for %d million:\n", num);
         else
            telegram("for no casualties.\n");
         if (action->success)
            telegram("%d megatons remain.\n", players[me].mt_left);	/*	bomber still in air	*/
      }
   }
}

void init_player(int player, int option) {
   int i, num;
   struct action_node *next;

   if (option != INIT_PLAYER_INACTIVE)
      players[player].active = 1;
   else
      players[player].active = 0;

   if (option != INIT_PLAYER_RESET) {
      switch (option) {
         case INIT_PLAYER_HUMAN:
            sprintf(GET_NAME(player), "Player %d", get_active_players());
            players[player].ai = 0;
            players[player].portrait = LEADER_RONNIE;
            break;
         case INIT_PLAYER_AI:
            players[player].ai = 1;
            while (1) {
            /* select a random leader but check so it doesn't exist already */
               num = randomize(0, NUM_LEADERS - 1);
               if (player_name_available((char *)leader_names[num]))
                  break;	/*	end while-loop	*/
            }
            strcpy(GET_NAME(player), leader_names[num]);
            players[player].portrait = num;
            break;
      }
      memset(players[player].msg, 0, MAX_MESSAGE_LENGTH+1);
      players[player].msg_timer = 0;
   }

/* population */
   if (option != INIT_PLAYER_RESET || server) {
      for (i = 0; i < NUM_CITIES; i++) {
         if (option == INIT_PLAYER_INACTIVE)
            GET_POPULATION(player, i) = 0;
         else
            GET_POPULATION(player, i) = randomize(10, 35);
         GET_WORLD(player, i) = GET_POPULATION(player, i);	/*	world state	*/
      }
   }

/* weapons */
   for (i = 0; i < NUM_WEAPONS; i++)
      GET_STOCK(player, i) = 0;
   GET_STOCK(player, WEAPON_MISSILE_10MT) = 1;
   GET_STOCK(player, WEAPON_WARHEAD_10MT) = 1;
   GET_STOCK(player, WEAPON_LNDS) = 1;

/* free action list */
   while (players[player].action_list != NULL) {
      next = players[player].action_list->next;
      free(players[player].action_list);
      players[player].action_list = next;
   }

   GET_PREVIOUS(player) = ACTION_NOTHING;
   GET_MISSILE(player) = -1;
   GET_BOMBER(player) = -1;

   GET_WORLD_MISSILE(player) = 0;
   GET_WORLD_BOMBER(player) = 0;
}

int has_cities(int player, int world) {
   int i;

   for (i = 0; i < NUM_CITIES; i++) {
      if ((!world && GET_POPULATION(player, i)) || (world && GET_WORLD(player, i)))
         return 1;
   }

   return 0;
}

int get_smallest_city(int player) {
   int city, num, i;

   city = get_biggest_city(player);
   num = GET_POPULATION(player, city);

   for (i = 0; i < NUM_CITIES; i++) {
      if (GET_POPULATION(player, i) && GET_POPULATION(player, i) < num) {
         num = GET_POPULATION(player, i);
         city = i;
      }
   }

   return city;
}

int get_biggest_city(int player) {
   int i, num = 0, city = 0;

   for (i = 0; i < NUM_CITIES; i++) {
      if (GET_POPULATION(player, i) >= num) {
         num = GET_POPULATION(player, i);
         city = i;
      }
   }

   return city;
}

int get_active_players() {
   int i, num = 0;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i))
         num++;
   }

   return num;
}

int get_free_player() {
   int i = 0;

   if (get_active_players() < NUM_PLAYERS) {
      while (IS_ACTIVE(i))
         i++;
      return i;
   }

   return -1;	/*	no free player slots	*/
}

int player_name_available(char *name) {
   int i;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (players[i].active && compare_string(name, GET_NAME(i)))
         return 0;
   }

   return 1;
}
