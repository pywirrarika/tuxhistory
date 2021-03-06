/*
C Interface: fileops.h

Description: File operations - together, fileops.h and fileops.c contain
all code involving disk operations. The older header files images.h and
sounds.h have been incorporated here. The intention is to make it easier to
port tuxmath to other operating systems, as code to read and write as
well as paths and file locations may be more OS-dependent.


Author: David Bruce <davidstuartbruce@gmail.com>, (C) 2006
Contains code originally written by Bill Kendrick (C) 2001.
Copyright: See COPYING file that comes with this distribution (briefly, GNU GPL)
*/

#ifndef FILEOPS_H
#define FILEOPS_H

#include "globals.h"

/* Flag basically telling whether or not to allow admin-level */
/* settings to be changed: */
enum {
  USER_CONFIG_FILE,
  GLOBAL_CONFIG_FILE
};

/* Names for images (formerly in images.h) */
enum {
  IMG_TITLE,
  IMG_LEFT,
  IMG_LEFT_GRAY,
  IMG_RIGHT,
  IMG_RIGHT_GRAY,
  IMG_TUX4KIDS,
  IMG_NBS,
  IMG_NUMS,
  IMG_LEDNUMS,
  IMG_LED_NEG_SIGN,
  IMG_PAUSED,
  IMG_DEMO,
  IMG_DEMO_SMALL,
  IMG_KEYPAD,
  IMG_KEYPAD_NO_NEG,
  IMG_CONSOLE_LED,
  IMG_CONSOLE_BASH,
  IMG_TUX_CONSOLE1,
  IMG_TUX_CONSOLE2,
  IMG_TUX_CONSOLE3,
  IMG_TUX_CONSOLE4,
  IMG_TUX_RELAX1,
  IMG_TUX_RELAX2,
  IMG_TUX_EGYPT1,
  IMG_TUX_EGYPT2,
  IMG_TUX_EGYPT3,
  IMG_TUX_EGYPT4,
  IMG_TUX_DRAT,
  IMG_TUX_YIPE,
  IMG_TUX_YAY1,
  IMG_TUX_YAY2,
  IMG_TUX_YES1,
  IMG_TUX_YES2,
  IMG_TUX_SIT,
  IMG_TUX_FIST1,
  IMG_TUX_FIST2,
  IMG_WAVE,
  IMG_SCORE,
  IMG_STOP,
  IMG_NUMBERS,
  IMG_GAMEOVER,
  IMG_GAMEOVER_WON,
  IMG_ISOMAPPER,
  IMG_ISOSELECT,
  IMG_ISOGO,
  IMG_ISOWRONG,
  IMG_EXPLORED,
  IMG_NOVISIBLE,
  IMG_CONSTRUCTION,
  IMG_GUIBG_BYZANTINE,
  IMG_GUIBUILD,
  IMG_GUIKILL,
  IMG_GUIPAWN,
  NUM_IMAGES
};

/* Names for animated images (sprites) */
enum {
  IMG_COMET,
  IMG_BONUS_COMET,
  IMG_COMET_EXPL,
  IMG_BONUS_COMET_EXPL,
  NUM_SPRITES
};

// NUM_OF_TERRAINS must be the last element.
// Feel free to use this arguments in the th_terrain
// array, and in your xml - map files. Borders
// will be generated by th_make_map()
enum{
    HIGHSEA,
    TUNDRA,
    SWAMP,
    UNEXPLORED,
    DESERT,
    GRASSLAND,
    ARCTIC,
    OCEAN,
    MARSH,
    SAVANNAH,
    PLAINS,
    PRAIRIE,
    NUM_OF_TERRAINS
};


/*  ---------------------------
 * |        |         |        |
 * |        |         |        |
 * |     NW |    N    | NE     |
 *  --------+---------+-------- 
 * |        | NW N NE |        |
 * |        |  W C E  | E      |
 * |        | SW S SE |        |
 *  --------+---------+-------- 
 * |     SW |    S    | SE     |
 * |        |         |        |
 * |        |         |        |
 *  --------------------------- 
 */
enum{
    BORDER_E_ODD,
    BORDER_SW_EVEN,
    BORDER_NW_ODD,
    BORDER_W_EVEN,
    BORDER_NE_ODD,
    BORDER_SE_EVEN,
    BORDER_W_ODD,
    BORDER_E_EVEN,
    BORDER_SW_ODD,
    BORDER_NW_EVEN,
    CENTER_0,
    BORDER_N_EVEN,
    BODER_SE_ODD,
    BORDER_NE_EVEN,
    BORDER_S_ODD,
    BORDER_N_ODD,
    CENTER_1,
    BORDER_S_EVEN,
    NUM_COMPTILDE
};

enum {
    HIGHSEAS_BORDER_E_ODD,
    HIGHSEAS_BORDER_SW_EVEN,
    HIGHSEAS_BORDER_NW_ODD,
    HIGHSEAS_BORDER_W_EVEN,
    HIGHSEAS_BORDER_NE_ODD,
    HIGHSEAS_BORDER_SE_EVEN,
    HIGHSEAS_BORDER_W_ODD,
    HIGHSEAS_BORDER_E_EVEN,
    HIGHSEAS_BORDER_SW_ODD,
    HIGHSEAS_BORDER_NW_EVEN,
    HIGHSEAS_CENTER_0,
    HIGHSEAS_BORDER_N_EVEN,
    HIGHSEAS_BODER_SE_ODD,
    HIGHSEAS_BORDER_NE_EVEN,
    HIGHSEAS_BORDER_S_ODD,
    HIGHSEAS_BORDER_N_ODD,
    HIGHSEAS_CENTER_1,
    HIGHSEAS_BORDER_S_EVEN,
    TUNDRA_BORDER_E_ODD,
    TUNDRA_BORDER_SW_EVEN,
    TUNDRA_BORDER_NW_ODD,
    TUNDRA_BORDER_W_EVEN,
    TUNDRA_BORDER_NE_ODD,
    TUNDRA_BORDER_SE_EVEN,
    TUNDRA_BORDER_W_ODD,
    TUNDRA_BORDER_E_EVEN,
    TUNDRA_BORDER_SW_ODD,
    TUNDRA_BORDER_NW_EVEN,
    TUNDRA_CENTER_0,
    TUNDRA_BORDER_N_EVEN,
    TUNDRA_BODER_SE_ODD,
    TUNDRA_BORDER_NE_EVEN,
    TUNDRA_BORDER_S_ODD,
    TUNDRA_BORDER_N_ODD,
    TUNDRA_CENTER_1,
    TUNDRA_BORDER_S_EVEN,
    SWAMP_BORDER_E_ODD,
    SWAMP_BORDER_SW_EVEN,
    SWAMP_BORDER_NW_ODD,
    SWAMP_BORDER_W_EVEN,
    SWAMP_BORDER_NE_ODD,
    SWAMP_BORDER_SE_EVEN,
    SWAMP_BORDER_W_ODD,
    SWAMP_BORDER_E_EVEN,
    SWAMP_BORDER_SW_ODD,
    SWAMP_BORDER_NW_EVEN,
    SWAMP_CENTER_0,
    SWAMP_BORDER_N_EVEN,
    SWAMP_BODER_SE_ODD,
    SWAMP_BORDER_NE_EVEN,
    SWAMP_BORDER_S_ODD,
    SWAMP_BORDER_N_ODD,
    SWAMP_CENTER_1,
    SWAMP_BORDER_S_EVEN,
    UNEXPLORED_BORDER_E_ODD,
    UNEXPLORED_BORDER_SW_EVEN,
    UNEXPLORED_BORDER_NW_ODD,
    UNEXPLORED_BORDER_W_EVEN,
    UNEXPLORED_BORDER_NE_ODD,
    UNEXPLORED_BORDER_SE_EVEN,
    UNEXPLORED_BORDER_W_ODD,
    UNEXPLORED_BORDER_E_EVEN,
    UNEXPLORED_BORDER_SW_ODD,
    UNEXPLORED_BORDER_NW_EVEN,
    UNEXPLORED_CENTER_0,
    UNEXPLORED_BORDER_N_EVEN,
    UNEXPLORED_BODER_SE_ODD,
    UNEXPLORED_BORDER_NE_EVEN,
    UNEXPLORED_BORDER_S_ODD,
    UNEXPLORED_BORDER_N_ODD,
    UNEXPLORED_CENTER_1,
    UNEXPLORED_BORDER_S_EVEN,
    DESERT_BORDER_E_ODD,
    DESERT_BORDER_SW_EVEN,
    DESERT_BORDER_NW_ODD,
    DESERT_BORDER_W_EVEN,
    DESERT_BORDER_NE_ODD,
    DESERT_BORDER_SE_EVEN,
    DESERT_BORDER_W_ODD,
    DESERT_BORDER_E_EVEN,
    DESERT_BORDER_SW_ODD,
    DESERT_BORDER_NW_EVEN,
    DESERT_CENTER_0,
    DESERT_BORDER_N_EVEN,
    DESERT_BODER_SE_ODD,
    DESERT_BORDER_NE_EVEN,
    DESERT_BORDER_S_ODD,
    DESERT_BORDER_N_ODD,
    DESERT_CENTER_1,
    DESERT_BORDER_S_EVEN,
    GRASSLAND_BORDER_E_ODD,
    GRASSLAND_BORDER_SW_EVEN,
    GRASSLAND_BORDER_NW_ODD,
    GRASSLAND_BORDER_W_EVEN,
    GRASSLAND_BORDER_NE_ODD,
    GRASSLAND_BORDER_SE_EVEN,
    GRASSLAND_BORDER_W_ODD,
    GRASSLAND_BORDER_E_EVEN,
    GRASSLAND_BORDER_SW_ODD,
    GRASSLAND_BORDER_NW_EVEN,
    GRASSLAND_CENTER_0,
    GRASSLAND_BORDER_N_EVEN,
    GRASSLAND_BODER_SE_ODD,
    GRASSLAND_BORDER_NE_EVEN,
    GRASSLAND_BORDER_S_ODD,
    GRASSLAND_BORDER_N_ODD,
    GRASSLAND_CENTER_1,
    GRASSLAND_BORDER_S_EVEN,
    ARCTIC_BORDER_E_ODD,
    ARCTIC_BORDER_SW_EVEN,
    ARCTIC_BORDER_NW_ODD,
    ARCTIC_BORDER_W_EVEN,
    ARCTIC_BORDER_NE_ODD,
    ARCTIC_BORDER_SE_EVEN,
    ARCTIC_BORDER_W_ODD,
    ARCTIC_BORDER_E_EVEN,
    ARCTIC_BORDER_SW_ODD,
    ARCTIC_BORDER_NW_EVEN,
    ARCTIC_CENTER_0,
    ARCTIC_BORDER_N_EVEN,
    ARCTIC_BODER_SE_ODD,
    ARCTIC_BORDER_NE_EVEN,
    ARCTIC_BORDER_S_ODD,
    ARCTIC_BORDER_N_ODD,
    ARCTIC_CENTER_1,
    ARCTIC_BORDER_S_EVEN,
    OCEAN_BORDER_E_ODD,
    OCEAN_BORDER_SW_EVEN,
    OCEAN_BORDER_NW_ODD,
    OCEAN_BORDER_W_EVEN,
    OCEAN_BORDER_NE_ODD,
    OCEAN_BORDER_SE_EVEN,
    OCEAN_BORDER_W_ODD,
    OCEAN_BORDER_E_EVEN,
    OCEAN_BORDER_SW_ODD,
    OCEAN_BORDER_NW_EVEN,
    OCEAN_CENTER_0,
    OCEAN_BORDER_N_EVEN,
    OCEAN_BODER_SE_ODD,
    OCEAN_BORDER_NE_EVEN,
    OCEAN_BORDER_S_ODD,
    OCEAN_BORDER_N_ODD,
    OCEAN_CENTER_1,
    OCEAN_BORDER_S_EVEN,
    MARSH_BORDER_E_ODD,
    MARSH_BORDER_SW_EVEN,
    MARSH_BORDER_NW_ODD,
    MARSH_BORDER_W_EVEN,
    MARSH_BORDER_NE_ODD,
    MARSH_BORDER_SE_EVEN,
    MARSH_BORDER_W_ODD,
    MARSH_BORDER_E_EVEN,
    MARSH_BORDER_SW_ODD,
    MARSH_BORDER_NW_EVEN,
    MARSH_CENTER_0,
    MARSH_BORDER_N_EVEN,
    MARSH_BODER_SE_ODD,
    MARSH_BORDER_NE_EVEN,
    MARSH_BORDER_S_ODD,
    MARSH_BORDER_N_ODD,
    MARSH_CENTER_1,
    MARSH_BORDER_S_EVEN,
    SAVANNAH_BORDER_E_ODD,
    SAVANNAH_BORDER_SW_EVEN,
    SAVANNAH_BORDER_NW_ODD,
    SAVANNAH_BORDER_W_EVEN,
    SAVANNAH_BORDER_NE_ODD,
    SAVANNAH_BORDER_SE_EVEN,
    SAVANNAH_BORDER_W_ODD,
    SAVANNAH_BORDER_E_EVEN,
    SAVANNAH_BORDER_SW_ODD,
    SAVANNAH_BORDER_NW_EVEN,
    SAVANNAH_CENTER_0,
    SAVANNAH_BORDER_N_EVEN,
    SAVANNAH_BODER_SE_ODD,
    SAVANNAH_BORDER_NE_EVEN,
    SAVANNAH_BORDER_S_ODD,
    SAVANNAH_BORDER_N_ODD,
    SAVANNAH_CENTER_1,
    SAVANNAH_BORDER_S_EVEN,
    PLAINS_BORDER_E_ODD,
    PLAINS_BORDER_SW_EVEN,
    PLAINS_BORDER_NW_ODD,
    PLAINS_BORDER_W_EVEN,
    PLAINS_BORDER_NE_ODD,
    PLAINS_BORDER_SE_EVEN,
    PLAINS_BORDER_W_ODD,
    PLAINS_BORDER_E_EVEN,
    PLAINS_BORDER_SW_ODD,
    PLAINS_BORDER_NW_EVEN,
    PLAINS_CENTER_0,
    PLAINS_BORDER_N_EVEN,
    PLAINS_BODER_SE_ODD,
    PLAINS_BORDER_NE_EVEN,
    PLAINS_BORDER_S_ODD,
    PLAINS_BORDER_N_ODD,
    PLAINS_CENTER_1,
    PLAINS_BORDER_S_EVEN,
    PRAIRIE_BORDER_E_ODD,
    PRAIRIE_BORDER_SW_EVEN,
    PRAIRIE_BORDER_NW_ODD,
    PRAIRIE_BORDER_W_EVEN,
    PRAIRIE_BORDER_NE_ODD,
    PRAIRIE_BORDER_SE_EVEN,
    PRAIRIE_BORDER_W_ODD,
    PRAIRIE_BORDER_E_EVEN,
    PRAIRIE_BORDER_SW_ODD,
    PRAIRIE_BORDER_NW_EVEN,
    PRAIRIE_CENTER_0,
    PRAIRIE_BORDER_N_EVEN,
    PRAIRIE_BODER_SE_ODD,
    PRAIRIE_BORDER_NE_EVEN,
    PRAIRIE_BORDER_S_ODD,
    PRAIRIE_BORDER_N_ODD,
    PRAIRIE_CENTER_1,
    PRAIRIE_BORDER_S_EVEN,
    NUM_TERRAINS
};

enum{
    FOREST_MIXED, // Forest images
    FOREST_TROPICAL,
    FOREST_CONIFER,
    FOREST_SCRUB,
    FOREST_BOREAL,
    FOREST_WETLAND,
    FOREST_RAIN,
    FOREST_BROADLEAF,
    FOREST_USED,
    REC_GOLD_1,
    REC_STONE_1,
    VILLAGER_MILKMAID,
    HOUSE,
    HOUSE_NEW,
    HUT,
    HUT_NEW,
    VILLAGE_CENTER,
    VILLAGE_CENTER_NEW,
    FARM,
    FARM_NEW,
    NUM_OBJECTS // Must be at the end.
};

/* Names for game sounds (formerly in sounds.h): */
enum {
  SND_HARP,
  SND_POP,
  SND_TOCK,
  SND_LASER,
  SND_BUZZ,
  SND_ALARM,
  SND_SHIELDSDOWN,
  SND_EXPLOSION,
  SND_SIZZLE,
  SND_BONUS_COMET,
  SND_EXTRA_LIFE,
  NUM_SOUNDS
};

/* Names for background music (also formerly in sounds.h): */
enum {
  MUS_GAME,
  MUS_GAME2,
  MUS_GAME3,
  NUM_MUSICS
};

/* Names for game summary files: */
enum {
  SUMMARY1,
  SUMMARY2,
  SUMMARY3,
  SUMMARY4,
  SUMMARY5,
  SUMMARY6,
  SUMMARY7,
  SUMMARY8,
  SUMMARY9,
  SUMMARY10,
  NUM_SUMMARIES
};

/* These functions used by setup() and titlescreen() to read in settings: */
int read_global_config_file(void);
int read_user_config_file(void);
int parse_lesson_file_directory(void);
int read_named_config_file(const char* fn);
int write_user_config_file(void);
int read_high_scores(void);
int append_high_score(int tableid,int score,char *player_name);
void set_high_score_path(void);
void set_user_data_dir(const char* dirname);
int write_goldstars(void);

/* These functions are used by titlescreen() to assist with the login */
int read_user_menu_entries(char ***user_names);
int read_user_login_questions(char ***user_login_questions);
int high_scores_found_in_user_dir(void);
void set_high_score_path(void);
void user_data_dirname_up(void);
void user_data_dirname_down(char *subdir);

/* These functions used by game() to record game summary: */
int write_pregame_summary(void);
int write_postgame_summary(void);

int load_image_data();


#ifndef NOSOUND
int load_sound_data();
#endif

#endif
