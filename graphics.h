//
// Created by Alan Pitcer on 11/22/2025.
//
#include<SDL3/SDL.h>
#include "player.h"
#ifndef GRAPHICS_H
#define GRAPHICS_H

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAP_WIDTH 24
#define MAP_HEIGHT 24

extern int worldMap[MAP_WIDTH][MAP_HEIGHT];

typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} ColorRGB;


void draw_frame(SDL_Renderer* renderer, Player* player);


#endif //GRAPHICS_H
