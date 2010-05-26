/*
*  C Implementation: highscore.c
*
* Description: Implementation of high score tables for tuxmath.
*
*
* Author: David Bruce <davidstuartbruce@gmail.com>, (C) 2007
*
* Copyright: See COPYING file that comes with this distribution
* (Briefly, GNU GPL version 2 or greater).
*/
#include <string.h>

#include "tuxhistory.h"
#include "highscore.h"
#include "menu.h"
#include "titlescreen.h"
#include "fileops.h"
#include "setup.h"
#include "options.h"
#include "SDL_extras.h"
#include "convert_utf.h"
#include "transtruct.h"
#include "throttle.h"


typedef struct high_score_entry {
  int score;
  char name[HIGH_SCORE_NAME_LENGTH];
} high_score_entry;


high_score_entry high_scores[NUM_HIGH_SCORE_LEVELS][HIGH_SCORES_SAVED];

/* Local function prototypes: */



/* Display high scores: */
void DisplayHighScores(int level)
{
  int i = 0;
  int finished = 0;
  Uint32 frame = 0;
  Uint32 start = 0;

  int diff_level = level;
  int old_diff_level = -1; //So table gets refreshed first time through
  /* Surfaces, char buffers, and rects for table: */
  SDL_Surface* score_surfs[HIGH_SCORES_SAVED] = {NULL};

  /* 10 spaces should be enough room for place and score on each line: */
  char score_strings[HIGH_SCORES_SAVED][HIGH_SCORE_NAME_LENGTH + 10] = {{'\0'}};

  SDL_Rect score_rects[HIGH_SCORES_SAVED];
  SDL_Rect table_bg;

  const int max_width = 300;
  int score_table_y = 100;

  const int title_font_size = 32;
  const int player_font_size = 14;

  while (!finished)
  {
    start = SDL_GetTicks();

    /* Check for user events: */
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          cleanup();
        }

        case SDL_MOUSEBUTTONDOWN:
        /* "Stop" button - go to main menu: */
        {
          if (inRect(stop_rect, event.button.x, event.button.y ))
          {
            finished = 1;
            playsound(SND_TOCK);
          }

          /* "Left" button - go to previous page: */
          if (inRect(prev_rect, event.button.x, event.button.y))
          {
            if (diff_level > CADET_HIGH_SCORE)
            {
              diff_level--;
              if (Opts_GetGlobalOpt(MENU_SOUND))
              {
                playsound(SND_TOCK);
              }
            }
          }

          /* "Right" button - go to next page: */
          if (inRect(next_rect, event.button.x, event.button.y ))
          {
            if (diff_level < (NUM_HIGH_SCORE_LEVELS-1))
            {
              diff_level++;
              if (Opts_GetGlobalOpt(MENU_SOUND))
              {
                playsound(SND_TOCK);
              }
            }
          }
          break;
        }


        case SDL_KEYDOWN:
        {
          finished = 1;
          playsound(SND_TOCK);
        }
      }
    }


    /* If needed, redraw: */
    if (diff_level != old_diff_level)
    {
      DrawTitleScreen();
      /* Draw controls: */
      if (stop_button)
        SDL_BlitSurface(stop_button, NULL, screen, &stop_rect);
      /* Draw regular or grayed-out left arrow: */
      if (diff_level == CADET_HIGH_SCORE)
      {
        if (prev_gray)
          SDL_BlitSurface(prev_gray, NULL, screen, &prev_rect);
      }
      else
      {
        if (prev_arrow)
          SDL_BlitSurface(prev_arrow, NULL, screen, &prev_rect);
      }
      /* Draw regular or grayed-out right arrow: */
      if (diff_level == NUM_HIGH_SCORE_LEVELS - 1)
      {
        if (next_gray)
          SDL_BlitSurface(next_gray, NULL, screen, &next_rect);
      }
      else
      {
        if (next_arrow)
          SDL_BlitSurface(next_arrow, NULL, screen, &next_rect);
      }

      /* Draw background shading for table: */
      table_bg.x = (screen->w)/2 - (max_width + 20)/2 + 50; //don't draw over Tux
      table_bg.y = 5;
      table_bg.w = max_width + 20;
      table_bg.h = screen->h - 10 - images[IMG_RIGHT]->h;
      DrawButton(&table_bg, 25, SEL_RGBA);

      /* Draw difficulty level heading: */
      {
        SDL_Surface* srfc = NULL;
        SDL_Rect text_rect, button_rect;

        srfc = BlackOutline(_("Hall Of Fame"), title_font_size, &yellow);
        if (srfc)
        {
          button_rect.x = text_rect.x = (screen->w)/2 - (srfc->w)/2 + 50;
          button_rect.y = text_rect.y = 10;
          button_rect.w = text_rect.w = srfc->w;
          button_rect.h = text_rect.h = srfc->h;
          /* add margin to button and draw: */
          button_rect.x -= 10;
          button_rect.w += 20;
          DrawButton(&button_rect, 15, 0, 0, 32, 192);
          /* Now blit text and free surface: */
          SDL_BlitSurface(srfc, NULL, screen, &text_rect);
          SDL_FreeSurface(srfc);
          srfc = NULL;
        }

        switch (diff_level)
        {
          case CADET_HIGH_SCORE:
            srfc = BlackOutline(_("Space Cadet"), title_font_size, &white);
            break;
          case SCOUT_HIGH_SCORE:
            srfc = BlackOutline(_("Scout"), title_font_size, &white);
            break;
          case RANGER_HIGH_SCORE:
            srfc = BlackOutline(_("Ranger"), title_font_size, &white);
            break;
          case ACE_HIGH_SCORE:
            srfc = BlackOutline(_("Ace"), title_font_size, &white);
            break;
          case COMMANDO_HIGH_SCORE:
            srfc = BlackOutline(_("Commando"), title_font_size, &white);
            break;
          case FACTORS_HIGH_SCORE:
            srfc = BlackOutline(_("Factors"), title_font_size, &white);
            break;
          case FRACTIONS_HIGH_SCORE:
            srfc = BlackOutline(_("Fractions"), title_font_size, &white);
            break;
          default:
            srfc = BlackOutline(_("Space Cadet"), title_font_size, &white);
        }

        if (srfc)
        {
          text_rect.x = (screen->w)/2 - (srfc->w)/2 + 50; 
          text_rect.y += text_rect.h; /* go to bottom of first line */
          text_rect.w = srfc->w;
          text_rect.h = srfc->h;
          SDL_BlitSurface(srfc, NULL, screen, &text_rect);
          SDL_FreeSurface(srfc);
          srfc = NULL;
          /* note where score table will start: */
          score_table_y = text_rect.y + text_rect.h;
        }
      }


      /* Generate and draw desired table: */

      for (i = 0; i < HIGH_SCORES_SAVED; i++)
      {
        /* Get data for entries: */
        sprintf(score_strings[i],
                "%d.    %d     %s",
                i + 1,                  /* Add one to get common-language place number */
                HS_Score(diff_level, i),
                HS_Name(diff_level, i));

        /* Clear out old surfaces and update: */
        if (score_surfs[i])               /* this should not happen! */
          SDL_FreeSurface(score_surfs[i]);
        if (HS_Score(diff_level, i) == Opts_LastScore() && frame % 5 < 2)
          score_surfs[i] = BlackOutline(N_(score_strings[i]), player_font_size, &yellow);
        else
          score_surfs[i] = BlackOutline(N_(score_strings[i]), player_font_size, &white);

        /* Get out if BlackOutline() fails: */
        if (!score_surfs[i])
          continue;
        /* Set up entries in vertical column: */
        if (0 == i)
          score_rects[i].y = score_table_y;
        else
          score_rects[i].y = score_rects[i - 1].y + score_rects[i - 1].h;

        score_rects[i].x = (screen->w)/2 - max_width/2 + 50;
        score_rects[i].h = score_surfs[i]->h;
        score_rects[i].w = max_width;

        SDL_BlitSurface(score_surfs[i], NULL, screen, &score_rects[i]);
        SDL_FreeSurface(score_surfs[i]);
        score_surfs[i] = NULL;
      }
      /* Update screen: */
      SDL_UpdateRect(screen, 0, 0, 0, 0);

      old_diff_level = diff_level;
    }

    HandleTitleScreenAnimations();

    /* Wait so we keep frame rate constant: */
    while ((SDL_GetTicks() - start) < 33)
    {
      SDL_Delay(20);
    }
    frame++;
  }  // End of while (!finished) loop
}


/* Display screen to allow player to enter name for high score table:     */
/* The pl_name argument *must* point to a validly allocated string array  */
/* at least three times HIGH_SCORE_NAME_LENGTH because UTF-8 is a         */
/* multibyte encoding.                                                    */
void HighScoreNameEntry(char* pl_name)
{
  NameEntry(pl_name, _("You Are In The Hall of Fame!"), _("Enter Your Name:"));
}

void NameEntry(char* pl_name, const char* heading, const char* sub)
{
  char UTF8_buf[HIGH_SCORE_NAME_LENGTH * 3] = {'\0'};

  SDL_Rect loc;
  SDL_Rect redraw_rect;

  int redraw = 0;
  int first_draw = 1;
  int finished = 0;
  Uint32 frame = 0;
  Uint32 start = 0;
  wchar_t wchar_buf[HIGH_SCORE_NAME_LENGTH + 1] = {'\0'};
  const int NAME_FONT_SIZE = 32;
  const int BG_Y = 100;
  const int BG_WIDTH = 400;
  const int BG_HEIGHT = 200;

  if (!pl_name)
    return;

  /* We need to get Unicode vals from SDL keysyms */
  SDL_EnableUNICODE(SDL_ENABLE);

  DEBUGMSG(debug_highscore, "Enter HighScoreNameEntry()\n" );

  DrawTitleScreen();

  /* Red "Stop" circle in upper right corner to go back to main menu: */
  if (stop_button)
  {
    SDL_BlitSurface(stop_button, NULL, screen, &stop_rect);
  }

  /* Draw translucent background for text: */
  {
    SDL_Rect bg_rect;
    bg_rect.x = (screen->w)/2 - BG_WIDTH/2;
    bg_rect.y = BG_Y;
    bg_rect.w = BG_WIDTH;
    bg_rect.h = BG_HEIGHT;
    DrawButton(&bg_rect, 15, REG_RGBA);

    bg_rect.x += 10;
    bg_rect.y += 10;
    bg_rect.w -= 20;
    bg_rect.h = 60;
    DrawButton(&bg_rect, 10, SEL_RGBA);
  }

  /* Draw heading: */
  {
    SDL_Surface* s = BlackOutline(_(heading),
                                  DEFAULT_MENU_FONT_SIZE, &white);
    if (s)
    {
      loc.x = (screen->w/2) - (s->w/2);
      loc.y = 110;
      SDL_BlitSurface(s, NULL, screen, &loc);
      SDL_FreeSurface(s);
    }

    s = BlackOutline(_(sub),
                     DEFAULT_MENU_FONT_SIZE, &white);
    if (s)
    {
      loc.x = (screen->w/2) - (s->w/2);
      loc.y = 140;
      SDL_BlitSurface(s, NULL, screen, &loc);
      SDL_FreeSurface(s);
    }
  }

  /* and update: */
  SDL_UpdateRect(screen, 0, 0, 0, 0);


  while (!finished)
  {
    start = SDL_GetTicks();

    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          cleanup();
        }

        case SDL_MOUSEBUTTONDOWN:
        /* "Stop" button - go to main menu: */
        {
          if (inRect(stop_rect, event.button.x, event.button.y ))
          {
            finished = 1;
            playsound(SND_TOCK);
            break;
          }
        }
        case SDL_KEYDOWN:
        {
          DEBUGMSG(debug_highscore, "Before keypress, string is %S\tlength = %d\n",
                   wchar_buf, (int)wcslen(wchar_buf));
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
            {
              finished = 1;
              playsound(SND_TOCK);
              break;
            }
            case SDLK_BACKSPACE:
            {
              if (wcslen(wchar_buf) > 0)
                wchar_buf[(int)wcslen(wchar_buf) - 1] = '\0';
              redraw = 1;
              break;
            }

            /* For any other keys, if the key has a Unicode value, */
            /* we add it to our string:                            */
            default:
            {
              if ((event.key.keysym.unicode > 0)
              && (wcslen(wchar_buf) < HIGH_SCORE_NAME_LENGTH)) 
              {
                wchar_buf[(int)wcslen(wchar_buf)] = event.key.keysym.unicode;
                redraw = 1;
              }
            }
          }  /* end  'switch (event.key.keysym.sym)'  */

          DEBUGMSG(debug_highscore, "After keypress, string is %S\tlength = %d\n",
                   wchar_buf, (int)wcslen(wchar_buf));
            /* Now draw name, if needed: */
          if (redraw)
          {
            SDL_Surface* s = NULL;
            redraw = 0;

            /* Convert text to UTF-8 so BlackOutline() can handle it: */
   //         wcstombs((char*) UTF8_buf, wchar_buf, HIGH_SCORE_NAME_LENGTH * 3);
            ConvertToUTF8(wchar_buf, UTF8_buf, HIGH_SCORE_NAME_LENGTH * 3);
            /* Redraw background and shading in area where we drew text last time: */ 
            if (!first_draw)
            {
              SDL_BlitSurface(current_bkg(), &redraw_rect, screen, &redraw_rect);
              DrawButton(&redraw_rect, 0, REG_RGBA);
              SDL_UpdateRect(screen,
                             redraw_rect.x,
                             redraw_rect.y,
                             redraw_rect.w,
                             redraw_rect.h);
            }

            s = BlackOutline(UTF8_buf, NAME_FONT_SIZE, &yellow);
            if (s)
            {
              /* set up loc and blit: */
              loc.x = (screen->w/2) - (s->w/2);
              loc.y = 200;
              SDL_BlitSurface(s, NULL, screen, &loc);

              /* Remember where we drew so we can update background next time through:  */
              /* (for some reason we need to update a wider area to get clean image)    */
              redraw_rect.x = loc.x - 20;
              redraw_rect.y = loc.y - 10;
              redraw_rect.h = s->h + 20;
              redraw_rect.w = s->w + 40;
              first_draw = 0;

              SDL_UpdateRect(screen,
                             redraw_rect.x,
                             redraw_rect.y,
                             redraw_rect.w,
                             redraw_rect.h);
              SDL_FreeSurface(s);
              s = NULL;
            }
          }
        }
      }
    }

    HandleTitleScreenAnimations();

    /* Wait so we keep frame rate constant: */
    while ((SDL_GetTicks() - start) < 33)
    {
      SDL_Delay(20);
    }
    frame++;
  }  // End of while (!finished) loop

  /* Turn off SDL Unicode lookup (because has some overhead): */
  SDL_EnableUNICODE(SDL_DISABLE);

  /* Now copy name into location pointed to by arg: */ 
  strncpy(pl_name, UTF8_buf, HIGH_SCORE_NAME_LENGTH * 3);
}




/* Zero-out the array before use: */
void initialize_scores(void)
{
  int i, j;
  for (i = 0; i < NUM_HIGH_SCORE_LEVELS; i++)
  {
    for (j = 0; j < HIGH_SCORES_SAVED; j++)
    {
      high_scores[i][j].score = 0;
      strcpy(high_scores[i][j].name, "");
    }
  }
}

/* Test to see where a new score ranks on the list.      */
/* The return value is the index value - add one to get  */
/* the common-language place on the list.                */
int check_score_place(int diff_level, int new_score)
{
  int i = 0;

  /* Make sure diff_level is valid: */
  if (diff_level < 0
   || diff_level >= NUM_HIGH_SCORE_LEVELS)
  {
    fprintf(stderr, "In insert_score(), diff_level invalid!\n");
    return 0;
  }

  /* Find correct place in list: */
  for (i = 0; i < HIGH_SCORES_SAVED; i++)
  {
    if (new_score > high_scores[diff_level][i].score)
      break;
  }

  return i;  /* So if we return HIGH_SCORES_SAVED, the score did not */
             /* make the list.                                       */
}

/* Put a new high score entry into the table for the corresponding */
/* difficulty level - returns 1 if successful.                     */ 
int insert_score(char* playername, int diff_level, int new_score)
{
  int i = 0;
  int insert_place;

  insert_place = check_score_place(diff_level, new_score);

  if (HIGH_SCORES_SAVED == insert_place) /* Score didn't make the top 10 */
  {
    return 0;
  }

  /* Move lower entries down: */
  for (i = HIGH_SCORES_SAVED - 1; i > insert_place; i--)
  {
    high_scores[diff_level][i].score =
            high_scores[diff_level][i - 1].score;
    strncpy(high_scores[diff_level][i].name,
            high_scores[diff_level][i - 1].name,
            HIGH_SCORE_NAME_LENGTH);
  }

  /* Now put in new entry: */
  high_scores[diff_level][insert_place].score = new_score;
  strncpy(high_scores[diff_level][insert_place].name,
          playername,
          HIGH_SCORE_NAME_LENGTH);
  return 1;
}


void print_high_scores(FILE* fp)
{
  int i, j;

  fprintf(fp, "\nHigh Scores:\n");

  for (i = 0; i < NUM_HIGH_SCORE_LEVELS; i++)
  {
    switch(i)
    {    
      case CADET_HIGH_SCORE:
      {
        fprintf(fp, "\nSpace Cadet:\n");
        break;
      }
      case SCOUT_HIGH_SCORE:
      {
        fprintf(fp, "\nScout:\n");
        break;
      }
      case RANGER_HIGH_SCORE:
      {
        fprintf(fp, "\nRanger:\n");
        break;
      }
      case ACE_HIGH_SCORE:
      {
        fprintf(fp, "\nAce:\n");
        break;
      }
      case COMMANDO_HIGH_SCORE:
      {
        fprintf(fp, "\nCommando:\n");
        break;
      }
      case FACTORS_HIGH_SCORE:
      {
        fprintf(fp, "\nFactors:\n");
        break;
      }
      case FRACTIONS_HIGH_SCORE:
      {
        fprintf(fp, "\nFractions:\n");
        break;
      }
    }

    for (j = 0; j < HIGH_SCORES_SAVED; j++)
    {
      fprintf(fp, "%d.\t%s\t%d\n",
              j + 1,                  //Convert to common-language ordinals
              high_scores[i][j].name,
              high_scores[i][j].score);
    }
  }
}


int read_high_scores_fp(FILE* fp)
{
  char buf[PATH_MAX];
  char* token;
  const char delimiters[] = "\t";

  char* name_read;
  int score_read;
  int diff_level;

  DEBUGMSG(debug_highscore, "Entering read_high_scores_fp()\n");

  /* get out if file pointer invalid: */
  if(!fp)
  {
    fprintf(stderr, "In read_high_scores_fp(), file pointer invalid!\n");
    return 0;
  }

  /* make sure we start at beginning: */
  rewind(fp);

  /* read in a line at a time: */
  while (fgets (buf, PATH_MAX, fp))
  { 
    /* Ignore comment lines: */
    if ((buf[0] == ';') || (buf[0] == '#'))
    {
      continue;
    }
    /* Split up line with strtok()to get needed values,  */ 
    /* then call insert_score() for each line.           */
    token = strtok(buf, delimiters);
    if (!token)
      continue;
    diff_level = atoi(token);
    if (diff_level >= NUM_HIGH_SCORE_LEVELS)
      continue;

    token = strtok(NULL, delimiters);
    if (!token)
      continue; 
    score_read = atoi(token);
    /* Note that name can contain spaces - \t is only delimiter: */
    name_read = strtok(NULL, delimiters);
    /* Now insert entry: */
    insert_score(name_read, diff_level, score_read); 
  }
  return 1;
}


/* Return the score associated with a table entry:    */
/* Note: the place is given as the array index, i.e.  */
/* 0 for the top of the list.                         */
int HS_Score(int diff_level, int place)
{
  /* Make sure diff_level is valid: */
  if (diff_level < 0
   || diff_level >= NUM_HIGH_SCORE_LEVELS)
  {
    fprintf(stderr, "In HS_Score(), diff_level = %d, invalid!\n", diff_level);
    return -1;
  }

  /* Make sure place is valid: */
  if (place < 0
   || place >= HIGH_SCORES_SAVED)
  {
    fprintf(stderr, "In HS_Score(), place invalid!\n");
    return -1;
  }

  return high_scores[diff_level][place].score;
}


/* Return (pointer to) the name associated with a table entry:  */
char* HS_Name(int diff_level, int place)
{
  /* Make sure diff_level is valid: */
  if (diff_level < 0
   || diff_level >= NUM_HIGH_SCORE_LEVELS)
  {
    fprintf(stderr, "In HS_Score(), diff_level invalid!\n");
    return NULL;
  }

  /* Make sure place is valid: */
  if (place < 0
   || place >= HIGH_SCORES_SAVED)
  {
    fprintf(stderr, "In HS_Score(), place invalid!\n");
    return NULL;
  }

  return high_scores[diff_level][place].name;
}


int Ready(const char* heading)
{
  SDL_Rect loc;
  SDL_Rect okRect;
  int finished = 0;
  Uint32 frame = 0;
  Uint32 timer = 0;
  const int BG_Y = 100;
  const int BG_WIDTH = 400;
  const int BG_HEIGHT = 200;

  DEBUGMSG(debug_highscore, "Enter Ready()\n" );

  DrawTitleScreen();

  /* Draw translucent background for text: */
  {
    SDL_Rect bg_rect;
    bg_rect.x = (screen->w)/2 - BG_WIDTH/2;
    bg_rect.y = BG_Y;
    bg_rect.w = BG_WIDTH;
    bg_rect.h = BG_HEIGHT;
    DrawButton(&bg_rect, 15, REG_RGBA);

    bg_rect.x += 10;
    bg_rect.y += 10;
    bg_rect.w -= 20;
    bg_rect.h = 60;
    DrawButton(&bg_rect, 10, SEL_RGBA);
  }

  /* Draw heading: */
  {
    SDL_Surface* s = BlackOutline(_(heading),
                                  DEFAULT_MENU_FONT_SIZE, &white);
    if (s)
    {
      loc.x = (screen->w/2) - (s->w/2);
      loc.y = 110;
      SDL_BlitSurface(s, NULL, screen, &loc);
      SDL_FreeSurface(s);
    }
  }

  /* Red "Stop" circle in upper right corner to go back to main menu: */
  if (stop_button)
  {
    SDL_BlitSurface(stop_button, NULL, screen, &stop_rect);
  }

  /* "Next_arrow" to indicate ready to proceed: */
  if (next_arrow)
  {
    okRect.x = (screen->w)/2;
    okRect.y = 240;
    SDL_BlitSurface(next_arrow, NULL, screen, &okRect);
  }

  /* and update: */
  SDL_UpdateRect(screen, 0, 0, 0, 0);


  while (!finished)
  {
    /* Handle user events: */
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          cleanup();
        }

        case SDL_MOUSEBUTTONDOWN:
        /* "Stop" button - go to main menu: */
        {
          if (inRect(stop_rect, event.button.x, event.button.y ))
          {
            finished = -1;
            playsound(SND_TOCK);
            break;
          } 
          else if (inRect(okRect, event.button.x, event.button.y ))
          {
            finished = 1;
            playsound(SND_TOCK);
            break;
          }

        }
        case SDL_KEYDOWN:
        {
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
            case SDLK_BACKSPACE:
            {
              finished = -2;
              playsound(SND_TOCK);
              break;
            }
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
            case SDLK_SPACE:
            {
              finished = 1;
              playsound(SND_TOCK);
              break;
            }
            default:
            {
              //Do nothing - event. add support for toggle fullscreen, etc.
            }
          } 
        }
      }
    }

    HandleTitleScreenAnimations();
    Throttle(20, &timer);
    frame++;
  }  // End of while (!finished) loop

  DEBUGMSG(debug_highscore, "Leave Ready()\n" );

  /* 1 means we start game, -1 means we go back to menu */
  return finished;
}


int Standby(const char* heading, const char* sub)
{
#ifndef HAVE_LIBSDL_NET
  return 0;
#else
  SDL_Rect loc;
  int finished = 0;
  Uint32 frame = 0;
  Uint32 timer = 0;
  const int BG_Y = 100;
  const int BG_WIDTH = 400;
  const int BG_HEIGHT = 200;

  char buf[NET_BUF_LEN];

  DEBUGMSG(debug_highscore, "Enter Standby()\n" );

  DrawTitleScreen();

  /* Draw translucent background for text: */
  {
    SDL_Rect bg_rect;
    bg_rect.x = (screen->w)/2 - BG_WIDTH/2;
    bg_rect.y = BG_Y;
    bg_rect.w = BG_WIDTH;
    bg_rect.h = BG_HEIGHT;
    DrawButton(&bg_rect, 15, REG_RGBA);

    bg_rect.x += 10;
    bg_rect.y += 10;
    bg_rect.w -= 20;
    bg_rect.h = 60;
    DrawButton(&bg_rect, 10, SEL_RGBA);
  }

  /* Draw heading: */
  {
    SDL_Surface* s = BlackOutline(_(heading),
                                  DEFAULT_MENU_FONT_SIZE, &white);
    if (s)
    {
      loc.x = (screen->w/2) - (s->w/2);
      loc.y = 110;
      SDL_BlitSurface(s, NULL, screen, &loc);
      SDL_FreeSurface(s);
    }

    s = BlackOutline(_(sub),
                     DEFAULT_MENU_FONT_SIZE, &white);
    if (s)
    {
      loc.x = (screen->w/2) - (s->w/2);
      loc.y = 140;
      SDL_BlitSurface(s, NULL, screen, &loc);
      SDL_FreeSurface(s);
    }
  }

  /* Red "Stop" circle in upper right corner to go back to main menu: */
  if (stop_button)
  {
    SDL_BlitSurface(stop_button, NULL, screen, &stop_rect);
  }

  /* and update: */
  SDL_UpdateRect(screen, 0, 0, 0, 0);


  while (!finished)
  {
    /* Handle user events: */
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          cleanup();
        }

        case SDL_MOUSEBUTTONDOWN:
        /* "Stop" button - go to main menu: */
        {
          if (inRect(stop_rect, event.button.x, event.button.y ))
          {
            finished = 1;
            playsound(SND_TOCK);
            break;
          }
        }
        case SDL_KEYDOWN:
        {
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
            case SDLK_BACKSPACE:
            {
              finished = -2;
              playsound(SND_TOCK);
              break;
            }

            default:
            {
              //Do nothing - event. add support for toggle fullscreen, etc.
            }
          } 
        }
      }
    }

    /* Handle server messages: */
    /* TODO: If this code only is relevant for networking, delete
    while(!check_messages(buf))
    {
      if(strncmp(buf,"GO_TO_GAME", strlen("GO_TO_GAME")) == 0)
      {
        finished = 1;
        playsound(SND_TOCK);
        break;
      }
      else if(strncmp(buf, "GAME_IN_PROGRESS", strlen("GAME_IN_PROGRESS")) == 0)
      {
        finished = -1;
        playsound(SND_TOCK);
        break;
      }
      else
      {
        DEBUGMSG(debug_highscore, "Unrecognized message from server: %s\n", buf);
        continue;
      }
    }*/

    HandleTitleScreenAnimations();
    Throttle(20, &timer);
    frame++;
  }  // End of while (!finished) loop

  DEBUGMSG(debug_highscore, "Leave Standby()\n" );

  /* 1 means we start game, -1 means we go back to menu */
  return finished;
#endif
}



