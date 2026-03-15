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
#define SCREEN_HEIGHT 240

//  Target window size (GPU-side scaling)
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

//  Standard texture dimensions and limits
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64
#define NUM_TEXTURES 8

//  Initializes renderer settings and loads all core game textures
void gfx_init(SDL_Renderer *renderer);

//  Standard DDA algorithm to find distance to nearest wall along a ray
//  returns distance and fills refSide, refMapX, refMapY for collision/shading data
double gfx_dda(double startX, double startY, double rayDirX, double rayDirY, int *refSide, int *refMapX, int *refMapY);

//  Renders the complete 3D scene view including walls, floors, and sprites
void gfx_draw_frame(SDL_Renderer* renderer, Player* player, double alpha);

//  Pre-calculates fog lookup table based on depth distance for faster rendering
void gfx_load_fog_table();

//  Renders a string of text to the screen buffer using the 8x8 font map
void gfx_draw_text(const char* text, int x, int y, Uint32 color);

//  Finalizes the frame by uploading the CPU buffer to the GPU texture and rendering it
void gfx_present(SDL_Renderer* renderer);

#endif //GRAPHICS_H
