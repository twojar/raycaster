//
// Created by Alan Pitcher on 11/22/2025.
//
#include<SDL3/SDL.h>
#include "player.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 360
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define NUM_TEXTURES 1

extern int worldMap[MAP_WIDTH][MAP_HEIGHT];
extern Uint32 screenBuffer[WINDOW_HEIGHT][WINDOW_WIDTH];
extern Uint32 textures[NUM_TEXTURES][TEXTURE_WIDTH * TEXTURE_HEIGHT];

void init_Graphics(SDL_Renderer *renderer);

typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} ColorRGB;

void draw_frame(SDL_Renderer* renderer, Player* player);


#endif //GRAPHICS_H
