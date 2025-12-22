//
// Created by Alan Pitcher on 11/22/2025.
//
#include<SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "player.h"
#ifndef GRAPHICS_H
#define GRAPHICS_H
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define NUM_TEXTURES 7

extern int *worldMap;
extern int mapCols;
extern int mapRows;

extern Uint32 screenBuffer[WINDOW_HEIGHT][WINDOW_WIDTH];
extern Uint32 textures[NUM_TEXTURES][TEXTURE_WIDTH * TEXTURE_HEIGHT];

void init_Graphics(SDL_Renderer *renderer);
void load_map(char *path);
void draw_frame(SDL_Renderer* renderer, Player* player);


#endif //GRAPHICS_H
