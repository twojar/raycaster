//
// Created by Alan Pitcher on 11/22/2025.
//
#include "graphics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
extern int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 0
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 1
    {1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1}, // 2 - Pillars top left
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1}, // 3
    {1,0,1,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,1}, // 4
    {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1}, // 5
    {1,1,1,1,1,1,1,0,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1}, // 6
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 7
    {1,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,1,1,1,0,1,1}, // 8 - Maze section
    {1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1}, // 9
    {1,0,1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1}, // 10
    {1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1}, // 11
    {1,0,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1}, // 12
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 13
    {1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1}, // 14
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 15
    {1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,1}, // 16 - Open Hall with pillars
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 17
    {1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,1}, // 18
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 19
    {1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,1}, // 20
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 21
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 22 - Spawn area
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}  // 23
};

Uint32 buffer[WINDOW_HEIGHT][WINDOW_WIDTH];
Uint32 texture[NUM_TEXTURES][TEXTURE_WIDTH * TEXTURE_HEIGHT];
SDL_Texture* screenTexture;
void load_texture(int index,char* path) {
    SDL_Surface* image = SDL_LoadBMP(path);
    if (image == NULL) {
        printf("Unable to load image: %s\n%s\n", path, SDL_GetError());
        return;
    }

    SDL_Surface* formattedImage = SDL_ConvertSurface(image, SDL_PIXELFORMAT_ARGB8888);
    if (formattedImage == NULL) {
        printf("Unable to convert image: %s\n", path);
        return;
    }

    Uint32* pixels = (Uint32*)formattedImage->pixels;
    for (int i = 0; i < formattedImage->h * formattedImage->w; i++) {
        texture[index][i] = pixels[i];
    }

    SDL_DestroySurface(image);
    SDL_DestroySurface(formattedImage);
}

void init_Textures() {
    load_texture(0, "../textures/bricksx64.bmp");
}

void init_Graphics(SDL_Renderer *renderer) {
    screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    init_Textures();

}

void draw_frame(SDL_Renderer* renderer, Player* player) {

    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            buffer[y][x] = 0xFF333333;
        }
    }

    for (int x = 0; x < WINDOW_WIDTH; x++) {
        double cameraX = 2 * x / (double) WINDOW_WIDTH - 1;
        double rayDirX = player->dirX + player->planeX * cameraX;
        double rayDirY = player->dirY + player->planeY * cameraX;

        int mapX = (int) player->posX;
        int mapY = (int) player->posY;

        double sideDistX;
        double sideDistY;

        double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1/rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1/rayDirY);
        double perpWallDist;

        int stepX;
        int stepY;
        int hit = 0; // did the rays hit a wall?
        int side;    // did it hit the side?

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (player->posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player->posX) * deltaDistX;
        }

        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (player->posY - mapY) * deltaDistY;
        }
        else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - player->posY) * deltaDistY;
        }

        //dda algorithm
        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        //distance projected on camera direction
        //having Euclidean distance instead of camera plane distance would give us a fisheye effect
        if (side == 0) perpWallDist = (sideDistX - deltaDistX);
        else perpWallDist = (sideDistY - deltaDistY);

        //find lowest and highest pixel to fill in
        int lineHeight = (int)(WINDOW_HEIGHT / perpWallDist);
        int drawStart = -lineHeight / 2 + WINDOW_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + WINDOW_HEIGHT / 2;
        if (drawEnd >= WINDOW_HEIGHT) drawEnd = WINDOW_HEIGHT - 1;

        double wallX;
        if (side == 0) wallX = player->posY + perpWallDist * rayDirY;
        else wallX = player->posX + perpWallDist * rayDirX;
        wallX-=floor(wallX);

        int textureX = (int)(wallX * (double) TEXTURE_WIDTH);
        if (side == 0 && rayDirX > 0) textureX = TEXTURE_WIDTH - textureX - 1;
        if (side == 1 && rayDirX < 0) textureX = TEXTURE_WIDTH - textureX - 1;

        double wallStep = 1.0 * TEXTURE_HEIGHT / lineHeight;
        double texturePos = (drawStart - WINDOW_HEIGHT / 2 + lineHeight / 2) * wallStep;

        for (int y = drawStart; y < drawEnd; y++) {
            int textureY = (int)texturePos & (TEXTURE_HEIGHT - 1);
            texturePos += wallStep;

            int textureNum = worldMap[mapX][mapY];
            if (textureNum < 0) textureNum = 0;
            if (textureNum >= NUM_TEXTURES)textureNum = 0;
            Uint32 pixelColor = texture[textureNum][TEXTURE_HEIGHT * textureY + textureX];
            //if (side == 1) pixelColor /= 2;

            buffer[y][x] = pixelColor;
        }


        /*//fill walls in with different colours depending on wall type
        ColorRGB color = {0,0,0};
        int wallType = worldMap[mapX][mapY];
        switch (wallType) {
            case 1:
                color.r = 255; color.g = 0; color.b = 0; break;
            case 2:
                color.r = 0; color.g = 255; color.b = 0; break;
            case 3:
                color.r = 0; color.g = 0; color.b = 255; break;
            case 4:
                color.r = 255; color.g = 255; color.b = 255; break;
            case 5:
                color.r = 0; color.g = 255; color.b = 255; break;
            default: color.r = 0; color.g = 0; color.b = 0; break;
        }

        //make sides slightly darker
        if (side == 1) {
            color.r /= 2; color.g /= 2; color.b /= 2;
        }


        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
        SDL_RenderLine(renderer,x,drawStart,x,drawEnd);
        */
        SDL_UpdateTexture(screenTexture, NULL, buffer, WINDOW_WIDTH * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer,screenTexture,NULL,NULL);
    }
}