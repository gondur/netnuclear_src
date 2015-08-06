#include "SDL.h"
#include "SDL_net.h"
#include "main.h"
#include "constants.h"
#include "input.h"
#include "misc.h"
#include "players.h"
#include "ai.h"
#include "terminal.h"
#include "net.h"
#include "packets.h"

void do_ai() {
   int i, select, ok, rnd, mt;
   struct action_node *action;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i) && players[i].ai && has_cities(i, 0)) {
         select = 0;
         ok = 0;
         while (!ok) {
            rnd = randomize(0, NUM_ACTIONS);
            switch (rnd) {
               case ACTION_BUILD:
                  if (GET_MISSILE(i) < 0 && GET_BOMBER(i) < 0)
                     ok = 1;
                  break;
               case ACTION_PROPAGANDA:
                  if (GET_MISSILE(i) < 0 && GET_BOMBER(i) < 0) {
                     select = 1;	/*	select a target city	*/
                     ok = 1;
                  }
                  break;
               case ACTION_MISSILE_10MT:
               case ACTION_MISSILE_20MT:
               case ACTION_MISSILE_50MT:
               case ACTION_MISSILE_100MT:
                  if (GET_MISSILE(i) < 0 && GET_BOMBER(i) < 0) {
                     mt = rnd - ACTION_MISSILE_10MT;
                  /* do we have a missile and large enough warhead? */
                     if (GET_STOCK(i, WEAPON_MISSILE_10MT + mt)) {
                        if (has_warhead(i, mt))
                           ok = 1;
                     }
                  }
                  break;
               case ACTION_WARHEAD_10MT:
               case ACTION_WARHEAD_20MT:
               case ACTION_WARHEAD_50MT:
               case ACTION_WARHEAD_100MT:
                  mt = rnd - ACTION_WARHEAD_10MT;
                  if (GET_STOCK(i, WEAPON_WARHEAD_10MT + mt)) {
                     if (GET_MISSILE(i) > -1) {
                     /* can the missile carry the warhead? */
                        if (mt <= GET_MISSILE(i)) {
                           select = 1;	/*	select a target city	*/
                           ok = 1;
                        }
                     }
                     else if (GET_BOMBER(i) > -1) {
                     /* does the bomber have enough capacity left? */
                        if (megatons[mt] <= players[i].mt_left) {
                           select = 1;	/*	select a target city	*/
                           ok = 1;
                        }
                     }
                  }
                  break;
               case ACTION_NP1:
               case ACTION_GR2:
                  if (GET_MISSILE(i) < 0 && GET_BOMBER(i) < 0) {
                     if (GET_STOCK(i, WEAPON_NP1 + rnd - ACTION_NP1)) {
                     /* do we have a warhead for the bomber? */
                        if ((rnd == ACTION_NP1 && has_warhead(i, MEGATON_50)) || (rnd == ACTION_GR2 && has_warhead(i, MEGATON_100)))
                           ok = 1;
                     }
                  }
                  break;
               case ACTION_LNDS:
               case ACTION_MEGA:
                  if (ai_use_defense(i, WEAPON_LNDS + rnd - ACTION_LNDS))
                     ok = 1;
                  break;
            }
         }
      /* set action */
         action = add_action(i, current_turn);
         action->action = rnd;
         if (select)
            ai_select_target_city(i, action);
      }
   }
}

void ai_select_target_city(int player, struct action_node *action) {
   int rnd, rnd2;

   while (1) {
      rnd = randomize(0, NUM_PLAYERS - 1);
      if (IS_ACTIVE(rnd) && rnd != player && has_cities(rnd, 0)) {
         rnd2 = randomize(0, NUM_CITIES - 1);
         if (GET_POPULATION(rnd, rnd2)) {
            action->target_player = rnd;
            action->target_city = rnd2;
            break;	/*	end while-loop	*/
         }
      }
   }
}

int ai_use_defense(int player, int type) {
   int i;

   if (GET_MISSILE(player) < 0 && GET_BOMBER(player) < 0) {
      if (GET_STOCK(player, type)) {
      /* do any opponents have a missile prepared? */
         for (i = 0; i < NUM_PLAYERS; i++) {
            if (IS_ACTIVE(i) && has_cities(i, 0) && i != player) {
               if ((type == WEAPON_LNDS && GET_MISSILE(i) > -1) || (type == WEAPON_MEGA && (GET_MISSILE(i) > -1 || GET_BOMBER(i) > -1)))
                  return 1;
            }
         }
      }
   }

   return 0;
}

int has_warhead(int player, int mt) {
   int i = 0;

   while (i <= mt) {
      if (GET_STOCK(player, WEAPON_WARHEAD_10MT + i))
         return 1;
      i++;
   }

   return 0;
}

void send_ai_actions() {
   int i;
   struct socket_node *p;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i) && players[i].ai) {
         for (p = socket_list; p != NULL; p = p->next) {
            if (p->type == SOCKET_CLIENT)
               send_action(p, i, current_turn);
         }
      }
   }
}

/* clear ai players chat messages */
void ai_clear_messages() {
   int i;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (players[i].active && players[i].ai)
         players[i].msg_timer = 0;
   }
}

void ai_random_message(int player) {
   int i = 0;

/* count lines */
   while (strlen(msgs_leaders[i]))
      i++;

   player_message(player, (char *)msgs_leaders[randomize(0, i - 1)], 0);
}
