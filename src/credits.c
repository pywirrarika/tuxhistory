/*
  credits.c
 
  For TuxMath
  Contains the text of the credits display, as well
  as the function which displays the credits in the game window.

  by Bill Kendrick
  bill@newbreedsoftware.com
  http://www.newbreedsoftware.com/


  Part of "Tux4Kids" Project
  http://www.tux4kids.org/
  
  August 26, 2001 - March 7, 2005

  Minor changes by
  Jesús Mager
  2017
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "tuxhistory.h"
#include "options.h"
#include "fileops.h"
#include "setup.h"
#include "credits.h"
#include "SDL_extras.h"


const char credit_text[MAX_LINES][MAX_LINEWIDTH] = {
  {"-"N_("TUX HISTORY")},  /* '-' at beginning makes highlighted: */
  {N_("COPYRIGHT 2010-2017")},
  {" "},
  {N_("PART OF THE 'TUX4KIDS' PROJECT")},
  {"http://tux4kids.alioth.debian.org"},
  {" "},
  {"-"N_("DESIGNED BY")},
  {"JESÚS MANUEL MAGER HOIS"},
  {"MENU AND SHAMELESSLY ROBBED"},
  {"FROM TUX MATH."},
  {" "},
  {"-"N_("PROGRAMMERS")},
  {""},
  {" "},
  {"-"N_("ADDITIONAL CODE")},
  {"Aviral Dasgupta"},
  {""},
  {" "},
  {"-"N_("LEAD ARTIST")},
  {""},
  {" "},
  {"-"N_("ADDITIONAL ART")},
  {""},
  {""},
  {" "},
  {"-"N_("SOUND EFFECTS")},
  {"TBA"},
  {" "},
  {"-"N_("MUSIC")},
  {"BEYOND THE HORIZON"},
  {"BY MYSTRA OF STONE ARTS, 1994"},
  {" "},
  {"CCCP MAIN"},
  {"BY GROO OF CNCD, 1994"},
  {" "},
  {"SOFT BRILLIANCE"},
  {"TJOPPBASS, 1994"},
  {" "},
  {"-"N_("PACKAGERS")},
  {""},
  {" "},
  {"-"N_("'TUX' THE PENGUIN CREATED BY")},
  {"LARRY EWING"},
  {" "},
  {"-"N_("TESTERS")},
  {""},
  {" "},
  {" "},
  {"-"N_("USER WEBSITE")},
  {"http://code.kiutz.com/tuxhistory"},
  {" "},
  {"-"N_("TUXHISTORY DEVELOPMENT HOSTED AT GITHUB:")},
  {"http://github.com/pywirrarika/tuxhistory.git"},
  {" "},
  {N_("History is Free Software licensed under the GNU General Public License (GPL). As such, you are specifically granted the rights that are usually denied to users of proprietary software.")},
  {" "},
  {N_("These rights include the freedom to study, copy, modify, and redistribute the program.")},
  {" "},
  {N_("A full copy of the GPL is included with the documentation for this program.")},
  {" "},
  {N_("For more information about Free Software and the GNU GPL, visit:")},
  {"http://www.fsf.org"},
  {" "}
};


/* Some simple pixel-based characters we can blit quickly: */

char chars[39][5][5] = {
  {".###.",
   "#..##",
   "#.#.#",
   "##..#",
   ".###."},

  {"..#..",
   ".##..",
   "..#..",
   "..#..",
   ".###."},

  {".###.",
   "....#",
   "..##.",
   ".#...",
   "#####"},

  {".###.",
   "....#",
   "..##.",
   "....#",
   ".###."},

  {"...#.",
   "..##.",
   ".#.#.",
   "#####",
   "...#."},

  {"#####",
   "#....",
   "####.",
   "....#",
   "####."},

  {".###.",
   "#....",
   "####.",
   "#...#",
   ".###."},

  {"#####",
   "....#",
   "...#.",
   "..#..",
   ".#..."},

  {".###.",
   "#...#",
   ".###.",
   "#...#",
   ".###."},

  {".###.",
   "#...#",
   ".####",
   "....#",
   ".###."},

  {".###.",
   "#...#",
   "#####",
   "#...#",
   "#...#"},

  {"####.",
   "#...#",
   "####.",
   "#...#",
   "####."},
  
  {".###.",
   "#....",
   "#....",
   "#....",
   ".###."},
  
  {"####.",
   "#...#",
   "#...#",
   "#...#",
   "####."},
  
  {"#####",
   "#....",
   "###..",
   "#....",
   "#####"},
  
  {"#####",
   "#....",
   "###..",
   "#....",
   "#...."},
  
  {".###.",
   "#....",
   "#.###",
   "#...#",
   ".###."},
  
  {"#...#",
   "#...#",
   "#####",
   "#...#",
   "#...#"},
  
  {".###.",
   "..#..",
   "..#..",
   "..#..",
   ".###."},
  
  {"....#",
   "....#",
   "....#",
   "#...#",
   ".###."},
  
  {"#..#.",
   "#.#..",
   "##...",
   "#.#..",
   "#..#."},
  
  {"#....",
   "#....",
   "#....",
   "#....",
   "#####"},
  
  {"#...#",
   "##.##",
   "#.#.#",
   "#...#",
   "#...#"},
  
  {"#...#",
   "##..#",
   "#.#.#",
   "#..##",
   "#...#"},
  
  {".###.",
   "#...#",
   "#...#",
   "#...#",
   ".###."},
  
  {"####.",
   "#...#",
   "####.",
   "#....",
   "#...."},
  
  {".###.",
   "#...#",
   "#.#.#",
   "#..#.",
   ".##.#"},
  
  {"####.",
   "#...#",
   "####.",
   "#...#",
   "#...#"},
  
  {".###.",
   "#....",
   ".###.",
   "....#",
   ".###."},
  
  {"#####",
   "..#..",
   "..#..",
   "..#..",
   "..#.."},
  
  {"#...#",
   "#...#",
   "#...#",
   "#...#",
   ".###."},
  
  {"#...#",
   "#...#",
   ".#.#.",
   ".#.#.",
   "..#.."},
  
  {"#...#",
   "#...#",
   "#.#.#",
   "##.##",
   "#...#"},
  
  {"#...#",
   ".#.#.",
   "..#..",
   ".#.#.",
   "#...#"},

  {"#...#",
   ".#.#.",
   "..#..",
   "..#..",
   "..#.."},
  
  {"#####",
   "...#.",
   "..#..",
   ".#...",
   "#####"},
  
  {".....",
   ".....",
   ".....",
   "..#..",
   ".#..."},

  {".....",
   ".....",
   ".....",
   "..#..",
   "..#.."},
  
  {"..#..",
   "..#..",
   ".....",
   ".....",
   "....."}
};


//void draw_text(char * str, SDL_Rect dest);


int line;


int credits(void)
{
  int done, quit, scroll;
  SDL_Rect subscreen, dest;
  
  
  /* Clear window: */
  
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
  
  
  /* Draw title: */
  
  dest.x = (screen->w - images[IMG_TITLE]->w) / 2;
  dest.y = 0;
  dest.w = images[IMG_TITLE]->w;
  dest.h = images[IMG_TITLE]->h;
  
  SDL_BlitSurface(images[IMG_TITLE], NULL, screen, &dest);
  
  
  /* --- MAIN OPTIONS SCREEN LOOP: --- */
  
  done = 0;
  quit = 0;
  scroll = 0;
  line = 0;

  subscreen.x = 0;
  subscreen.y = images[IMG_TITLE]->h;
  subscreen.w = screen->w;
  subscreen.h = screen->h - images[IMG_TITLE]->h;

  /*convert the text array to one wrapped at 40 columns: */
  linewrap_list(credit_text, wrapped_lines, 40, MAX_LINES, MAX_LINEWIDTH);
  quit = scroll_text(wrapped_lines, subscreen, 2);
  
  /* Return the chosen command: */
  
  return quit;
}

int scroll_text(char text[MAX_LINES][MAX_LINEWIDTH], SDL_Rect subscreen, int speed)
{
  int done = 0, quit = 0, scroll = 0, clearing = 0;
  SDL_Event event;
  SDL_Rect src, dest;
  Uint32 last_time = SDL_GetTicks(), now_time;

  line = 0;
    
  do
  {
    /* Handle any incoming events: */
    while (SDL_PollEvent(&event) > 0)
    {
      if (event.type == SDL_QUIT)
      {
        /* Window close event - quit! */
        quit = 1;
        done = 1;
      }
      else if (event.type == SDL_KEYDOWN)
      {
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
          /* Escape key - quit! */
          done = 1;
        }
      }
      else if (event.type == SDL_MOUSEBUTTONDOWN)
      {
        done = 1;
      }
    }

    /* Scroll: */
    src = dest = subscreen;
    src.y += speed; //amount to scroll by

    SDL_BlitSurface(screen, &src, screen, &dest);

    dest.x = subscreen.x;
    dest.y = subscreen.y + subscreen.h - speed;
    dest.w = subscreen.w;
    dest.h = speed;

    SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));
 
    ++scroll;

    if (clearing) //scroll/check, but don't display any more text
    {
      if (scroll > subscreen.h/speed)
            done = 1;
    }
    else
    {
      dest.x = subscreen.x + subscreen.w / 2;
      dest.y = subscreen.y + (subscreen.h - scroll * speed);
      dest.w = 1;
      dest.h = 1;
      draw_text(text[line], dest);  // translation should have already occurred

      if (scroll * speed >= DEFAULT_MENU_FONT_SIZE)
      {
        scroll = 0;
        line++;

        if (text[line][0] == '\0') //end of text 
        {
          clearing = 1; //scroll to blank            
        }            
        else
          DEBUGMSG(debug_titlescreen, "text[line]: %s\n", text[line]);
      }
    }

    SDL_Flip(screen);

    /* Pause (keep frame-rate event) */
    now_time = SDL_GetTicks();
    if (now_time < last_time + (1000 / 20))
    {
      SDL_Delay(last_time + (1000 / 20) - now_time);
    }
    last_time = SDL_GetTicks();

  }
  while (!done);

  return quit;
}
#if 0 //really cool effect, but not translatable. I'll leave it in in case we 
      //decide to use it e.g. only for English
void draw_text(char * str, int offset)
{
  int i, c, x, y, cur_x, start, hilite;
  SDL_Rect dest;
  Uint8 r, g, b;


  if (str[0] == '-')
  {
    start = 1;
    hilite = 1;
  }
  else
  {
    start = 0;
    hilite = 0;
  }
  
  
  cur_x = (screen->w - ((strlen(str) - start) * 18)) / 2;
  
  for (i = start; i < strlen(str); i++)
    { 
      c = -1;
      
      if (str[i] >= '0' && str[i] <= '9')
        c = str[i] - '0';
      else if (str[i] >= 'A' && str[i] <= 'Z')
        c = str[i] - 'A' + 10;
      else if (str[i] == ',')
        c = 36;
      else if (str[i] == '.')
        c = 37;
      else if (str[i] == '\'')
        c = 38;
      
      
      if (c != -1)
        {
          for (y = 0; y < 5; y++)
            {
              if (hilite == 0)
              {
                r = 255 - ((line * y) % 256);
                g = 255 / (y + 2);
                b = (line * line * 2) % 256;
              }
              else
              {
                r = 128;
                g = 192;
                b = 255 - (y * 40);
              }
              
              for (x = 0; x < 5; x++)
                {
                  if (chars[c][y][x] == '#')
                    {
                      dest.x = cur_x + (x * 3);
                      dest.y = ((screen->h - (5 * 3)) + (y * 3) +
                                (18 - offset * 2));
                      dest.w = 3;
                      dest.h = 3;
                      
                      SDL_FillRect(screen, &dest,
                                   SDL_MapRGB(screen->format, r, g, b));
                    }
                }
            }
        }
      
      
      /* Move virtual cursor: */
      
      cur_x = cur_x + 18;
    }
}

#else

//FIXME it's possible that generating the surface every frame taxes 
//slower machines. If so consider returning the surface to be used 
//as long as it's needed.
void draw_text(char* str, SDL_Rect dest)
{
  SDL_Color col;
  SDL_Surface* surf = NULL;
  if (!str || *str == '\0')
    return;

  DEBUGMSG(debug_titlescreen, "Entering draw_text(%s)\n", str);

  if (str[0] == '-') //highlight text
  {
    str++;
    col.r = 128;
    col.g = 192;
    col.b = 255 - (40);
  }
  else //normal color
  {
    col.r = 255 - (line % 256);
    col.g = 255 / 2;
    col.b = (line * line * 2) % 256;  
  }

  /* This func from SDL_extras draws with SDL_Pango if avail, */
  /* with SDL_ttf as fallback:                                */
  surf =  SimpleText(str, DEFAULT_MENU_FONT_SIZE, &col);

  dest.x -= surf->w / 2; //center text
  SDL_BlitSurface(surf, NULL, screen, &dest);
  SDL_FreeSurface(surf);
  DEBUGMSG(debug_titlescreen, "done\n");
}
#endif
