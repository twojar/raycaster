//
// Created by Alan Pitcher on 11/22/2025.
//
#include<SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "game/player.h"
#include "game/map.h"

#ifndef GRAPHICS_H
#define GRAPHICS_H

//  Internal resolution (CPU-side casting)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

//  Target window size (GPU-side scaling)
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define NUM_TEXTURES 8

void gfx_init(SDL_Renderer *renderer);
double gfx_dda(double startX, double startY, double rayDirX, double rayDirY, int *refSide, int *refMapX, int *refMapY);
void gfx_draw_frame(SDL_Renderer* renderer, Player* player);
void gfx_load_fog_table();

#endif //GRAPHICS_H
