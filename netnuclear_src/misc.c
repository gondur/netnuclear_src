#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "constants.h"
#include "SDL.h"

int    swap_int (int l)
{
	char    b1,b2,b3,b4;
	int lng=0;

	if (SDL_BYTEORDER != SDL_BIG_ENDIAN) return l;
	b1 = l&0xff;
	b2 = (l>>8)&0xff;
	b3 = (l>>16)&0xff;
	b4 = (l>>24)&0xff;
	lng=lng|(b1&0xff); lng=lng<<8;
	lng=lng|(b2&0xff); lng=lng<<8;
	lng=lng|(b3&0xff); lng=lng<<8;
	lng=lng|(b4&0xff);
	return lng;
}

/* improved strcat() */
void str_add(char *str, char *str2, ...) {
   va_list args;
   char tmp[BUFFER_SIZE];

   va_start(args, str2);
   vsprintf(tmp, str2, args);
   va_end(args);

   strcat(str, tmp);
}

char *get_word(char *str, int num) {
   int i, cur = 0, i2 = 0;
   static char word[BUFFER_SIZE];

   for (i = 0; i < strlen(str); i++) {
      if (str[i] != ' ') {
         if (i && str[i - 1] == ' ')
            cur++;	/*	next word begins	*/
         if (cur == num) {
            word[i2] = str[i];
            i2++;
         }
      }
   }

   word[i2] = '\0';

   return (word);
}

/* case insensitive match */
int compare_string(char *str, char *str2) {
   int i;

   if (strlen(str) != strlen(str2))
      return 0;
   for (i = 0; i < strlen(str); i++) {
      if (tolower(str[i]) != tolower(str2[i]))
         return 0;
   }

   return 1;
}

/* return the distance between two pixel points */
int get_distance(int x, int y, int x2, int y2) {
   int dx, dy;

   dx = x - x2;
   dy = y - y2;

   return sqrt((dx * dx) + (dy * dy));
}

int center(int pos, int pos2, int size) {
   return pos + ((pos2 - pos - size) / 2);
}

int randomize(int min, int max) {
   return rand()%(max - min + 1) + min;
}

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
   if (x1 + w1 - 1 >= x2 && x1 <= x2 + w2 - 1) {
      if (y1 + h1 - 1 >= y2 && y1 <= y2 + h2 - 1)
         return 1;
   }

   return 0;
}

void do_error(char *str, ...) {
   va_list args;
   char tmp[BUFFER_SIZE];
   FILE *fp;

   va_start(args, str);
   vsprintf(tmp, str, args);
   va_end(args);

   fp = fopen(PROGRAM_ERRORLOG, "w");
   if (fp) {
      fputs("ERROR: ", fp);
      fputs(tmp, fp);
      fputs("\n", fp);
      fclose(fp);
   }

   exit(EXIT_FAILURE);
}

int file_exists(char *file) {
   FILE *fp;

   fp = fopen(file, "r");
   if (fp) {
      fclose(fp);
      return 1;
   }

   return 0;
}
