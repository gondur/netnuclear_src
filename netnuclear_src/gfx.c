#include "SDL.h"
#include "main.h"
#include "constants.h"
#include "gfx.h"
#include "misc.h"
struct Ssprites sprites[NUM_BITMAPS];
Uint32 tick;
float timestep;
SDL_Surface *mainscr, *surfaces[NUM_BITMAPS];
const struct {
   char *filename;
   int scale;
} gfx_files[] = {
   { "gfx/cursor.bmp",     1 },
   { "gfx/font.bmp",       1 },
   { "gfx/terminal.bmp",   1 },
   { "gfx/monitor.bmp",    2 },
   { "gfx/leaders.bmp",    2 },
   { "gfx/door.bmp",       1 },
   { "gfx/led.bmp",        2 },
   { "gfx/printhead.bmp",  2 },
   { "gfx/input.bmp",      2 },
   { "gfx/message.bmp",    2 },
   { "gfx/target.bmp",     2 },
   { "gfx/arrow.bmp",      1 },
   { "gfx/silo_big.bmp",   2 },
   { "gfx/button_big.bmp", 2 },
   { "gfx/runway_big.bmp", 2 },
   { "gfx/dish_big.bmp",   2 },
   { "gfx/world.bmp",      2 },
   { "gfx/cities.bmp",     2 },
   { "gfx/factory.bmp",    2 },
   { "gfx/radio.bmp",      2 },
   { "gfx/missile.bmp",    2 },
   { "gfx/missile2.bmp",   2 },
   { "gfx/bomber.bmp",     2 },
   { "gfx/dish.bmp",       2 },
   { "gfx/defector.bmp",   2 },
   { "gfx/explosion.bmp",  2 },
   { "", 0 }
};

/* also used to reset animation between turns */
void init_sprites() {
   int i;

   for (i = 0; i < NUM_BITMAPS; i++)
      set_sprite(i, 0, 0, 0, 0, 0);

   set_sprite(BMP_CURSOR,     44,  44,  0,  0,  0);
   set_sprite(BMP_MONITOR,    122, 74,  0,  0,  0);
   set_sprite(BMP_LEADERS,    64,  64,  3,  45, ANIM_MODE_PINGPONG);
   set_sprite(BMP_TARGET,     32,  32,  6,  2,  ANIM_MODE_PINGPONG);
   set_sprite(BMP_SILO_BIG,   246, 160, 9,  15, ANIM_MODE_ONESHOT);
   set_sprite(BMP_BUTTON_BIG, 246, 160, 0,  0,  0);
   set_sprite(BMP_RUNWAY_BIG, 246, 160, 0,  0,  0);
   set_sprite(BMP_DISH_BIG,   246, 160, 7,  10, ANIM_MODE_PINGPONG);
   set_sprite(BMP_CITIES,     32,  32,  0,  0,  0);
   set_sprite(BMP_FACTORY,    30,  32,  6,  10, ANIM_MODE_NORMAL);
   set_sprite(BMP_RADIO,      32,  42,  6,  10, ANIM_MODE_NORMAL);
   set_sprite(BMP_MISSILE,    24,  38,  17, 3,  ANIM_MODE_ONESHOT);
   set_sprite(BMP_MISSILE2,   82,  62,  0,  0,  0);
   set_sprite(BMP_BOMBER,     32,  32,  0,  0,  0);
   set_sprite(BMP_DISH,       30,  32,  7,  10, ANIM_MODE_PINGPONG);
   set_sprite(BMP_DEFECTOR,   32,  36,  9,  5,  ANIM_MODE_NORMAL);
   set_sprite(BMP_EXPLOSION,  32,  32,  23, 5,  ANIM_MODE_ONESHOT);
}

void set_sprite(int num, int w, int h, int frames, int speed, int mode) {
   sprites[num].w = w;
   sprites[num].h = h;
   sprites[num].frames = frames;
   sprites[num].speed = speed;
   sprites[num].mode = mode;
   sprites[num].frame = 0;
   sprites[num].anim_dir = 1;
   sprites[num].timer = speed;
}

void do_animation() {
   int i;

   for (i = 0; i < NUM_BITMAPS; i++) {
      if (sprites[i].mode != ANIM_MODE_NONE) {
         if (sprites[i].mode != ANIM_MODE_ONESHOT || sprites[i].frame + 1 < sprites[i].frames) {
            if (sprites[i].timer > 0)
               sprites[i].timer -= timestep;
            else {
               while (sprites[i].timer <= 0) {
                  if (sprites[i].anim_dir > 0) {
                     if (sprites[i].frame + 1 < sprites[i].frames)
                        sprites[i].frame++;
                     else {
                        if (sprites[i].mode == ANIM_MODE_PINGPONG) {
                           sprites[i].anim_dir = -1;		/*	reverse	*/
                           sprites[i].frame--;
                        }
                        else
                           sprites[i].frame = 0;
                     }
                  }
                  else {
                     if (sprites[i].frame > 0)
                        sprites[i].frame--;
                     else {
                        sprites[i].anim_dir = 1;	/*	change direction again	*/
                        sprites[i].frame++;
                     }
                  }
                  sprites[i].timer += sprites[i].speed;
               }
            }
         }
      }
   }
}

void init_screen() {
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
      do_error("Failed initializing SDL (\"%s\")", SDL_GetError());

   atexit(SDL_Quit);

   putenv("SDL_VIDEO_CENTERED=center");
   SDL_WM_SetCaption(PROGRAM_NAME, PROGRAM_NAME);

   mainscr = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_SWSURFACE | SDL_ANYFORMAT);
   if (!mainscr)
      do_error("Failed creating main screen surface");

   SDL_ShowCursor(SDL_DISABLE);
}

void load_graphics() {
   int i, x, y, x2, y2;
   SDL_Surface *tmp;
   Uint32 pixel;

   for (i = 0; i < NUM_BITMAPS; i++) {
      if (!file_exists(gfx_files[i].filename))
         do_error("Couldn't find bitmap file \"%s\"", gfx_files[i].filename);
      tmp = SDL_DisplayFormat(SDL_LoadBMP(gfx_files[i].filename));
      if (!tmp)
         do_error("Failed loading bitmap file \"%s\"", gfx_files[i].filename);
      surfaces[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, tmp->w * gfx_files[i].scale, tmp->h * gfx_files[i].scale, mainscr->format->BitsPerPixel, mainscr->format->Rmask, mainscr->format->Gmask, mainscr->format->Bmask, mainscr->format->Amask);
      if (!surfaces[i])
         do_error("Failed creating new surface for bitmap \"%s\"", gfx_files[i].filename);
      for (y = 0; y < tmp->h; y++) {
         for (x = 0; x < tmp->w; x++) {
            pixel = get_pixel(tmp, x, y);
            for (y2 = 0; y2 < gfx_files[i].scale; y2++) {
               for (x2 = 0; x2 < gfx_files[i].scale; x2++)
                  put_pixel(surfaces[i], (x * gfx_files[i].scale) + x2, (y * gfx_files[i].scale) + y2, pixel);
            }
         }
      }
      SDL_FreeSurface(tmp);
      SDL_SetColorKey(surfaces[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(surfaces[i]->format, 255, 0, 255));
   }
}

void blit(int num, int x, int y, int w, int h, int x2, int y2) {
   SDL_Rect src, dst;

   src.x = x;
   src.y = y;
   src.w = w;
   src.h = h;

   dst.x = x2;
   dst.y = y2;

   SDL_BlitSurface(surfaces[num], &src, mainscr, &dst);
}

void draw_text(int x, int y, int col, char *str, ...) {
   va_list args;
   char str2[BUFFER_SIZE];
   int i, x2, y2;

   va_start(args, str);
   vsprintf(str2, str, args);
   va_end(args);

   for (i = 0; i < strlen(str2); i++) {
      if (str2[i] != ' ') {
         y2 = str2[i] / 16;
         x2 = str2[i] - (y2 * 16);
         blit(BMP_FONT, x2 * FONT_WIDTH, (col * FONT_HEIGHT * 16) + y2 * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, x + (i * FONT_WIDTH), y);
      }
   }
}

void render() {
   SDL_Flip(mainscr);

   SDL_Delay(1);
   timestep = (float)(SDL_GetTicks() - tick) / (1000 / FRAMES_PER_SECOND);
   tick = SDL_GetTicks();
}

void save_screenshot() {
   SDL_SaveBMP(mainscr, "screenshot.bmp");
}

Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
   int bpp = surface->format->BytesPerPixel;
   Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

   switch (bpp) {
      case 1:
         return *p;
      case 2:
         return *(Uint16 *)p;
      case 3:
         if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
         else
            return p[0] | p[1] << 8 | p[2] << 16;
      case 4:
         return *(Uint32 *)p;
   }

/* shouldn't happen */
   do_error("get_pixel()");
   return 0;	/*	included to avoid compiler warnings	*/
}

void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
   int bpp = surface->format->BytesPerPixel;
   Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

   switch (bpp) {
      case 1:
         *p = pixel;
         break;
      case 2:
         *(Uint16 *)p = pixel;
         break;
      case 3:
         if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
         }
         else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
         }
         break;
      case 4:
         *(Uint32 *)p = pixel;
         break;
   }
}
