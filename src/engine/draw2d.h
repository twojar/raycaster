#ifndef DRAW2D_H
#define DRAW2D_H

#include <SDL3/SDL.h>

//  Simple helpers for drawing HUD elements
void draw2d_pixel(int x, int y, Uint32 color);
void draw2d_line(int x0, int y0, int x1, int y1, Uint32 color);
void draw2d_rect(int x, int y, int width, int height, Uint32 color);
void draw2d_rect_outline(int x, int y, int width, int height, Uint32 color);
void draw2d_crosshair(int centerX, int centerY, int radius, Uint32 color);
void draw2d_char(unsigned char c, int x, int y, Uint32 color);
void draw2d_char_scaled(unsigned char c, int x, int y, Uint32 color, float scale);
void draw2d_text(const char *text, int x, int y, Uint32 color);
void draw2d_text_scaled(const char *text, int x, int y, Uint32 color, float scale);
void draw2d_int(int value, int x, int y, Uint32 color);

#endif //DRAW2D_H
