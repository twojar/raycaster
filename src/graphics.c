//
// Created by Alan Pitcher on 11/22/2025.
//
#include "graphics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "sprite.h"

//the higher the darker
#define SHADE_FACTOR 2

// the lower the darker
#define SHADE_LIMIT 0.02

#define MAX_LINE_LENGTH 1024

int *worldMap = NULL;
int mapCols = 0;
int mapRows = 0;

Uint32 buffer[WINDOW_HEIGHT][WINDOW_WIDTH];
Uint32 texture[NUM_TEXTURES][TEXTURE_WIDTH * TEXTURE_HEIGHT];
double ZBuffer[WINDOW_WIDTH];

SDL_Texture* screenTexture;
void load_texture(int index,char* path) {
    SDL_Surface* image = IMG_Load(path);
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

//TO IMPLEMENT
void load_map(char* path) {
    char line[MAX_LINE_LENGTH];
    mapRows = 0;
    mapCols = 0;
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file %s\n", path);
        return;
    }

    if (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        mapRows++;
        char *temp = strdup(line);
        char *token = strtok(temp, " \t\r\n");
        while (token != NULL) {
            mapCols++;
            token = strtok(NULL, " \t\r\n");
        }
        free(temp);
    }

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        mapRows++;
    }

    printf("mapCols: %d\nmapRows: %d\n", mapCols, mapRows);
    if (worldMap != NULL) free(worldMap);
    worldMap = (int*)malloc(sizeof(int) * mapCols * mapRows);
    if (worldMap == NULL) fprintf(stderr, "Unable to allocate memory for worldMap\n");
    rewind(fp);
    int y = 0;
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line[strcspn(line, "\r\n")] = 0;
        int x = 0;
        char *token = strtok(line, " \t");
        while (token != NULL) {
            worldMap[y * mapCols + x] = atoi(token);
            x++;
            token = strtok(NULL, " \t");
        }
        y++;
    }
    fclose(fp);
    return;
}

void init_Textures() {
    load_texture(1, "../assets/textures/bricksx64.png");
    load_texture(2, "../assets/textures/ConcreteFloor-02_64.png");
    load_texture(3, "../assets/textures/Panel-001-2_Base-002.png");

    //Textures not included in repo due to copyright
    //Place your own sprite textures in /textures
    load_texture(4, "../assets/textures/greenlight.png");
    load_texture(5, "../assets/textures/barrel.png");
    load_texture(6, "../assets/textures/pillar.png");
}

void init_Graphics(SDL_Renderer *renderer) {
    screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    init_Textures();
}

void draw_frame(SDL_Renderer* renderer, Player* player) {

    //FLOORCASTING
    for (int y = WINDOW_HEIGHT / 2 + 1; y < WINDOW_HEIGHT; y++) {
        float rayDirX0 = player->dirX - player->planeX;
        float rayDirY0 = player->dirY - player->planeY;
        float rayDirX1 = player->dirX + player->planeX;
        float rayDirY1 = player->dirY + player->planeY;

        int p = y - WINDOW_HEIGHT / 2;
        float posZ = 0.5 * WINDOW_HEIGHT;

        float rowDistance = posZ / p;

        float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / WINDOW_WIDTH;
        float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / WINDOW_WIDTH;
        float floorX = player->posX + rowDistance * rayDirX0;
        float floorY = player->posY + rowDistance * rayDirY0;

        for (int x = 0; x < WINDOW_WIDTH; x++) {
            int cellX = (int) (floorX);
            int cellY = (int) (floorY);

            int tx = (int) (TEXTURE_WIDTH * (floorX - cellX)) & (TEXTURE_WIDTH - 1);
            int ty = (int) (TEXTURE_WIDTH * (floorY - cellY)) & (TEXTURE_HEIGHT - 1);

            floorX += floorStepX;
            floorY += floorStepY;

            int floorTexture = 2;
            int ceilingTexture = 3;


            double floorShade = 1.0/(1.0 + rowDistance * SHADE_FACTOR);
            if (floorShade < SHADE_LIMIT) floorShade = SHADE_LIMIT;


            //floor texture
            Uint32 colour = texture[floorTexture][TEXTURE_WIDTH * ty + tx];
            colour = (colour >> 1) & 8355711;
            colour = colour | 0xFF000000;

            ColorRGB floorPxColour = {(((colour >> 16)&0xFF)*floorShade), (((colour >> 8)&0xFF)*floorShade), (((colour )&0xFF)*floorShade)};
            colour = 0xFF000000 | (floorPxColour.r << 16) | (floorPxColour.g << 8) | (floorPxColour.b);
            buffer[y][x] = colour;

            //ceiling texture
            colour = texture[ceilingTexture][TEXTURE_WIDTH * ty + tx];
            colour = (colour >> 1) & 8355711;
            colour = colour | 0xFF000000;
            ColorRGB ceilPxColour = {(((colour >> 16)&0xFF)*floorShade), (((colour >> 8)&0xFF)*floorShade), (((colour )&0xFF)*floorShade)};
            colour = 0xFF000000 | (ceilPxColour.r << 16) | (ceilPxColour.g << 8) | (ceilPxColour.b);
            buffer[WINDOW_HEIGHT - y - 1][x] = colour;
        }

    }


    //WALLCASTING
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
            if (worldMap[mapY * mapCols + mapX] > 0) hit = 1;
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
        if (side == 1 && rayDirY < 0) textureX = TEXTURE_WIDTH - textureX - 1;

        double wallStep = 1.0 * TEXTURE_HEIGHT / lineHeight;
        double texturePos = (drawStart - WINDOW_HEIGHT / 2 + lineHeight / 2) * wallStep;

        for (int y = drawStart; y < drawEnd; y++) {
            int textureY = (int)texturePos & (TEXTURE_HEIGHT - 1);
            texturePos += wallStep;
            int textureNum = worldMap[(mapY * mapCols) + mapX];
            if (textureNum < 0) textureNum = 0;
            if (textureNum >= NUM_TEXTURES)textureNum = 0;


            //shade pixels based off distance to walls and rebuild
            double distShade = 1.0 / (1.0 + perpWallDist * SHADE_FACTOR);
            if (distShade < SHADE_LIMIT) distShade = SHADE_LIMIT;

            Uint32 pixelColour = texture[textureNum][TEXTURE_HEIGHT * textureY + textureX];
            ColorRGB wColour = {0,0,0};
            wColour.r = (Uint8) (((pixelColour >> 16) & 0xFF) * distShade);
            wColour.g = (Uint8) (((pixelColour >> 8) & 0xFF) * distShade);
            wColour.b = (Uint8) (((pixelColour >> 0) & 0xFF)* distShade);
            pixelColour = (0xFF << 24) | (wColour.r << 16) | (wColour.g << 8) | wColour.b;

            if (side == 1) {
                pixelColour = (pixelColour >> 1) & 8355711;
                pixelColour = pixelColour | 0xFF000000;
            }

            buffer[y][x] = pixelColour;
        }
        ZBuffer[x] = perpWallDist;
    }

    //SPRITE CASTING
    //sort sprites from furthest to closest if sprite data exists
    if (spriteDataExists) {
        for (int i = 0; i < numSprites ; i++) {
            spriteOrder[i] = i;
            spriteDistance[i] = ((player->posX - sprites[i].x) * (player->posX - sprites[i].x) + (player->posY - sprites[i].y) * (player->posY - sprites[i].y));
        }

        sort_sprites(spriteOrder, spriteDistance, numSprites);
        for (int i = 0; i < numSprites; i++) {
            double spriteX = sprites[spriteOrder[i]].x - player->posX;
            double spriteY = sprites[spriteOrder[i]].y - player->posY;

            //transform sprite with the inverse camera matrix
            // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
            // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
            // [ planeY   dirY ]                                          [ -planeY  planeX ]
            double invDet = 1.0 / (player->planeX * player->dirY - player->dirX * player->planeY);
            double transformX = invDet * (player->dirY * spriteX - player->dirX * spriteY);
            double transformY = invDet * (-player->planeY * spriteX + player->planeX * spriteY);
            int spriteScreenX = (int) ((WINDOW_WIDTH/2) * (1 + transformX/transformY));

            int spriteHeight = abs((int) (WINDOW_HEIGHT/(transformY)));
            int drawStartY = -spriteHeight / 2 + WINDOW_HEIGHT / 2;
            if (drawStartY < 0) drawStartY = 0;
            int drawEndY = spriteHeight / 2 + WINDOW_HEIGHT / 2;
            if (drawEndY >= WINDOW_HEIGHT) drawEndY = WINDOW_HEIGHT - 1;

            int spriteWidth = abs( (int) (WINDOW_HEIGHT/(transformY)));
            int drawStartX = -spriteWidth / 2 + spriteScreenX;
            if (drawStartX < 0) drawStartX = 0;
            int drawEndX = spriteWidth / 2 + spriteScreenX;
            if (drawEndX >= WINDOW_WIDTH) drawEndX = WINDOW_WIDTH - 1;

            for (int stripe = drawStartX; stripe < drawEndX; stripe ++) {
                int texX = (int) (256* (stripe - (-spriteWidth / 2 + spriteScreenX)) * TEXTURE_WIDTH / spriteWidth) / 256;
                if (transformY > 0 && stripe > 0 && stripe < WINDOW_WIDTH && transformY < ZBuffer[stripe]) {
                    for (int y = drawStartY; y < drawEndY; y++) {

                        //128 and 256 are to avoid floats
                        int d = (y) * 256 - WINDOW_HEIGHT * 128 + spriteHeight * 128;
                        int texY = ((d * TEXTURE_HEIGHT)/spriteHeight) / 256;

                        //shade pixels based off distance to walls and rebuild
                        double spriteDistShade = 1.0 / (1.0 + transformY * SHADE_FACTOR);
                        if (spriteDistShade < SHADE_LIMIT) spriteDistShade = SHADE_LIMIT;
                        Uint32 spritePixelColour = texture[sprites[spriteOrder[i]].texture][TEXTURE_HEIGHT * texY + texX];
                        ColorRGB sColour = {0,0,0};
                        sColour.r = (Uint8) (((spritePixelColour >> 16) & 0xFF) * spriteDistShade);
                        sColour.g = (Uint8) (((spritePixelColour >> 8) & 0xFF) * spriteDistShade);
                        sColour.b = (Uint8) (((spritePixelColour >> 0) & 0xFF)* spriteDistShade);
                        spritePixelColour = (0xFF << 24) | (sColour.r << 16) | (sColour.g << 8) | sColour.b;
                        if ((spritePixelColour & 0x00FFFFFF) != 0) buffer[y][stripe] = spritePixelColour;
                    }
                }
            }
        }
    }
    SDL_UpdateTexture(screenTexture, NULL, buffer, WINDOW_WIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer,screenTexture,NULL,NULL);
}
