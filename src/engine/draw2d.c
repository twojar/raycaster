#include "engine/draw2d.h"

#include <stdio.h>

#include "engine/graphics.h"

void draw2d_pixel(int x, int y, Uint32 color) {
    gfx_put_pixel(x, y, color);
}

void draw2d_line(int x0, int y0, int x1, int y1, Uint32 color) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -((y1 > y0) ? (y1 - y0) : (y0 - y1));
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        draw2d_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;

        int err2 = 2 * err;
        if (err2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (err2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw2d_rect(int x, int y, int width, int height, Uint32 color) {
    for (int py = y; py < y + height; py++) {
        for (int px = x; px < x + width; px++) {
            draw2d_pixel(px, py, color);
        }
    }
}

void draw2d_rect_outline(int x, int y, int width, int height, Uint32 color) {
    draw2d_line(x, y, x + width - 1, y, color);
    draw2d_line(x, y + height - 1, x + width - 1, y + height - 1, color);
    draw2d_line(x, y, x, y + height - 1, color);
    draw2d_line(x + width - 1, y, x + width - 1, y + height - 1, color);
}

void draw2d_crosshair(int centerX, int centerY, int radius, Uint32 color) {
    for (int offset = -radius; offset <= radius; offset++) {
        if (offset == 0) continue;
        draw2d_pixel(centerX + offset, centerY, color);
        draw2d_pixel(centerX, centerY + offset, color);
    }
}

void draw2d_char(unsigned char c, int x, int y, Uint32 color) {
    gfx_draw_char(c, x, y, color);
}

void draw2d_char_scaled(unsigned char c, int x, int y, Uint32 color, float scale) {
    gfx_draw_char_scaled(c, x, y, color, scale);
}

void draw2d_text(const char *text, int x, int y, Uint32 color) {
    gfx_draw_text(text, x, y, color);
}

void draw2d_text_scaled(const char *text, int x, int y, Uint32 color, float scale) {
    if (text == NULL || scale <= 0.0f) return;

    int currentX = x;
    int currentY = y;
    int advanceX = (int)(24.0f * scale);
    int advanceY = (int)(32.0f * scale);

    if (advanceX <= 0) advanceX = 1;
    if (advanceY <= 0) advanceY = 1;

    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = (unsigned char)text[i];
        if (c == '\n') {
            currentX = x;
            currentY += advanceY;
            continue;
        }

        draw2d_char_scaled(c, currentX, currentY, color, scale);
        currentX += advanceX;
    }
}

void draw2d_int(int value, int x, int y, Uint32 color) {
    char buffer[32];
    SDL_snprintf(buffer, sizeof(buffer), "%d", value);
    draw2d_text(buffer, x, y, color);
}
