/*
  setup.c

  For TuxHistory (stolen from tuxmath)
  Contains some globals (screen surface, images, some option flags, etc.)
  as well as the function to load data files (images, sounds, music)
  and display a "Loading..." screen.

  by Bill Kendrick
  bill@newbreedsoftware.com
  http://www.newbreedsoftware.com/

  Modified by David Bruce
  davidstuartbruce@gmail.com
  and Jesus Mager
  fongog@gmail.com

  Part of "Tux4Kids" Project
  http://www.tux4kids.com/
  Git repository:
  git://git.debian.org/git/tux4kids/tuxhistory.git

  August 26, 2001 - June, 2010.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FIXME maybe unistd.h not needed, even less sure about portability */
//#include <unistd.h>

#include "SDL.h"

#ifndef NOSOUND
#include "SDL_mixer.h"
#endif

#include "SDL_image.h"

#include "options.h"
#include "tuxhistory.h"
#include "globals.h"
#include "setup.h"
#include "fileops.h"
#include "loaders.h"
#include "map.h"
#include "game.h"
#include "menu.h"
#include "titlescreen.h"
#include "highscore.h"
#include "SDL_extras.h"


//#ifdef LINEBREAK
#include "linewrap.h"
//#endif

/* Global data used in setup.c:              */
/* (These are now 'extern'd in "tuxhsitory.h") */

/* window size */
int win_res_x = 640;
int win_res_y = 480;

/* full screen size (set in initialize_SDL() ) */
int fs_res_x = 0;
int fs_res_y = 0;

th_map **map = NULL; //The map array.
th_obj* object = NULL;


SDL_Surface* screen;
SDL_Surface* images[NUM_IMAGES];
sprite* sprites[NUM_SPRITES];
SDL_Surface* terrain[NUM_TERRAINS];
SDL_Surface* objects[NUM_OBJECTS];
/* Need special handling to generate flipped versions of images. This
   is a slightly ugly hack arising from the use of the enum trick for
   NUM_IMAGES. */
#define NUM_FLIPPED_IMAGES 6
SDL_Surface* flipped_images[NUM_FLIPPED_IMAGES];
SDL_Surface* blended_igloos[NUM_BLENDED_IGLOOS];

#ifndef NOSOUND
Mix_Chunk* sounds[NUM_SOUNDS];
Mix_Music* musics[NUM_MUSICS];
#endif

/* Local function prototypes: */
void initialize_options(void);
void handle_command_args(int argc, char* argv[]);
void initialize_SDL(void);
void load_data_files(void);
void data_memory_alloc(void);

//int initialize_game_options(void);
void seticon(void);
void usage(int err, char * cmd);

void cleanup_memory(void);

/* --- Set-up function - now in four easier-to-digest courses! --- */
/* --- Er - make that six courses! --- */
/* --- Six is right out. Seven is much better. --- */
void setup(int argc, char * argv[])
{
  /* initialize settings and read in config files: */
  /* Note this now only does the global settings   */
  initialize_options();
  /* Command-line code now in own function: */
  handle_command_args(argc, argv);
  /* SDL setup in own function:*/
  initialize_SDL();
  
  /*Alloc global vars fot tuxhistory*/
  data_memory_alloc();

  /* Read image and sound files: */
  load_data_files();
  /* Note that the per-user options will be set after the call to
     titlescreen, to allow for user-login to occur. 
     
     FIXME this means that command-line args will be overridden!
     Is this desirable? */
}




/* Set up mathcards with default values for math question options, */
/* set up game_options with defaults for general game options,     */
/* then read in global config file                                 */
void initialize_options(void)
{

  /* initialize game_options struct with defaults DSB */
  if (!Opts_Initialize())
  {
    fprintf(stderr, "\nUnable to initialize game_options\n");
    cleanup_on_error();
    exit(1);
  }

  /* Now that MathCards and game_options initialized using  */
  /* hard-coded defaults, read options from disk and mofify */
  /* as needed. First read in installation-wide settings:   */
  if (!read_global_config_file())
  {
    fprintf(stderr, "\nCould not find global config file.\n");
    /* can still proceed using hard-coded defaults.         */
  }
}

/* Read in the user-specific options (if desired)              */
/* This has been split from the above to allow it to be called */
/* from titlescreen, to allow for user-login to occur.         */
void initialize_options_user(void)
{
  /* Read in user-specific settings, if desired.  By    */
  /* default, this restores settings from the player's last */
  /* game:                                                  */
  if (Opts_GetGlobalOpt(PER_USER_CONFIG))
  {
    if (!read_user_config_file())
    {
      fprintf(stderr, "\nCould not find user's config file.\n");
      /* can still proceed using hard-coded defaults.         */
    }

    /* If game being run for first time, try to write file: */
    if (!write_user_config_file())
    {
      fprintf(stderr, "\nUnable to write user's config file.\n");
    }
  }

  /* Read the lessons directory to determine which lesson   */
  /* files are available.                                   */
  if (!parse_lesson_file_directory())
    fprintf(stderr,"\nCould not parse the lesson file directory.\n");

  /* Now set up high score tables: */
  initialize_scores();
  if (!read_high_scores())
  {
    fprintf(stderr, "\nCould not find high score table.\n");
    /* (can still proceed).         */
  }

  DEBUGCODE(debug_setup)
    print_high_scores(stdout);
}




/* Handle any arguments passed from command line */
void handle_command_args(int argc, char* argv[])
{
  DIR *dirp;
  int i;

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      /* Display help message: */

      printf("\nTux, of Math Command\n\n"
        "Use the number keys on the keyboard to answer math equations.\n"
        "If you don't answer a comet's math equation before it hits\n"
        "one of your cities, the city's shields will be destroyed.\n"
        "If that city is hit by another comet, it is destroyed completely.\n"
        "When you lose all of your cities, the game ends.\n\n");

      printf("Note: all settings are now stored in a config file named 'options' in\n"
             "a hidden directory named './tuxmath' within the user's home directory.\n"
             "The file consists of simple name/value pairs. It is much easier\n"
             "to edit this file to set game parameters than to use the command-line\n"
             "arguments listed below. Also, many options are not selectable from the\n"
             "command line. The config file contains extensive comments detailing how\n"
             "to configure the behavior of Tuxmath.\n\n");

      printf("Run the game with:\n"
        "--homedir dirname      - seek for user home director(ies) in the specified\n"
        "                         location, rather than the user's actual home\n"
        "                         directory.  You can set up a user directory tree in\n"
        "                         this location (see README).  This option is\n"
        "                         especially useful for schools where all students log\n"
        "                         in with a single user name.\n"
        "--optionfile filename  - read config settings from named file. The locations\n"
        "                         searched for a file with a matching name are the\n"
        "                         current working directory, the absolute path of the\n"
        "                         filename, tuxmath's missions directory, the user's\n"
        "                         tuxmath directory, and the user's home.\n"
        "--nosound        - to disable sound/music\n"
        "--nobackground   - to disable background photos (for slower systems)\n"
        "--fullscreen     - to run in fullscreen, if possible (vs. windowed)\n"
        "--windowed       - to run in a window rather than fullscreen\n"
        "--keypad         - to enable the on-sceen numeric keypad\n"
        "--demo           - to run the program as a cycling demonstration\n"
        "--speed S        - set initial speed of the game\n"
        "                   (S may be fractional, default is 1.0)\n"
        "--allownegatives - to allow answers to be less than zero\n"
        );

      printf("\n");

      cleanup_on_error();
      exit(0);
    }
    else if (strcmp(argv[i], "--copyright") == 0 ||
             strcmp(argv[i], "-c") == 0)
    {
      printf(
        "\n\"Tux, of Math Command\" version " VERSION ", Copyright (C) 2001-2009,\n"
        "Bill Kendrick, David Bruce, Tim Holy, and the Tux4Kids Project.\n"
        "This program is free software; you can redistribute it and/or\n"
        "modify it under the terms of the GNU General Public License\n"
        "as published by the Free Software Foundation.  See COPYING.txt\n"
        "\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
        "\n");

      cleanup_on_error();
      exit(0);
    }
    else if (strcmp(argv[i], "--usage") == 0 ||
             strcmp(argv[i], "-u") == 0)
    {
      /* Display (happy) usage: */

      usage(0, argv[0]);
    }
    else if (0 == strcmp(argv[i], "--homedir"))
    {
      // Parse the user choice of a non-default home directory
      if (i >= argc -1)
      {
        fprintf(stderr, "%s option requires an argument (dirname)\n", argv[i]);
        usage(1, argv[0]);
      }
      else // see whether the specified name is a directory
      {
        if ((dirp = opendir(argv[i+1])) == NULL)
          fprintf(stderr,"homedir: %s is not a directory, or it could not be read\n", argv[i+1]);
        else {
          set_user_data_dir(argv[i+1]);  // copy the homedir setting
          closedir(dirp);
        }
        i++;   // to pass over the next argument, so remaining options parsed
      }
    }
    else if (0 == strcmp(argv[i], "--optionfile"))
    {
      if (i >= argc - 1)
      {
        fprintf(stderr, "%s option requires an argument (filename)\n", argv[i]);
        usage(1, argv[0]);
      }
      else /* try to read file named in following arg: */
      {
        if (!read_named_config_file(argv[i + 1]))
        {
          fprintf(stderr, "Could not read config file: %s\n", argv[i + 1]);
        }
      }
      i++; /* so program doesn't barf on next arg (the filename) */
    }
    else if (strcmp(argv[i], "--fullscreen") == 0 ||
             strcmp(argv[i], "-f") == 0)
    {
        printf("Fullscreen is not now supported...");
        Opts_SetGlobalOpt(FULLSCREEN, 0);
    }
    else if (strcmp(argv[i], "--windowed") == 0 ||
             strcmp(argv[i], "-w") == 0)
    {
      Opts_SetGlobalOpt(FULLSCREEN, 0);
    }
    else if (strcmp(argv[i], "--nosound") == 0 ||
             strcmp(argv[i], "-s") == 0 ||
             strcmp(argv[i], "--quiet") == 0 ||
             strcmp(argv[i], "-q") == 0)
    {
      Opts_SetGlobalOpt(USE_SOUND, -1);  // prevent options files from overwriting
    }
    else if (strcmp(argv[i], "--version") == 0 ||
             strcmp(argv[i], "-v") == 0)
    {
      printf("Tux History (\"tuxhistory\")\n"
             "Version " VERSION "\n");
      cleanup_on_error();
      exit(0);
    }
    else if (strcmp(argv[i], "--nobackground") == 0 ||
             strcmp(argv[i], "-b") == 0)
    {
      Opts_SetUseBkgd(0);
    }
    else if (strcmp(argv[i], "--demo") == 0 ||
             strcmp(argv[i], "-d") == 0)
    {
      Opts_SetDemoMode(1);
    }
    else if (strcmp(argv[i], "--keypad") == 0 ||
             strcmp(argv[i], "-k") == 0)
    {
      Opts_SetGlobalOpt(USE_KEYPAD, 1);
    }
    else if (strcmp(argv[i], "--speed") == 0 ||
             strcmp(argv[i], "-s") == 0)
    {
      if (i >= argc - 1)
      {
        fprintf(stderr, "%s option requires an argument\n", argv[i]);
        usage(1, argv[0]);
      }

      Opts_SetSpeed(strtod(argv[i + 1], (char **) NULL));
      i++;
    }
    else if (strcmp(argv[i], "--debug-all") == 0)
    {
      debug_status |= debug_all;
    }
    else if (strcmp(argv[i], "--debug-setup") == 0)
    {
      debug_status |= debug_setup;
    }
    else if (strcmp(argv[i], "--debug-fileops") == 0)
    {
      debug_status |= debug_fileops;
    }
    else if (strcmp(argv[i], "--debug-loaders") == 0)
    {
      debug_status |= debug_loaders;
    }
    else if (strcmp(argv[i], "--debug-titlescreen") == 0)
    {
      debug_status |= debug_titlescreen;
    }
    else if (strcmp(argv[i], "--debug-menu") == 0)
    {
      debug_status |= debug_menu;
    }
    else if (strcmp(argv[i], "--debug-menu-parser") == 0)
    {
      debug_status |= debug_menu_parser;
    }
    else if (strcmp(argv[i], "--debug-game") == 0)
    {
      debug_status |= debug_game;
    }
    else if (strcmp(argv[i], "--debug-factoroids") == 0)
    {
      debug_status |= debug_factoroids;
    }
    else if (strcmp(argv[i], "--debug-lan") == 0)
    {
      debug_status |= debug_lan;
    }
    else if (strcmp(argv[i], "--debug-mathcards") == 0)
    {
      debug_status |= debug_mathcards;
    }
    else if (strcmp(argv[i], "--debug-sdl") == 0)
    {
      debug_status |= debug_sdl;
    }
    else if (strcmp(argv[i], "--debug-lessons") == 0)
    {
      debug_status |= debug_lessons;
    }
    else if (strcmp(argv[i], "--debug-highscore") == 0)
    {
      debug_status |= debug_highscore;
    }
    else if (strcmp(argv[i], "--debug-options") == 0)
    {
      debug_status |= debug_options;
    }
    else
    /* TODO try to match unrecognized strings to config file names */
    {
      /* Display 'made' usage: */

      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      usage(1, argv[0]);
    }
  }/* end of command-line args */

  DEBUGMSG(debug_setup,"debug_status: %x", debug_status);

  if (Opts_DemoMode() && Opts_GetGlobalOpt(USE_KEYPAD))
  {
    fprintf(stderr, "No use for keypad in demo mode!\n");
    Opts_SetGlobalOpt(USE_KEYPAD, 0);
  }
}

void initialize_SDL(void)
{
  // Audio parameters
  int frequency, channels, n_timesopened;
  Uint16 format;

  /* Init SDL Video: */
  screen = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr,
           "\nError: I could not initialize video!\n"
           "The Simple DirectMedia error that occured was:\n"
           "%s\n\n", SDL_GetError());
    cleanup_on_error();
    exit(1);
  }

  /* This sets up whichever text library we are using: */
  if (!Setup_SDL_Text())
  {
    fprintf( stderr, "Couldn't initialize text (SDL_ttf or SDL_Pango)\n");
    cleanup_on_error();
    exit(2);
  }

//  atexit(TTF_Quit); // Maybe this is redundant?



  #ifndef NOSOUND
  /* Init SDL Audio: */
  Opts_SetSoundHWAvailable(0);  // By default no sound HW
  if (Opts_GetGlobalOpt(USE_SOUND))
  {
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
      fprintf(stderr,
            "\nWarning: I could not initialize audio!\n"
            "The Simple DirectMedia error that occured was:\n"
            "%s\n\n", SDL_GetError());
    }
    else {
      //if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0)
      if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16SYS, 2, 2048) < 0)
      {
        fprintf(stderr,
                "\nWarning: I could not set up audio for 44100 Hz "
                "16-bit stereo.\n"
                "The Simple DirectMedia error that occured was:\n"
                "%s\n\n", SDL_GetError());

      }
    }
    n_timesopened = Mix_QuerySpec(&frequency,&format,&channels);
    if (n_timesopened > 0)
      Opts_SetSoundHWAvailable(1);
    else
      frequency = format = channels = 0; //more helpful than garbage
    DEBUGMSG(debug_setup, "Sound mixer: frequency = %d, "
                          "format = %x, "
                          "channels = %d, "
                          "n_timesopened = %d\n",
                          frequency,format,channels,n_timesopened);
  }

  #endif
  {
    const SDL_VideoInfo *videoInfo;
    Uint32 surfaceMode;
    videoInfo = SDL_GetVideoInfo();
    if (videoInfo->hw_available)
    {
      surfaceMode = SDL_HWSURFACE;
      DEBUGMSG(debug_setup, "HW mode\n");
    }
    else
    {
      surfaceMode = SDL_SWSURFACE;
      DEBUGMSG(debug_setup, "SW mode\n");
    }

    // Determine the current resolution: this will be used as the
    // fullscreen resolution, if the user wants fullscreen.
    DEBUGMSG(debug_setup, "Current resolution: w %d, h %d.\n",videoInfo->current_w,videoInfo->current_h);
    fs_res_x = videoInfo->current_w;
    fs_res_y = videoInfo->current_h;

    if (Opts_GetGlobalOpt(FULLSCREEN))
    {
      screen = SDL_SetVideoMode(fs_res_x, fs_res_y, PIXEL_BITS, SDL_FULLSCREEN | surfaceMode);
      if (screen == NULL)
      {
        fprintf(stderr,
              "\nWarning: I could not open the display in fullscreen mode.\n"
              "The Simple DirectMedia error that occured was:\n"
              "%s\n\n", SDL_GetError());
        Opts_SetGlobalOpt(FULLSCREEN, 0);
      }
    }

    if (!Opts_GetGlobalOpt(FULLSCREEN))
    {
      screen = SDL_SetVideoMode(win_res_x, win_res_y, PIXEL_BITS, surfaceMode);
    }

    if (screen == NULL)
    {
      fprintf(stderr,
            "\nError: I could not open the display.\n"
            "The Simple DirectMedia error that occured was:\n"
            "%s\n\n", SDL_GetError());
      cleanup_on_error();
      exit(1);
    }

    seticon();

    SDL_WM_SetCaption("Tu History", "TuxHistory");
  }

  /* --- Define the colors we use --- */
  /* This was moved here because of wanting to replace some images
     with fonts during setup */
  black.r       = 0x00; black.g       = 0x00; black.b       = 0x00;
  gray.r        = 0x80; gray.g        = 0x80; gray.b        = 0x80;
  dark_blue.r   = 0x00; dark_blue.g   = 0x00; dark_blue.b   = 0x60;
  red.r         = 0xff; red.g         = 0x00; red.b         = 0x00;
  white.r       = 0xff; white.g       = 0xff; white.b       = 0xff;
  yellow.r      = 0xff; yellow.g      = 0xff; yellow.b      = 0x00;
}


void load_data_files(void)
{
  if (!load_sound_data())
  {
    fprintf(stderr, "\nCould not load sound file - attempting to proceed without sound.\n");
    Opts_SetSoundHWAvailable(0);
  }

  /* This now has to come after loading the font, because it replaces
     a couple of images with translatable versions. */
  /* NOTE now the text code will load the font if it isn't already loaded */
  if (!load_image_data())
  {
    fprintf(stderr, "\nCould not load image file - exiting!\n");
    cleanup_on_error();
    exit(1);
  }
}

void data_memory_alloc(void)
{
    int i;
    object = NULL;
    object = (th_obj*)malloc(MAX_DEF_OBJECTS * sizeof(th_obj));
    if (object == NULL)
    {
        DEBUGMSG(debug_setup, "Allocation of game objects faild!\n");
        exit(1);
    }

    map = NULL;
    map = (th_map**)malloc(MAX_X_TILDES * sizeof(th_map *));
    if(map == NULL)
    {
        DEBUGMSG(debug_setup,"Error\n");
        exit(1);
    }
    for(i = 0; i < MAX_X_TILDES; i++){
        map[i] = NULL;
        map[i] = (th_map*)malloc(MAX_Y_TILDES * sizeof(th_map));
        if(map[i] == NULL)
        {
            DEBUGMSG(debug_setup, "Error: Allocation of objects faild!\n");
            exit(1);
        }
    }

}

/* save options and free heap */
/* use for successful exit */
void cleanup(void)
{
  /* No longer write settings here, because we only */
  /* want to save settings from certain types of games. */
  //write_user_config_file();
  cleanup_memory();
//  exit(0);
}



/* save options and free heap */
/* use for fail exit */
void cleanup_on_error(void)
{
  cleanup_memory();
  exit(1);
}



/* free any heap memory used during game DSB */
/* and also quit SDL properly:               */
/* NOTE - this function will get called twice if   */
/* exit occurs because of window close, so we      */
/* need to check all pointers before freeing them, */
/* and set them to NULL after freeing them, so we  */
/* avoid segfaults at exit from double free()      */
void cleanup_memory(void)
{
  int i;
  int frequency,channels,n_timesopened;
  Uint16 format;

  /* Free global memory allocation ( from memory_data_alloc() )*/
  FREE(object);

  for(i = 0; i < MAX_X_TILDES; i++)
  {
      FREE(map[i]);
  }
  FREE(map);

  free_map();

  /* Free all images and sounds used by SDL: */
  Cleanup_SDL_Text();

  for (i = 0; i < NUM_IMAGES; i++)
  {
    if (images[i])
      SDL_FreeSurface(images[i]);
    images[i] = NULL;
  }

  for (i = 0; i < NUM_TERRAINS; i++)
  {
    if (terrain[i])
      SDL_FreeSurface(terrain[i]);
    terrain[i] = NULL;
  }

  for (i = 0; i < NUM_OBJECTS; i++)
  {
    if (objects[i])
      SDL_FreeSurface(objects[i]);
    objects[i] = NULL;
  }

  for (i = 0; i < NUM_SOUNDS; i++)
  {
    if (sounds[i])
      Mix_FreeChunk(sounds[i]);
    sounds[i] = NULL;
  }

  for (i = 0; i < NUM_MUSICS; i++)
  {
    if (musics[i])
      Mix_FreeMusic(musics[i]);
    musics[i] = NULL;
  }

  // Close the audio mixer. We have to do this at least as many times
  // as it was opened.
  n_timesopened = Mix_QuerySpec(&frequency,&format,&channels);
  while (n_timesopened) {
    Mix_CloseAudio();
    n_timesopened--;
  }

  UnloadMenus();

  // Finally, quit SDL
  SDL_Quit();

  /* frees the game_options struct: */
  Opts_Cleanup();
  }



/* Set the application's icon: */

void seticon(void)
{
  int masklen;
  Uint8* mask;
  SDL_Surface* icon;


  /* Load icon into a surface: */
  icon = IMG_Load(DATA_PREFIX "/images/icons/icon.png");
  if (icon == NULL)
  {
    fprintf(stderr,
            "\nWarning: I could not load the icon image: %s\n"
            "The Simple DirectMedia error that occured was:\n"
            "%s\n\n", DATA_PREFIX "/images/icons/icon.png", SDL_GetError());
    return;
  }


  /* Create mask: */
  masklen = (((icon -> w) + 7) / 8) * (icon -> h);
  mask = malloc(masklen * sizeof(Uint8));
  memset(mask, 0xFF, masklen);


  /* Set icon: */
  SDL_WM_SetIcon(icon, mask);


  /* Free icon surface & mask: */
  free(mask);
  SDL_FreeSurface(icon);


  /* Seed random-number generator: */
  srand(SDL_GetTicks());
}

void usage(int err, char * cmd)
{
  FILE * f;

  if (err == 0)
    f = stdout;
  else
    f = stderr;

  fprintf(f,
   "\nUsage: %s {--help | --usage | --copyright}\n"
   "          [--optionfile <filename>]\n"
   "          [--playthroughlist] [--answersfirst] [--answersmiddle]\n"
   "       %s [--fullscreen] [--nosound] [--nobackground]\n"
   "          [--demo] [--keypad] [--allownegatives]\n"
//   "          [--operator {add | subtract | multiply | divide} ...]\n"
   "          [--speed <val>]\n"
    "\n", cmd, cmd);

  exit (err);
}

