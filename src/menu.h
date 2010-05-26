/*
  menu.h

  Functions responsible for loading, parsing and displaying game menu.
  (interface)

  Part of "Tux4Kids" Project
  http://www.tux4kids.com/

  Author: Boleslaw Kulbabinski <bkulbabinski@gmail.com>, (C) 2009

  Copyright: See COPYING file that comes with this distribution.
*/

#ifndef MENU_H
#define MENU_H

#include "globals.h"
#include "loaders.h"

#include "SDL.h"
/* titlescreen & menu frame rate */
#define MAX_FPS                    30
/* number of "real" frames per one sprite frame */
#define SPRITE_FRAME_DELAY         6

/* these are all menu choices that are available in tuxmath.
   By using a define we can create both an enum and
   a string array without writing these names twice */
#define QUIT -2
#define STOP -1

#define ACTIVITIES \
  X( RUN_QUIT ),\
  X( RUN_ARCADE ),\
  X( RUN_MAIN_MENU ),\
  X( RUN_SCORE_SWEEP ),\
  X( RUN_HELP ),\
  X( RUN_DEMO ),\
  X( RUN_INFO ),\
  X( RUN_CREDITS ),\
  X( RUN_HALL_OF_FAME ),\
  X( N_OF_ACTIVITIES )  /* this one has to be the last one */

/* create enum */
#define X(name) name
enum { ACTIVITIES };
#undef X

struct mNode {
  struct mNode* parent;

  char* title;
  int font_size;

  char* icon_name;
  sprite* icon;

  SDL_Rect button_rect;
  SDL_Rect icon_rect;
  SDL_Rect text_rect;

  /* submenu_size = 0 if no submenu */
  int submenu_size;
  struct mNode** submenu;

  /* these fields are used only if submenu_size = 0 */
  int activity;
  int param;

  /* these fields are used only if submenu_size > 0 */
  bool show_title;
  int entries_per_screen;
  int first_entry;
};

typedef struct mNode MenuNode;

/* used also by highscore.c */
extern SDL_Rect menu_rect, stop_rect, prev_rect, next_rect;
extern SDL_Surface *stop_button, *prev_arrow, *next_arrow, *prev_gray, *next_gray;

/* global functions */
void LoadMenus(void);
int RunLoginMenu(void);
void RunMainMenu(void);
void UnloadMenus(void);

#endif // MENU_H

