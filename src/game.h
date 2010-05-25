/*
  game.h

  For TuxMath
  The main game loop!

  by Bill Kendrick
  bill@newbreedsoftware.com
  http://www.newbreedsoftware.com/


  Part of "Tux4Kids" Project
  http://www.tux4kids.org/
      
  August 26, 2001 - February 18, 2004
*/


#ifndef GAME_H
#define GAME_H

#define MAX_COMETS 10
#define NUM_CITIES 4   /* MUST BE AN EVEN NUMBER! */

#define NUM_BKGDS 8

#define MAX_CITY_COLORS 4

typedef struct laser_type {
  int alive;
  int x1, y1;
  int x2, y2;
} laser_type;

/* Note: both igloos and the original "cities" graphics are handled
   with the "cities" structure.
   hits_left holds the number of hits it can withstand before
   being "dead". If using the original cities graphics,
      2 = with shield,
      1 = without shield,
      0 = dead
   If using the igloo graphics,
      2 = intact,
      1 = half-melted
      0 = melted
*/

/* For both cities/igloos & penguins, the animation state is
   controlled by "status", and "counter" is used for timing.  We also
   have "img" and "layer" so that the image can be pre-planned to have
   a specific rendering order (so that foreground/background issues
   are handled properly). Layer 0 is rendered first, then layer 1, and
   so on. */

typedef struct city_type {
  int hits_left;
  int status, counter;
  int threatened;   /* true if a comet is near */
  int x;
  int img,layer;
} city_type;

typedef struct penguin_type {
  int status, counter;
  int x;
  int img,layer;
} penguin_type;

typedef struct steam_type {
  int status, counter;
  int img,layer;
} steam_type;

#define NUM_SNOWFLAKES 100

typedef struct cloud_type {
  int status;
  int city;
  int x,y;
  int snowflake_x[NUM_SNOWFLAKES];
  int snowflake_y[NUM_SNOWFLAKES];
  int snowflake_size[NUM_SNOWFLAKES];
} cloud_type;

#define GAME_MESSAGE_LENGTH 100

typedef struct {
  int x,y;
  int alpha;
  char message[GAME_MESSAGE_LENGTH];
} game_message;

enum {
  GAME_IN_PROGRESS,
  GAME_OVER_WON,
  GAME_OVER_LOST,
  GAME_OVER_OTHER,
  GAME_OVER_ESCAPE,
  GAME_OVER_WINDOW_CLOSE,
  GAME_OVER_CHEATER,
  GAME_OVER_ERROR
};

/* City animation status types */
enum {
  CITY_PRESENT,
  CITY_EXPLODING,
  CITY_EVAPORATING,
  CITY_REBUILDING,
  CITY_GONE
};

/* Penguin animation status types */
enum {
  PENGUIN_OFFSCREEN,
  PENGUIN_HAPPY,
  PENGUIN_FLAPPING,
  PENGUIN_DUCKING,
  PENGUIN_GRUMPY,
  PENGUIN_WORRIED,
  PENGUIN_WALKING_OFF,
  PENGUIN_WALKING_ON,
  PENGUIN_STANDING_UP,
  PENGUIN_SITTING_DOWN,
  PENGUIN_BOWING
};

/* Steam animation status types */
enum {
  STEAM_OFF,
  STEAM_ON
};

/* Cloud & snowflake animation types */
enum {
  EXTRA_LIFE_OFF,
  EXTRA_LIFE_ON
};

int game(void);
void game_set_start_message(const char*, const char*, const char*, const char*);

/* draw_nums() is used in options.c and factoroids.c/h so need extern linkage */
void draw_nums(const char* str, int x, int y);

/*used in factoroids.c/h*/
int pause_game(void);
//void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
void draw_line(int x1, int y1, int x2, int y2, int r, int g, int b);
void draw_numbers(const char* str, int x, int y);

#endif
