//
// Created by Alan Pitcher on 11/22/2025.
//
#include "graphics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "sprite.h"
#include "maptile.h"

#define FOG_COLOUR 0xFF000000
#define FOG_DENSITY 0.8
#define MAX_FOG_DIST 64
#define FOG_TABLE_SIZE 2048

float fogTable[FOG_TABLE_SIZE];
const float CAM_Z = 0.5;

//  precomputed lookup table for fog calculations
void load_fogTable() {
    for (int i = 0; i < FOG_TABLE_SIZE; i++) {
        double dist = (double) i * (MAX_FOG_DIST / (double) FOG_TABLE_SIZE);
        fogTable[i] = 1.0 / exp(dist * FOG_DENSITY);
    }
}

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

//  Initializes all textures
void init_Textures() {
    load_texture(1, "../assets/textures/bricksx64.png");
    load_texture(2, "../assets/textures/ConcreteFloor-02_64.png");
    load_texture(3, "../assets/textures/Panel-001-2_Base-002.png");

    //  sprite textures not included in repo due to copyright
    //  Place your own sprite textures in /textures
    load_texture(4, "../assets/textures/greenlight.png");
    load_texture(5, "../assets/textures/barrel.png");
    load_texture(6, "../assets/textures/pillar.png");
    load_texture(7, "../assets/textures/angel.png");

}

//  Initializes gfx
void init_Graphics(SDL_Renderer *renderer) {
    screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    load_fogTable();
    init_Textures();
}

//  Fog density + Colour calculation for every pixel on screen
Uint32 apply_fog(Uint32 pixel, double distance) {

    int i = (int) (distance * (FOG_TABLE_SIZE / MAX_FOG_DIST));
    if (i < 0) i = 0;
    if (i >= FOG_TABLE_SIZE) i = FOG_TABLE_SIZE - 1;
    double fogFactor = fogTable[i];

    if (fogFactor > 1.0) fogFactor = 1.0;
    if (fogFactor < 0.0) fogFactor = 0.0;

    Uint8 fogR = (FOG_COLOUR >> 16) & 0xFF;
    Uint8 fogG = (FOG_COLOUR >> 8) & 0xFF;
    Uint8 fogB = (FOG_COLOUR) & 0xFF;

    Uint8 pixelR = (pixel >> 16) & 0xFF;
    Uint8 pixelG = (pixel >> 8) & 0xFF;
    Uint8 pixelB = (pixel >> 0) & 0xFF;

    Uint8 r =  (Uint8) (pixelR * fogFactor + fogR * (1.0 - fogFactor));
    Uint8 g =  (Uint8) (pixelG * fogFactor + fogG * (1.0 - fogFactor));
    Uint8 b =  (Uint8) (pixelB * fogFactor + fogB * (1.0 - fogFactor));

    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}


//  DDA algorithm for wall casting
double dda(double startX, double startY, double rayDirX, double rayDirY, int *refSide, int *refMapX, int *refMapY) {
    int mapX = (int) startX;
    int mapY = (int) startY;

    double sideDistX;
    double sideDistY;

    double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1/rayDirX);
    double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1/rayDirY);

    int stepX;
    int stepY;
    int hit = 0; // did the rays hit a wall?
    int side;    // did it hit the side?

    if (rayDirX < 0) {
        stepX = -1;
        sideDistX = (startX - mapX) * deltaDistX;
    } else {
        stepX = 1;
        sideDistX = (mapX + 1.0 - startX) * deltaDistX;
    }

    if (rayDirY < 0) {
        stepY = -1;
        sideDistY = (startY - mapY) * deltaDistY;
    }
    else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - startY) * deltaDistY;
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
        if (mapX < 0 || mapX >= mapCols || mapY < 0 || mapY >= mapRows) return 1e30;
        if (worldMap[mapY * mapCols + mapX].textureID > 0) hit = 1;
    }

    if (refSide > 0) *refSide = side;
    if (refMapX > 0) *refMapX = mapX;
    if (refMapY > 0) *refMapY = mapY;

    if (side == 0) return (sideDistX - deltaDistX);
    else return (sideDistY - deltaDistY);
}

//  Handles all rendering
void draw_frame(SDL_Renderer* renderer, Player* player) {

    //  FLOORCASTING
    for (int y = WINDOW_HEIGHT / 2 + 1; y < WINDOW_HEIGHT; y++) {
        float rayDirX0 = player->dirX - player->planeX;
        float rayDirY0 = player->dirY - player->planeY;
        float rayDirX1 = player->dirX + player->planeX;
        float rayDirY1 = player->dirY + player->planeY;

        int p = y - WINDOW_HEIGHT / 2;
        float posZ = CAM_Z * WINDOW_HEIGHT;

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

            //  floor texture
            Uint32 floorPixelColour = texture[floorTexture][TEXTURE_WIDTH * ty + tx];
            floorPixelColour = (floorPixelColour >> 1) & 8355711;
            floorPixelColour = floorPixelColour | 0xFF000000;

            floorPixelColour = apply_fog(floorPixelColour, rowDistance);
            buffer[y][x] = floorPixelColour;

            //  ceiling texture
            Uint32 ceilingPixelColour = texture[ceilingTexture][TEXTURE_WIDTH * ty + tx];
            ceilingPixelColour = (ceilingPixelColour >> 1) & 8355711;
            ceilingPixelColour = ceilingPixelColour | 0xFF000000;
            ceilingPixelColour = apply_fog(ceilingPixelColour, rowDistance);
            buffer[WINDOW_HEIGHT - y - 1][x] = ceilingPixelColour;
        }

    }


    //  WALLCASTING
    for (int x = 0; x < WINDOW_WIDTH; x++) {
        double cameraX = 2 * x / (double) WINDOW_WIDTH - 1;
        double rayDirX = player->dirX + player->planeX * cameraX;
        double rayDirY = player->dirY + player->planeY * cameraX;
        int side;
        int mapX;
        int mapY;

        double perpWallDist = dda(player->posX, player->posY, rayDirX, rayDirY, &side, &mapX, &mapY);


        //  find lowest and highest pixel to fill in
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
            int textureNum = worldMap[(mapY * mapCols) + mapX].textureID;
            if (textureNum < 0) textureNum = 0;
            if (textureNum >= NUM_TEXTURES)textureNum = 0;


            Uint32 wallPixelColour = texture[textureNum][TEXTURE_HEIGHT * textureY + textureX];
            wallPixelColour = apply_fog(wallPixelColour, perpWallDist);
            buffer[y][x] = wallPixelColour;

            //slightly darken wall sides
            if (side == 1) {
                Uint8 r = ((wallPixelColour >> 16) & 0xFF) * 0.8;
                Uint8 g = ((wallPixelColour >> 8) & 0xFF) * 0.8;
                Uint8 b = (wallPixelColour & 0xFF) * 0.8;
                wallPixelColour = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
        ZBuffer[x] = perpWallDist;
    }

    //  SPRITE CASTING
    //  sort sprites from furthest to closest if sprite data exists
    if (spriteDataExists) {
        for (int i = 0; i < numSprites ; i++) {
            spriteOrder[i] = i;
            spriteDistance[i] = ((player->posX - sprites[i].x) * (player->posX - sprites[i].x) + (player->posY - sprites[i].y) * (player->posY - sprites[i].y));
        }

        sort_sprites(spriteOrder, spriteDistance, numSprites);
        for (int i = 0; i < numSprites; i++) {
            double spriteX = sprites[spriteOrder[i]].x - player->posX;
            double spriteY = sprites[spriteOrder[i]].y - player->posY;

            //  transform sprite with the inverse camera matrix
            //  [ planeX   dirX ] -1                                       [ dirY      -dirX ]
            //  [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
            //  [ planeY   dirY ]                                          [ -planeY  planeX ]

            double invDet = 1.0 / (player->planeX * player->dirY - player->dirX * player->planeY);
            double transformX = invDet * (player->dirY * spriteX - player->dirX * spriteY);
            double transformY = invDet * (-player->planeY * spriteX + player->planeX * spriteY);
            int spriteScreenX = (int) ((WINDOW_WIDTH/2) * (1 + transformX/transformY));

            int spriteHeight = abs((int) (WINDOW_HEIGHT/(transformY)));
            int drawStartY = -spriteHeight / 2 + WINDOW_HEIGHT / 2;
            if (drawStartY < 0) drawStartY = 0;
            int drawEndY = spriteHeight / 2 + WINDOW_HEIGHT / 2;
            if (drawEndY >= WINDOW_HEIGHT) drawEndY = WINDOW_HEIGHT - 1;

            int spriteWidth = abs((int)(WINDOW_HEIGHT/(transformY)));
            int drawStartX = -spriteWidth / 2 + spriteScreenX;
            if (drawStartX < 0) drawStartX = 0;
            int drawEndX = spriteWidth / 2 + spriteScreenX;
            if (drawEndX >= WINDOW_WIDTH) drawEndX = WINDOW_WIDTH - 1;

            for (int stripe = drawStartX; stripe < drawEndX; stripe ++) {
                int texX = (int) (256* (stripe - (-spriteWidth / 2 + spriteScreenX)) * TEXTURE_WIDTH / spriteWidth) / 256;
                if (transformY > 0 && stripe > 0 && stripe < WINDOW_WIDTH && transformY < ZBuffer[stripe]) {
                    for (int y = drawStartY; y < drawEndY; y++) {
                        //  128 and 256 are to avoid floats
                        int d = (y) * 256 - WINDOW_HEIGHT * 128 + spriteHeight * 128;
                        int texY = ((d * TEXTURE_HEIGHT)/spriteHeight) / 256;

                        Uint32 spritePixelColour = texture[sprites[spriteOrder[i]].texture][TEXTURE_HEIGHT * texY + texX];

                        if ((spritePixelColour & 0x00FFFFFF) != 0) {
                            buffer[y][stripe] = apply_fog(spritePixelColour,transformY);
                        }
                    }
                }
            }
        }
    }
    SDL_UpdateTexture(screenTexture, NULL, buffer, WINDOW_WIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer,screenTexture,NULL,NULL);
}

void worldMap_free() {
    if (worldMap != NULL) free(worldMap);
    printf("worldMap freed\n");
}