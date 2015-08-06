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

#include <proto/utility.h>
int view, queued_view;	/*	current view	*/

int flash;
float flash_timer;

int door_dir;	/*	direction	*/
float door_y;

/* red portrait arrow */
int arrow_x, arrow_dir;
float arrow_timer;

char tele_buf[BUFFER_SIZE], tele_printed[BUFFER_SIZE];
float tele_timer;
int prev_tele_sample;

const int monitor_positions[] = {
   8,   6,
   8,   380,
   510, 6,
   510, 380,
   -1
};

const int led_positions[] = {
   120, 118,	/*	10 mt missile	*/
   120, 136,	/*	20 mt missile	*/
   120, 154,	/*	50 mt missile	*/
   120, 172,	/*	100 mt missile	*/
   120, 216,	/*	10 mt warhead	*/
   120, 234,	/*	20 mt warhead	*/
   120, 252,	/*	50 mt warhead	*/
   120, 270,	/*	100 mt warhead	*/
   510, 124,	/*	np-1	*/
   510, 160,	/*	gr-2	*/
   510, 220,	/*	lnds	*/
   510, 256,	/*	mega	*/
   -1
};

const int chat_positions[] = {
   126, 82,
   126, 456,
   514, 82,	/*	X is right side of bubble	*/
   514, 456,	/*	same	*/
   -1
};

void draw_chat() {
   int i, x, y, row = 0, i2;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (strlen(players[i].msg)) {
         x = chat_positions[get_player_monitor(i) * 2];
         y = chat_positions[(get_player_monitor(i) * 2) + 1];

      /* draw chat bubbles */
         if (get_player_monitor(i) >= 2) {
         /* right-aligned */
            x -= 32 + ((strlen(players[i].msg) + 1) * FONT_WIDTH);
            row = 1;
         }
         blit(BMP_MESSAGE, 0, row * 24, 16, 24, x, y);
         for (i2 = 0; i2 < strlen(players[i].msg) + 1; i2++)
            blit(BMP_MESSAGE, 16, row * 24, FONT_WIDTH, 24, x + 16 + (i2 * FONT_WIDTH), y);
         blit(BMP_MESSAGE, 16 + FONT_WIDTH, row * 24, 16, 24, x + 16 + ((strlen(players[i].msg) + 1) * FONT_WIDTH), y);

         draw_text(x + 16, y + 4, COL_RED, players[i].msg);

      /* update timer */
         players[i].msg_timer -= timestep;
         if (players[i].msg_timer <= 0)
            memset(players[i].msg, 0, MAX_MESSAGE_LENGTH+1);
      }
   }
}

void draw_terminal() {
   int i, x, y, player, col, i2;
   SDL_Rect clip;
   struct action_node *action;

   blit(BMP_TERMINAL, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

/* draw portraits and names */
   for (i = 0; i < 4; i++) {
      x = monitor_positions[i * 2];
      y = monitor_positions[(i * 2) + 1];
      player = get_monitor_player(i);
      if (player > -1 && current_turn && has_cities(player, 0)) {
         blit(BMP_LEADERS, GET_FRAME(BMP_LEADERS) * GET_W(BMP_LEADERS), players[player].portrait * GET_H(BMP_LEADERS), GET_W(BMP_LEADERS), GET_H(BMP_LEADERS), x + 18, y + 4);

      /* portrait arrow */
         if (view == VIEW_PLAYER0 + player) {
            clip.x = x + 84;
            clip.y = y + 36;
            clip.w = 32;
            clip.h = 30;
            SDL_SetClipRect(mainscr, &clip);
            blit(BMP_ARROW, 0, 0, 32, 30, clip.x + arrow_x, clip.y);
            SDL_SetClipRect(mainscr, NULL);
         }
      }
      else
         blit(BMP_MONITOR, 0, 0, GET_W(BMP_MONITOR), GET_H(BMP_MONITOR), x, y);	/*	inactive or defeated	*/

      if (player > -1) {
         col = COL_YELLOW;
      /* flash name? */
         if (queued_screen == GAME_SCREEN_WORLD && IS_ACTIVE(player) && !players[player].ai && has_cities(player, 0)) {
            action = find_action(player, current_turn);
            if (!action || action->action == ACTION_NOTHING)
               col = COL_YELLOW + flash;
         }
      /* draw font with shadow */
         for (i2 = 1; i2 >= 0; i2--)
            draw_text(center(x, x + 122, strlen(GET_NAME(player)) * FONT_WIDTH) + (i2 * 2), y + 82 + (i2 * 2), i2 ? COL_BLACK : col, GET_NAME(player));
      }
   }

   if (current_turn) {
   /* draw weapon indicators */
      for (i = 0; i < NUM_WEAPONS; i++) {
         if (GET_STOCK(me, i))
            blit(BMP_LED, 0, 0, 10, 10, led_positions[i * 2], led_positions[(i * 2) + 1]);
      }

      switch (view) {
         case VIEW_PLAYER0:
         case VIEW_PLAYER1:
         case VIEW_PLAYER2:
         case VIEW_PLAYER3:
         case VIEW_PLAYER4:
            draw_player_country();
            action = find_action(me, current_turn);
            if (action && view == action->target_player) {
            /* draw target arrows */
               x = get_city_x(action->target_player, action->target_city);
               y = get_city_y(action->target_player, action->target_city);
               blit(BMP_TARGET, GET_FRAME(BMP_TARGET) * GET_W(BMP_TARGET), 0, GET_W(BMP_TARGET), GET_H(BMP_TARGET), WINDOW_X + x, WINDOW_Y + y);
            }
            break;
         case VIEW_SILO:
            blit(BMP_SILO_BIG, GET_FRAME(BMP_SILO_BIG) * GET_W(BMP_SILO_BIG), 0, GET_W(BMP_SILO_BIG), GET_H(BMP_SILO_BIG), WINDOW_X, WINDOW_Y);
            break;
         case VIEW_BUTTON:
            blit(BMP_BUTTON_BIG, 0, 0, GET_W(BMP_BUTTON_BIG), GET_H(BMP_BUTTON_BIG), WINDOW_X, WINDOW_Y);
            break;
         case VIEW_RUNWAY:
            blit(BMP_RUNWAY_BIG, 0, 0, GET_W(BMP_RUNWAY_BIG), GET_H(BMP_RUNWAY_BIG), WINDOW_X, WINDOW_Y);
            break;
         case VIEW_DISH:
            blit(BMP_DISH_BIG, GET_FRAME(BMP_DISH_BIG) * GET_W(BMP_DISH_BIG), 0, GET_W(BMP_DISH_BIG), GET_H(BMP_DISH_BIG), WINDOW_X, WINDOW_Y);
            break;
      }
   }

/* update portrait arrow */
   if (arrow_timer > 0)
      arrow_timer -= timestep;
   else {
      while (arrow_timer <= 0) {
         arrow_x += arrow_dir;
         if (!arrow_x || arrow_x == 32)
            arrow_dir *= -1;	/*	reverse direction	*/
         arrow_timer += 0.5f;
      }
   }

/* update flashing names */
   if (flash_timer > 0)
      flash_timer -= timestep;
   else {
      if (!flash)
         flash = 1;
      else
         flash = 0;
      flash_timer += FRAMES_PER_SECOND / 2;
   }
}

void draw_player_country() {
   int player, prev, x, y, i, i2;
   struct action_node *action;
   char str[BUFFER_SIZE];

   player = view - VIEW_PLAYER0;

   blit(BMP_WORLD, get_country_x(player), get_country_y(player), WINDOW_W, WINDOW_H, WINDOW_X, WINDOW_Y);

   if (IS_ACTIVE(player)) {
      draw_cities(player, WINDOW_X, WINDOW_Y);

      if (has_cities(player, 0)) {
      /* find previous action */
         if (world_state[player].missile_prepared)
            prev = ACTION_MISSILE_10MT;		/*	which one isn't important here	*/
         else if (world_state[player].bomber_prepared)
            prev = ACTION_NP1;		/*	which one isn't important here	*/
         else
            prev = GET_PREVIOUS(player);
      /* draw previous action */
         switch (prev) {
            case ACTION_BUILD:
               draw_factory(WINDOW_X, WINDOW_Y);
               break;
            case ACTION_PROPAGANDA:
               action = find_action(player, current_turn - 1);
               if (!action)
                  do_error("draw_player_country()");	/*	shouldn't happen	*/
               if (action->success)
                  draw_radio(WINDOW_X, WINDOW_Y);
               else
                  draw_defector(WINDOW_X, WINDOW_Y);
               break;
            case ACTION_MISSILE_10MT:
            case ACTION_MISSILE_20MT:
            case ACTION_MISSILE_50MT:
            case ACTION_MISSILE_100MT:
               draw_missile_pad(WINDOW_X, WINDOW_Y, 0);
               break;
            case ACTION_LNDS:
            case ACTION_MEGA:
               draw_dish(WINDOW_X, WINDOW_Y);
               break;
            case ACTION_NP1:
            case ACTION_GR2:
               x = center(WINDOW_X, WINDOW_X + WINDOW_W, GET_W(BMP_BOMBER));
               y = center(WINDOW_Y, WINDOW_Y + WINDOW_H, GET_H(BMP_BOMBER));
               blit(BMP_BOMBER, 0, 0, GET_W(BMP_BOMBER), GET_H(BMP_BOMBER), x, y);
               break;
         }

      /* draw population numbers */
         if (!door_dir) {
            for (i = 0; i < NUM_CITIES; i++) {
               if (GET_WORLD(player, i)) {
                  x = get_city_x(player, i);
                  y = get_city_y(player, i);
                  sprintf(str, "%d", GET_WORLD(player, i));
               /* draw font with shadow */
                  for (i2 = 1; i2 >= 0; i2--)
                     draw_text(WINDOW_X + center(x, x + 32, strlen(str) * FONT_WIDTH) + (i2 * 2), WINDOW_Y + y + 32 + (i2 * 2), COL_WHITE - i2, str);
               }
            }
         }
      }
   }
}

void init_door() {
   door_dir = 0;
   door_y = 0;
}

void door(int dir) {
   door_dir = dir;
   if (dir < 0)
      door_y = DOOR_H;	/*	open	*/
   else
      door_y = 0;	/*	closed	*/

   if (dir)
      play_sample(WAV_DOOR);
}

void draw_door() {
   SDL_Rect clip;
   float speed = DOOR_SPEED;

   if (door_y < DOOR_H) {
      clip.x = WINDOW_X;
      clip.y = WINDOW_Y;
      clip.w = WINDOW_W;
      clip.h = WINDOW_H;
      SDL_SetClipRect(mainscr, &clip);
      blit(BMP_DOOR, 0, 0, WINDOW_W, DOOR_H, WINDOW_X, WINDOW_Y - (int)door_y);	/*	top	*/
      blit(BMP_DOOR, 0, DOOR_H, WINDOW_W, DOOR_H, WINDOW_X, WINDOW_Y + DOOR_H + (int)door_y);	/*	bottom	*/
      SDL_SetClipRect(mainscr, NULL);
   }

   if (door_dir) {
   /* update door */
      if (door_dir > 0)
         speed /= 1.5f;		/*	open slightly slower	*/

      door_y += door_dir * speed * timestep;
      if (door_dir < 0 && door_y <= 0) {
         door_y = 0;
         door_dir = 0;
      }
      else if (door_dir > 0 && door_y >= DOOR_H) {
         door_y = DOOR_H;
         door_dir = 0;
      }
   }
}

/* chat message */
void player_message(int player, char *str, int sound) {
   strcpy(players[player].msg, str);
   players[player].msg_timer = 15 * FRAMES_PER_SECOND;	/*	display for 15 seconds	*/
   if (sound)
      play_sample(WAV_CHAT);
}

/* place a message on the telegraph */
void telegram(char *str, ...) {
   va_list args;
   char tmp[BUFFER_SIZE];
   memset(tmp, 0, BUFFER_SIZE);
   va_start(args, str);
   vsprintf(tmp, str, args);
   va_end(args);

   strcat(tele_buf, tmp);
}

void draw_telegraph() {
   int i, sfx = 0, sample, brk = 0, len, x = 0, y = 0;

   if (!door_dir) {
      if (tele_timer > 0)
         tele_timer -= timestep;
      else {
         while (tele_timer <= 0) {
            if (strlen(tele_buf)) {
               tele_printed[strlen(tele_printed)] = tele_buf[0];
               for (i = 0; i < strlen(tele_buf); i++)
                  tele_buf[i] = tele_buf[i + 1];
               tele_buf[i] = '\0';
               
               sfx = 1;
            }
            tele_timer += TELEGRAPH_SPEED;
         }
         if (sfx) {
            if (strlen(tele_printed) == 1 || tele_printed[strlen(tele_printed) - 1] == '\n')
               play_sample(WAV_TELESTOP);
            else {
            /* attempt to randomize the printer sound a little */
               while (1) {
                  sample = randomize(WAV_TELE1, WAV_TELE5);
                  if (sample != prev_tele_sample)
                     break;
               }
               prev_tele_sample = sample;
               play_sample(sample);
            }
         }
      }
   }

/* count line breaks */
   for (i = 0; i < strlen(tele_printed); i++) {
      if (tele_printed[i] == '\n')
         brk++;
   }

/* remove lines if too many */
   while (brk > MAX_TELEGRAPH_LINES - 1) {
      i = 0;
      while (tele_printed[i] != '\n')
         i++;
      len = strlen(tele_printed) - i - 1;
      
      memcpy(tele_printed, tele_printed + i + 1 , len);
      memset(tele_printed + len, 0, BUFFER_SIZE - len);
      
      brk--;
   }

   for (i = 0; i < strlen(tele_printed); i++) {
      if (tele_printed[i] == '\n') {
         y++;
         x = 0;
      }
      else {
         draw_text(TELEGRAPH_X + (x * FONT_WIDTH), TELEGRAPH_Y + ((MAX_TELEGRAPH_LINES - 1 + y) * FONT_LEADING) - (brk * FONT_LEADING), COL_BLACK, "%c", tele_printed[i]);
         x++;
      }
   }

   blit(BMP_PRINTHEAD, 0, 0, 24, 22, TELEGRAPH_X + (x * FONT_WIDTH), TELEGRAPH_Y + ((MAX_TELEGRAPH_LINES - 1) * FONT_LEADING) + 2);
   
}

void init_telegraph() {
   memset(tele_buf, 0, BUFFER_SIZE);
   memset(tele_printed, 0, BUFFER_SIZE);
   tele_timer = 0;
}

/* return which monitor belongs to the player */
int get_player_monitor(int player) {
   int i, mon = 0;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i) && i != me) {
         if (i != player)
            mon++;
         else
            return mon;
      }
   }

   return -1;
}

/* return which player is in the monitor */
int get_monitor_player(int num) {
   int i, i2 = 0;

   for (i = 0; i < NUM_PLAYERS; i++) {
      if (IS_ACTIVE(i) && i != me) {
         if (i2 < num)
            i2++;
         else
            return i;
      }
   }

   return -1;
}
