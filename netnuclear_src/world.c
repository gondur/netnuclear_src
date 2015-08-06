#include <math.h>
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
int world_turn;		/*	game turn currently displayed	*/
int world_player;	/*	player to draw actions for	*/
int world_step;
char action_str[BUFFER_SIZE];
struct Sworld_state world_state[NUM_PLAYERS];
void next_player() {
   struct action_node *action;
   int missile, bomber;

/* skip to next player or just step? */
   if (world_step || world_player < 0) {
      world_player++;
      world_step = 0;
      while (world_player < NUM_PLAYERS) {
         if (!IS_ACTIVE(world_player))
            world_player++;
         else {
         /* skip dead players unless they're nuking */
            if (has_cities(world_player, 1))
               break;	/*	end while-loop	*/
            else {
               action = find_action(world_player, world_turn);
               if (!action || (action->action != ACTION_WARHEAD_10MT && action->action != ACTION_WARHEAD_20MT && action->action != ACTION_WARHEAD_50MT && action->action != ACTION_WARHEAD_100MT))
                  world_player++;
               else
                  break;	/*	end while-loop	*/
            }
         }
      }
   }
   else
      world_step++;	/*	don't skip to next player yet	*/

   if (world_player < NUM_PLAYERS) {
   /* prepare drawing selected action */
      init_sprites();
      missile = 0;
      bomber = 0;
      action = find_action(world_player, world_turn);
      if (!action)
         do_error("next_player()");	/*	shouldn't happen	*/
      switch (action->action) {
         case ACTION_BUILD:
            str_add(action_str, (char *)msgs_build[randomize(0, 4)], GET_NAME(world_player));
            strcat(action_str, "\n");
            play_sample(WAV_BUILD);
            world_step = 1;	/*	done	*/
            break;
         case ACTION_PROPAGANDA:
            if (!world_step) {
               if (action->success) {
                  str_add(action_str, (char *)msgs_propaganda[randomize(0, 3)], GET_NAME(world_player), GET_NAME(action->target_player));
                  strcat(action_str, "\n");
                  play_sample(WAV_RADIO);
               }
               else {
                  str_add(action_str, "%d Million leave %s for %s.\n", action->res_num, GET_NAME(world_player), GET_NAME(action->target_player));
                  init_defector_sprite(world_player);
                  play_sample(WAV_FEET);
               }
            }
            else {
               if (action->success) {
                  if (action->res_num) {
                     GET_WORLD(action->target_player, action->target_city) -= action->res_num;	/*	remove from target	*/
                     GET_WORLD(world_player, action->res_city) += action->res_num;	/*	add to our own city	*/
                     str_add(action_str, "Defectors leave %s for %s.\n", GET_NAME(action->target_player), GET_NAME(world_player));
                  }
                  else
                     str_add(action_str, "No one leaves %s for %s.\n", GET_NAME(action->target_player), GET_NAME(world_player));
               }
               else {
                  GET_WORLD(world_player, action->res_city) -= action->res_num;	/*	remove from our city	*/
                  GET_WORLD(action->target_player, action->res_target_city) += action->res_num;	/*	add to opponents city	*/
                  str_add(action_str, "%s gains %d Million.\n", GET_NAME(action->target_player), action->res_num);
               }
            }
            break;
         case ACTION_MISSILE_10MT:
         case ACTION_MISSILE_20MT:
         case ACTION_MISSILE_50MT:
         case ACTION_MISSILE_100MT:
            missile = 1;
            str_add(action_str, (char *)msgs_prepare_missile[randomize(0, 3)], GET_NAME(world_player));
            strcat(action_str, "\n");
            play_sample(WAV_FANFARE);
            world_step = 1;	/*	done	*/
            break;
         case ACTION_WARHEAD_10MT:
         case ACTION_WARHEAD_20MT:
         case ACTION_WARHEAD_50MT:
         case ACTION_WARHEAD_100MT:
            if (!world_step) {
               if (GET_WORLD_MISSILE(world_player)) {
                  str_add(action_str, (char *)msgs_missile_attack[randomize(0, 2)], GET_NAME(world_player), GET_NAME(action->target_player));
                  strcat(action_str, "\n");
                  init_missile_sprite(world_player);
                  play_sample(WAV_MISSILE);
               }
               else {
                  str_add(action_str, (char *)msgs_bomber_attack[randomize(0, 3)], GET_NAME(world_player), GET_NAME(action->target_player));
                  strcat(action_str, "\n");
                  init_bomber_sprite(world_player, 0);
                  play_sample(WAV_ENGINE2);
               }
            }
            else {
               if (action->success) {
                  GET_WORLD(action->target_player, action->target_city) -= action->res_num;
                  if (GET_WORLD(action->target_player, action->target_city))
                     str_add(action_str, (char *)msgs_nuke[randomize(0, 5) * 2], action->res_num);
                  else
                     str_add(action_str, (char *)msgs_nuke[(randomize(0, 5) * 2) + 1]);	/*	city destroyed	*/
                  strcat(action_str, "\n");
                  if (GET_WORLD_BOMBER(world_player)) {
                     bomber = 1;
                     init_bomber_sprite(world_player, 1);
                  }
                  play_sample(WAV_BOOM);
               }
               else {
                  if (GET_WORLD_MISSILE(world_player))
                     str_add(action_str, "Missile destroyed by ABM. City is safe!\n");
                  else {
                     str_add(action_str, "Bomber Shot down. City is safe!\n");
                     stop_samples();
                  }
               }
            }
            break;
         case ACTION_LNDS:
         case ACTION_MEGA:
            str_add(action_str, (char *)msgs_defend[randomize(0, 3)], GET_NAME(world_player));
            strcat(action_str, "\n");
            play_sample(WAV_DISH);
            world_step = 1;	/*	done	*/
            break;
         case ACTION_NP1:
         case ACTION_GR2:
            bomber = 1;
            str_add(action_str, (char *)msgs_prepare_bomber[randomize(0, 3)], GET_NAME(world_player));
            strcat(action_str, "\n");
            sprites[BMP_BOMBER].angle = 0;	/*	reset sprite	*/
            play_sample(WAV_ENGINE);
            world_step = 1;	/*	done	*/
            break;
      }

      if (world_step) {
         GET_WORLD_MISSILE(world_player) = missile;
         GET_WORLD_BOMBER(world_player) = bomber;
      }
   }
   else {
      if (check_gameover(1))
         do_gameover();
      else {
      /* stay at world screen or return to terminal? */
         world_turn++;
         if (!has_cities(me, 0)) {
            world_player = -1;
            world_step = 0;
            memset(action_str, 0, BUFFER_SIZE);
            if (!recv_all_actions(world_turn)) {
               str_add(action_str, "Waiting for players...\n");
               stop_samples();
            }
            else
               next_player();
         }
         else
            set_screen(GAME_SCREEN_TERMINAL);
      }
   }
}

/*
   0 (destroyed) - crater
   1-4 million - tent
   5-9 million - thatched hut
   10-19 million - cottage
   20-29 million - mansion
   30+ million - metropolis
*/
int get_population_level(int population) {
   if (population >= 30)
      return 5;
   else if (population >= 20)
      return 4;
   else if (population >= 10)
      return 3;
   else if (population >= 5)
      return 2;
   else if (population >= 1)
      return 1;

   return 0;
}

void draw_cities(int player, int x, int y) {
   int i;

   for (i = 0; i < NUM_CITIES; i++)
      blit(BMP_CITIES, get_population_level(GET_WORLD(player, i)) * GET_W(BMP_CITIES), 0, GET_W(BMP_CITIES), GET_H(BMP_CITIES), x + get_city_x(player, i), y + get_city_y(player, i));
}

void draw_world() {
   int i;
   struct action_node *action;

   blit(BMP_WORLD, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

/* draw all cities first */
   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i))
         draw_cities(i, get_country_x(i), get_country_y(i));
   }

/* draw action */
   if (recv_all_actions(world_turn)) {
      if (world_player < 0)
         next_player();
      if (IS_ACTIVE(world_player)) {
         action = find_action(world_player, world_turn);
         if (!action)
            do_error("draw_world()");	/*	shouldn't happen	*/
         switch (action->action) {
            case ACTION_BUILD:
               if (has_cities(world_player, 1))
                  draw_factory(get_country_x(world_player), get_country_y(world_player));
               break;
            case ACTION_PROPAGANDA:
               if (action->success)
                  draw_radio(get_country_x(world_player), get_country_y(world_player));
               else {
                  if (world_step) {
                  /* place defector at destination */
                     sprites[BMP_DEFECTOR].x = sprites[BMP_DEFECTOR].dst_x;
                     sprites[BMP_DEFECTOR].y = sprites[BMP_DEFECTOR].dst_y;
                  }
                  draw_defector(0, 0);
                  if (!world_step && sprites[BMP_DEFECTOR].x == sprites[BMP_DEFECTOR].dst_x && sprites[BMP_DEFECTOR].y == sprites[BMP_DEFECTOR].dst_y)
                     next_player();	/*	defector has reached its destination	*/
               }
               break;
            case ACTION_MISSILE_10MT:
            case ACTION_MISSILE_20MT:
            case ACTION_MISSILE_50MT:
            case ACTION_MISSILE_100MT:
               if (has_cities(world_player, 1))
                  draw_missile_pad(get_country_x(world_player), get_country_y(world_player), 1);
               break;
            case ACTION_WARHEAD_10MT:
            case ACTION_WARHEAD_20MT:
            case ACTION_WARHEAD_50MT:
            case ACTION_WARHEAD_100MT:
               if (!world_step) {
                  if (world_state[world_player].missile_prepared) {
                     draw_missile(world_player);
                     if (sprites[BMP_MISSILE2].x == sprites[BMP_MISSILE2].dst_x && sprites[BMP_MISSILE2].y == sprites[BMP_MISSILE2].dst_y)
                        next_player();	/*	missile has reached its target	*/
                  }
                  else {
                     draw_bomber(world_player);
                     if (sprites[BMP_BOMBER].x == sprites[BMP_BOMBER].dst_x && sprites[BMP_BOMBER].y == sprites[BMP_BOMBER].dst_y)
                        next_player();	/*	bomber has reached its target	*/
                  }
               }
               else {
                  if (action->success)
                     draw_explosion(action->target_player, action->target_city);
                  if (GET_WORLD_BOMBER(world_player))
                     draw_bomber(world_player);
               }
               break;
            case ACTION_LNDS:
            case ACTION_MEGA:
               if (has_cities(world_player, 1))
                  draw_dish(get_country_x(world_player), get_country_y(world_player));
               break;
            case ACTION_NP1:
            case ACTION_GR2:
               if (has_cities(world_player, 1))
                  draw_bomber(world_player);
               break;
         }
      }
   }
}

void draw_factory(int x, int y) {
   blit(BMP_FACTORY, GET_FRAME(BMP_FACTORY) * GET_W(BMP_FACTORY), 0, GET_W(BMP_FACTORY), GET_H(BMP_FACTORY), center(x, x + WINDOW_W, GET_W(BMP_FACTORY)), center(y, y + WINDOW_H, GET_H(BMP_FACTORY)));
}

void draw_radio(int x, int y) {
   blit(BMP_RADIO, GET_FRAME(BMP_RADIO) * GET_W(BMP_RADIO), 0, GET_W(BMP_RADIO), GET_H(BMP_RADIO), center(x, x + WINDOW_W, GET_W(BMP_RADIO)), center(y, y + WINDOW_H, GET_H(BMP_RADIO)));
}

void draw_missile_pad(int x, int y, int animate) {
   int frame;

   if (animate)
      frame = GET_FRAME(BMP_MISSILE);
   else
      frame = sprites[BMP_MISSILE].frames - 1;	/*	last frame	*/
   blit(BMP_MISSILE, frame * GET_W(BMP_MISSILE), 0, GET_W(BMP_MISSILE), GET_H(BMP_MISSILE), center(x, x + WINDOW_W, GET_W(BMP_MISSILE)), center(y, y + WINDOW_H, GET_H(BMP_MISSILE)));
}

void init_missile_sprite(int player) {
   int x, y;
   struct action_node *action;

/* start */
   x = center(get_country_x(player), get_country_x(player) + WINDOW_W, GET_W(BMP_MISSILE2));
   y = center(get_country_y(player), get_country_y(player) + WINDOW_H, GET_H(BMP_MISSILE2));
   sprites[BMP_MISSILE2].x = x;
   sprites[BMP_MISSILE2].y = y;
   sprites[BMP_MISSILE2].start_x = x;
   sprites[BMP_MISSILE2].start_y = y;

/* destination */
   action = find_action(player, world_turn);
   x = get_country_x(action->target_player);
   y = get_country_y(action->target_player);
   x += center(get_city_x(action->target_player, action->target_city), get_city_x(action->target_player, action->target_city) + GET_W(BMP_CITIES), GET_W(BMP_MISSILE2));
   y += center(get_city_y(action->target_player, action->target_city), get_city_y(action->target_player, action->target_city) + GET_H(BMP_CITIES), GET_H(BMP_MISSILE2));
   sprites[BMP_MISSILE2].dst_x = x;
   sprites[BMP_MISSILE2].dst_y = y;
}

void draw_missile(int player) {
   int x, y, dst_x, dst_y, start_x, start_y, total, left, frame, row = 0;

   x = sprites[BMP_MISSILE2].x;
   y = sprites[BMP_MISSILE2].y;
   dst_x = sprites[BMP_MISSILE2].dst_x;
   dst_y = sprites[BMP_MISSILE2].dst_y;
   start_x = sprites[BMP_MISSILE2].start_x;
   start_y = sprites[BMP_MISSILE2].start_y;

/* calculate frame to use from distance between missile and target */
   total = get_distance(start_x, start_y, dst_x, dst_y);
   left = get_distance(x, y, dst_x, dst_y);
   frame = ((float)(total - left) / total) * 9;

   if (sprites[BMP_MISSILE2].dst_x < sprites[BMP_MISSILE2].start_x)
      row = 1;
   blit(BMP_MISSILE2, frame * GET_W(BMP_MISSILE2), row * GET_H(BMP_MISSILE2), GET_W(BMP_MISSILE2), GET_H(BMP_MISSILE2), (int)sprites[BMP_MISSILE2].x, (int)sprites[BMP_MISSILE2].y);

   move_sprite_to_destination(BMP_MISSILE2, 2);
}

/* initialize attacking bomber */
void init_bomber_sprite(int player, int dir) {
   int x, y;
   struct action_node *action;

/* start */
   x = center(get_country_x(player), get_country_x(player) + WINDOW_W, GET_W(BMP_BOMBER));
   y = center(get_country_y(player), get_country_y(player) + WINDOW_H, GET_H(BMP_BOMBER));
   if (!dir) {
      sprites[BMP_BOMBER].x = x;
      sprites[BMP_BOMBER].y = y;
      sprites[BMP_BOMBER].start_x = x;
      sprites[BMP_BOMBER].start_y = y;
   }
   else {
   /* flying back */
      sprites[BMP_BOMBER].x = sprites[BMP_BOMBER].dst_x;
      sprites[BMP_BOMBER].y = sprites[BMP_BOMBER].dst_y;
      sprites[BMP_BOMBER].start_x = sprites[BMP_BOMBER].dst_x;
      sprites[BMP_BOMBER].start_y = sprites[BMP_BOMBER].dst_y;
   }

/* destination */
   if (!dir) {
      action = find_action(player, world_turn);
      x = get_country_x(action->target_player);
      y = get_country_y(action->target_player);
      x += center(get_city_x(action->target_player, action->target_city), get_city_x(action->target_player, action->target_city) + GET_W(BMP_CITIES), GET_W(BMP_BOMBER));
      y += center(get_city_y(action->target_player, action->target_city), get_city_y(action->target_player, action->target_city) + GET_H(BMP_CITIES), GET_H(BMP_BOMBER));
   }
   sprites[BMP_BOMBER].dst_x = x;
   sprites[BMP_BOMBER].dst_y = y;
}

void draw_bomber(int player) {
   struct action_node *action;
   int x, y;
   float angle;

   action = find_action(player, world_turn);
   if (!action)
      do_error("draw_bomber()");	/*	shouldn't happen	*/
   switch (action->action) {
      case ACTION_NP1:
      case ACTION_GR2:
      /* idle bomber */
         x = center(get_country_x(player), get_country_x(player) + WINDOW_W, GET_W(BMP_BOMBER));
         y = center(get_country_y(player), get_country_y(player) + WINDOW_H, GET_H(BMP_BOMBER));
         angle = sprites[BMP_BOMBER].angle * (PI / 180);
         x += sin(angle) * 40;
         y -= cos(angle) * 40;
      /* correct angle for get_bomber_frame() */
         angle = sprites[BMP_BOMBER].angle + 90;
         if (angle >= 360)
            angle -= 360;
         blit(BMP_BOMBER, get_bomber_frame(angle) * GET_W(BMP_BOMBER), 0, GET_W(BMP_BOMBER), GET_H(BMP_BOMBER), x, y);
      /* update circle movement */
         sprites[BMP_BOMBER].angle += 2 * timestep;
         while (sprites[BMP_BOMBER].angle >= 360)
            sprites[BMP_BOMBER].angle -= 360;
         break;
      case ACTION_WARHEAD_10MT:
      case ACTION_WARHEAD_20MT:
      case ACTION_WARHEAD_50MT:
      case ACTION_WARHEAD_100MT:
      /* attacking bomber */
         move_sprite_to_destination(BMP_BOMBER, 2);	/*	move first to update the angle	*/
         blit(BMP_BOMBER, get_bomber_frame(sprites[BMP_BOMBER].angle) * GET_W(BMP_BOMBER), 0, GET_W(BMP_BOMBER), GET_H(BMP_BOMBER), (int)sprites[BMP_BOMBER].x, (int)sprites[BMP_BOMBER].y);
         break;
   }
}

int get_bomber_frame(float angle) {
   if (angle >= 337.5)
      return 0;
   else if (angle >= 292.5)
      return 7;
   else if (angle >= 247.5)
      return 6;
   else if (angle >= 202.5)
      return 5;
   else if (angle >= 157.5)
      return 4;
   else if (angle >= 112.5)
      return 3;
   else if (angle >= 67.5)
      return 2;
   else if (angle >= 22.5)
      return 1;

   return 0;
}

void init_defector_sprite(int player) {
   int x, y;
   struct action_node *action;

/* start */
   x = center(get_country_x(player), get_country_x(player) + WINDOW_W, GET_W(BMP_DEFECTOR));
   y = center(get_country_y(player), get_country_y(player) + WINDOW_H, GET_H(BMP_DEFECTOR));
   sprites[BMP_DEFECTOR].x = x;
   sprites[BMP_DEFECTOR].y = y;
   sprites[BMP_DEFECTOR].start_x = x;
   sprites[BMP_DEFECTOR].start_y = y;

/* destination */
   action = find_action(player, world_turn);
   if (!action)
      do_error("init_defector_sprite()");	/*	shouldn't happen	*/
   sprites[BMP_DEFECTOR].dst_x = center(get_country_x(action->target_player), get_country_x(action->target_player) + WINDOW_W, GET_W(BMP_DEFECTOR));
   sprites[BMP_DEFECTOR].dst_y = center(get_country_y(action->target_player), get_country_y(action->target_player) + WINDOW_H, GET_H(BMP_DEFECTOR));
}

void draw_defector(int x, int y) {
   int row = 0, x2, y2;

   if (screen == GAME_SCREEN_WORLD) {
      if (sprites[BMP_DEFECTOR].dst_x < sprites[BMP_DEFECTOR].start_x)
         row = 1;
      x2 = sprites[BMP_DEFECTOR].x;
      y2 = sprites[BMP_DEFECTOR].y;
   }
   else {
      x2 = center(x, x + WINDOW_W, GET_W(BMP_DEFECTOR));
      y2 = center(y, y + WINDOW_H, GET_H(BMP_DEFECTOR));
   }
   blit(BMP_DEFECTOR, GET_FRAME(BMP_DEFECTOR) * GET_W(BMP_DEFECTOR), row * GET_H(BMP_DEFECTOR), GET_W(BMP_DEFECTOR), GET_H(BMP_DEFECTOR), x2, y2);

   if (screen == GAME_SCREEN_WORLD)
      move_sprite_to_destination(BMP_DEFECTOR, 0.75f);	/*	update position	*/
}

void draw_dish(int x, int y) {
   blit(BMP_DISH, GET_FRAME(BMP_DISH) * GET_W(BMP_DISH), 0, GET_W(BMP_DISH), GET_H(BMP_DISH), center(x, x + WINDOW_W, GET_W(BMP_DISH)), center(y, y + WINDOW_H, GET_H(BMP_DISH)));
}

void draw_explosion(int player, int city) {
   int x, y;

   x = get_country_x(player) + get_city_x(player, city);
   y = get_country_y(player) + get_city_y(player, city);

   blit(BMP_EXPLOSION, GET_FRAME(BMP_EXPLOSION) * GET_W(BMP_EXPLOSION), 0, GET_W(BMP_EXPLOSION), GET_H(BMP_EXPLOSION), x, y);
}

void draw_action_message() {
   int i, brk = 0, len, i2 = 0, y = 0;
   char str[BUFFER_SIZE];

/* count line breaks */
   for (i = 0; i < strlen(action_str); i++) {
      if (action_str[i] == '\n')
         brk++;
   }

/* remove lines if too many */
   while (brk > NUM_MESSAGE_LINES) {
      i = 0;
      while (action_str[i] != '\n')
         i++;
      len = strlen(action_str) - i - 1;
      memcpy(action_str, action_str + i + 1, len);
      memset(action_str + len, 0, BUFFER_SIZE - len);
      brk--;
   }

/* draw */
   for (i = 0; i < strlen(action_str); i++) {
      if (action_str[i] != '\n') {
      /* gather letters until line break */
         str[i2] = action_str[i];
         i2++;
      }
      else {
         str[i2] = '\0';
         draw_text(MESSAGE_X, MESSAGE_Y + (NUM_MESSAGE_LINES - brk + y) * FONT_LEADING, COL_YELLOW, str);
         y++;
         i2 = 0;
      }
   }
}

void move_sprite_to_destination(int num, float speed) {
   float x, y, angle;

/* remember current position before moving */
   x = sprites[num].x;
   y = sprites[num].y;

/* find angle */
   angle = atan2(sprites[num].dst_y - sprites[num].y, sprites[num].dst_x - sprites[num].x);
   angle += 90 * (PI / 180);
/* store angle (as degree) for future use */
   sprites[num].angle = angle * (180 / PI);
   if (sprites[num].angle < 0)
      sprites[num].angle += 360;

   sprites[num].x += sin(angle) * speed * timestep;
   sprites[num].y -= cos(angle) * speed * timestep;

/* correct position if moved too far */
   if (x <= sprites[num].dst_x && sprites[num].x > sprites[num].dst_x)
      sprites[num].x = sprites[num].dst_x;
   else if (x >= sprites[num].dst_x && sprites[num].x < sprites[num].dst_x)
      sprites[num].x = sprites[num].dst_x;
   if (y <= sprites[num].dst_y && sprites[num].y > sprites[num].dst_y)
      sprites[num].y = sprites[num].dst_y;
   else if (y >= sprites[num].dst_y && sprites[num].y < sprites[num].dst_y)
      sprites[num].y = sprites[num].dst_y;
}

int get_country_x(int player) {
   return country_positions[player * 2];
}

int get_country_y(int player) {
   return country_positions[(player * 2) + 1];
}

int get_city_x(int player, int city) {
   return city_positions[(player * 10) + (city * 2)];
}

int get_city_y(int player, int city) {
   return city_positions[(player * 10) + (city * 2) + 1];
}
