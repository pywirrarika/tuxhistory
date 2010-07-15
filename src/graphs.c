/*
 * graphs.c
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
#include<stdio.h>
#include<stdlib.h>

#include "globals.h"
#include "graphs.h"
#include "players.h"

static int gmaps_alloc(int xsize, int ysize, int maps);
static void gmaps_free(int xsize, int ysize, int maps);

//gmaps is a tree dimensional array og gnode's, each 
//gnode must be linked with their niehboors.
static int gmaps_alloc(int xsize, int ysize, int maps)
{
    int i,j;
    gmaps = (gnode ***)malloc(maps * sizeof(gnode **));
    if(gmaps[i] == NULL)
    {
        printf("Error: Allocation of objects faild!\n");
        return 1; 
    }

    for(i = 0; i < maps; i++)
    {
        gmaps[i] = (gnode **)malloc(xsize * sizeof(gnode *));
        if(gmaps[i] == NULL)
        {
            printf("Error: Allocation of objects faild!\n");
            return 1; 
        }
        for(j = 0; j < ysize; j++)
        {
            gmaps[i][j] = (gnode *)malloc(ysize * sizeof(gnode));
            if(gmaps[i][j] == NULL)
            {
                printf("Error: Allocation of objects faild!\n");
                return 1; 
            }
        }
    }
    return 0;
}

static void gmaps_free(int xsize, int ysize, int maps)
{
    int i, j;
    for(i = 0; i < maps; i++)
    {
        for(j = 0; j < xsize; j++)
        {
            FREE(gmaps[i][j]);
        }
        FREE(gmaps[i]);
    }
}

int create_gmaps(int players)
{
    int i,j,k,l;
    int count;
    th_point point;
    th_vector vector;
    
    players++;
    if(gmaps_alloc(x_tildes, y_tildes, players))
    {
        printf("Error: out of memory!\n");
        return 1;
    }

    for(i = 1; i < players; i++)
    {
        count = 0;
        for(j = 0; j < x_tildes; j++)
        {
            for(k = 0; k < y_tildes; k++)
            {
                count++;
                gmaps[i][j][k].visible = 1;
                for(l = 0; l < NUM_DIRS; l++)
                {
                    point.x = j;
                    point.y = k;
                    vector = get_vector(point, l);
                    if(vector.x != -2 && vector.y != -2)
                    {
                        gmaps[i][j][k].nodes[l] = &gmaps[0][j+vector.x][k+vector.y];
                    }
                    else
                    {
                        gmaps[i][j][k].nodes[l] = NULL;
                    }
                    if(i > 0)
                    {
                        gmaps[i][j][k].visible = 1;
                        gmaps[i][j][k].explored = 1;
                    }
                    else
                    {
                        gmaps[i][j][k].visible = 0;
                        gmaps[i][j][k].explored = 0;
                    }
                }
                gmaps[i][j][k].id = count;
            }
        }
    }
}

static void gmaps_free(int xsize, int ysize, int maps)
{
    int i, j;
    for(i = 0; i < maps; i++)
    {
        for(j = 0; j < xsize; j++)
        {
            FREE(gmaps[i][j]);
        }
        FREE(gmaps[i]);
    }
}

void clean_gmaps(int players)
{
   gmaps_free(x_tildes, y_tildes, players);
}


