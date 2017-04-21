/*
 * ui_window.c
 * This file is part of Tux4Kids
 *
 * Copyright (C) 2010 - Aviral Dasgupta
 *
 * Tux4Kids is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tux4Kids is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tux4Kids; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */
#include <stdio.h>
#include "ui_window.h"
#include "ui_colors.h"
#include "ui_layouts.h"
#include "ui_proxywidget.h"
#include "util.h"
#include <SDL/SDL.h>

struct _UI_Window {
	int x, y, width, height;
	SDL_Surface *canvas;
	LinkedList *widgets;
};

struct _UI_Window *ui_CreateWindow(int x, int y, int width, int height) {
	struct _UI_Window *window = malloc(sizeof(struct _UI_Window));
	window->x = x;
	window->y = y;
	window->width = width;
	window->height = height;
	window->canvas = SDL_CreateRGBSurface(SDL_SRCALPHA, width, height, 32, 0, 0, 0, 0);
	window->widgets = list_Create();
	SDL_FillRect(window->canvas, NULL, C_BLACK);
	return window;
}

void ui_DestroyWindow(struct _UI_Window *window) {
	SDL_FreeSurface(window->canvas);
	free(window);
}

static UI_ProxyWidget *_ui_FindWidget(struct _UI_Window *window, int x, int y) {
	Node *node = list_GetFirst(window->widgets);
	while(node!=NULL) {
		if(ui_WidgetContains((UI_ProxyWidget *)list_GetNodeValue(node), x, y))
			return (UI_ProxyWidget *)list_GetNodeValue(node);
		node = list_Traverse(node);
	}
	return NULL;
}

void ui_onWindowEvent(struct _UI_Window *window, int x, int y) {
	//note: the subtraction stuff on the line below converts screen coords to window coords
	UI_ProxyWidget *widget = _ui_FindWidget(window, x - window->x, y - window->y);
	if(widget!=NULL) {
		ui_onWidgetEvent(widget, x, y);
	}
}

int ui_WindowContains(struct _UI_Window *window, int x, int y) {
	//fprintf(stdout, "X:%d Y:%d W:%d H:%d / px: %d py: %d\n", window->x, window->y, window->width, window->height, x, y);
	return ((x > window->x) && (y > window->y) && (x < (window->x + window->width)) && (y < (window->y + window->height)))?1:0;
}

void ui_DrawWindow(struct _UI_Window *window, SDL_Surface *screen) {
	//redraw widgets
    SDL_Rect dest, title_rect;


	Node *node = list_GetFirst(window->widgets);
	while(node!=NULL) {
		ui_PaintWidget((UI_ProxyWidget *)list_GetNodeValue(node), window->canvas);
		node = list_Traverse(node);
	}

	SDL_Rect position;
	position.x = window->x;
	position.y = window->y;


    // Construct title
    title_rect.x = 2;
    title_rect.y = 2;
    title_rect.w = window->canvas->w-2;
    title_rect.h = 30;
	SDL_FillRect(window->canvas, &title_rect, 0x881111);
	SDL_BlitSurface(window->canvas, NULL, screen, &position);
    th_ShowMessage("TuxHistory Window", 16, window->x+5, window->y+5);
}

void ui_AddWidget(struct _UI_Window *window, void *widget, WidgetType type) {
    printf("NewPoxy!\n");
	UI_ProxyWidget *proxy = ui_CreateProxyWidget(widget, type);
    printf("NewPOS!\n");
	SDL_Rect pos = layout_FGetPos();
    printf("SetPos!\n");
	ui_SetWidgetPosition(proxy, pos.x, pos.y);
    printf("Add to window!\n");
	list_AddNode(window->widgets, proxy);
}
