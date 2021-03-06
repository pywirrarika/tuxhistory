/*
 * graphs.h
 *
 * Description: Graph functions 
 * 
 * Author: Jesús Manuel Mager Hois (fongog@gmail.com) 2010
 * Copyright: GPL v3 or later
 *
 * Part of "Tux4Kids Project
 * http://www.tux4kids.com
 * 
 */

#ifndef GRAPHS_H
#define GRAPHS_H

#include "SDL.h"
#include "tuxhistory.h"
#include "globals.h"
#include "objects.h"
#include "map.h"

enum
{
    ISO_NW,
    ISO_N,
    ISO_NE,
    ISO_W,
    ISO_E,
    ISO_SW,
    ISO_S,
    ISO_SE,
    NUM_DIRS
};

typedef struct gnode{
    int id;
    th_point point;
    th_point rect;
    th_point anchor; //Anchors in main map surface.
    int visible;
    int explored;
    int drawed;
    int usable; 
    struct gnode *nodes[NUM_DIRS];
    int nodes_flag[NUM_DIRS];
    th_obj *object;
    int terrain;
}gnode;

//gnode gmaps[player][x][y]
gnode ***gmaps;

//Introduce the number of players for a game, abt create_gmaps()
//will create a new graphs mesh in a 3 dimensional array: 
//This function uses **map as reference and x_size y_size
int create_gmaps(int players);

int init_gmaps(void);

int update_gmaps(void);

// Use this function to cleanup after a game.
void clean_gmaps(int players);

#endif 
