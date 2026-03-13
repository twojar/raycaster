//
// Created by Alan Pitcher on 11/22/2025.
//
#include<SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "game/player.h"
#include "game/map.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define NUM_TEXTURES 8

void init_Graphics(SDL_Renderer *renderer);
double dda(double startX, double startY, double rayDirX, double rayDirY, int *refSide, int *refMapX, int *refMapY);
void draw_frame(SDL_Renderer* renderer, Player* player);
void load_fogTable();
void worldMap_free();

#endif //GRAPHICS_H
