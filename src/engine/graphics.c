//
// Created by Alan Pitcher on 11/22/2025.
//
#include "engine/graphics.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine/sprite.h"
#include "game/map.h"

#define FOG_COLOUR 0xFF000000
#define FOG_DENSITY 0.8
#define MAX_FOG_DIST 64
#define FOG_TABLE_SIZE 2048

float g_fogTable[FOG_TABLE_SIZE];
const float CAM_Z = 0.5;

//  precomputed lookup table for fog calculations
void gfx_load_fog_table() {
    for (int i = 0; i < FOG_TABLE_SIZE; i++) {
        double dist = (double) i * (MAX_FOG_DIST / (double) FOG_TABLE_SIZE);
        g_fogTable[i] = 1.0 / exp(dist * FOG_DENSITY);
    }
}

Uint32 g_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
Uint32 g_texture[NUM_TEXTURES][TEXTURE_WIDTH * TEXTURE_HEIGHT];
Uint32 g_fontTexture[512 * 512]; // Even larger buffer for 512x512 font map
double g_zBuffer[SCREEN_WIDTH];

SDL_Texture* g_screenTexture;

// Helper to load the font map specifically
void gfx_load_font(char* path) {
    SDL_Surface* image = IMG_Load(path);
    if (image == NULL) {
        printf("Unable to load font image: %s\n%s\n", path, SDL_GetError());
        return;
    }

    SDL_Surface* formattedImage = SDL_ConvertSurface(image, SDL_PIXELFORMAT_ARGB8888);
    if (formattedImage == NULL) {
        printf("Unable to convert font image: %s\n", path);
        SDL_DestroySurface(image);
        return;
    }

    // Safely copy pixels row by row using the surface pitch
    Uint8* srcPixels = (Uint8*)formattedImage->pixels;
    for (int y = 0; y < 512; y++) {
        Uint32* row = (Uint32*)(srcPixels + y * formattedImage->pitch);
        for (int x = 0; x < 512; x++) {
            g_fontTexture[y * 512 + x] = row[x];
        }
    }
    printf("Font map loaded successfully (512x512): %s\n", path);

    SDL_DestroySurface(image);
    SDL_DestroySurface(formattedImage);
}

void gfx_load_texture(int index,char* path) {
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
        g_texture[index][i] = pixels[i];
    }

    SDL_DestroySurface(image);
    SDL_DestroySurface(formattedImage);
}

//  Initializes all textures
void gfx_init_textures() {
    // wall, ceiling, floor textures
    gfx_load_texture(1, "../assets/textures/brick_wall.png");
    gfx_load_texture(2, "../assets/textures/concrete_floor.png");
    gfx_load_texture(3, "../assets/textures/panel_ceiling.png");

    //  sprite textures not included in repo due to copyright
    //  Place your own sprite textures in /textures

    //  static sprites
    gfx_load_texture(4, "../assets/textures/greenlight.png");
    gfx_load_texture(5, "../assets/textures/barrel.png");
    gfx_load_texture(6, "../assets/textures/pillar.png");

    //  entity sprites
    gfx_load_texture(7, "../assets/textures/angel.png");

    //  font map
    gfx_load_font("../assets/textures/font.png");
}

//  Initializes gfx
void gfx_init(SDL_Renderer *renderer) {
    // smaller texture for CPU buffer and set sharp scaling
    g_screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetTextureScaleMode(g_screenTexture, SDL_SCALEMODE_NEAREST);
    
    gfx_load_fog_table();
    gfx_init_textures();
}

//  Fog density + Colour calculation for every pixel on screen
Uint32 gfx_apply_fog(Uint32 pixel, double distance) {

    int i = (int) (distance * (FOG_TABLE_SIZE / MAX_FOG_DIST));
    if (i < 0) i = 0;
    if (i >= FOG_TABLE_SIZE) i = FOG_TABLE_SIZE - 1;
    double fogFactor = g_fogTable[i];

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
double gfx_dda(double startX, double startY, double rayDirX, double rayDirY, int *refSide, int *refMapX, int *refMapY) {
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
        if (mapX < 0 || mapX >= g_mapCols || mapY < 0 || mapY >= g_mapRows) return 1e30;
        if (g_worldMap[mapY * g_mapCols + mapX].textureId > 0) hit = 1;
    }

    if (refSide > 0) *refSide = side;
    if (refMapX > 0) *refMapX = mapX;
    if (refMapY > 0) *refMapY = mapY;

    if (side == 0) return (sideDistX - deltaDistX);
    else return (sideDistY - deltaDistY);
}

//  Handles all rendering
void gfx_draw_frame(SDL_Renderer* renderer, Player* player, double alpha) {

    //  quick hack 3: pre-calc (slang for calculate) plane constants
    float planeX2 = 2.0f * (float)player->planeX;
    float planeY2 = 2.0f * (float)player->planeY;

    //  FLOORCASTING
    for (int y = SCREEN_HEIGHT / 2 + 1; y < SCREEN_HEIGHT; y++) {
        float rayDirX0 = (float)player->dirX - (float)player->planeX;
        float rayDirY0 = (float)player->dirY - (float)player->planeY;

        int p = y - SCREEN_HEIGHT / 2;
        float posZ = CAM_Z * SCREEN_HEIGHT;

        float rowDistance = posZ / p;

        float floorStepX = rowDistance * planeX2 / SCREEN_WIDTH;
        float floorStepY = rowDistance * planeY2 / SCREEN_WIDTH;
        float floorX = (float)player->posX + rowDistance * rayDirX0;
        float floorY = (float)player->posY + rowDistance * rayDirY0;

        for (int x = 0; x < SCREEN_WIDTH; x++) {
            int cellX = (int) (floorX);
            int cellY = (int) (floorY);

            int tx = (int) (TEXTURE_WIDTH * (floorX - cellX)) & (TEXTURE_WIDTH - 1);
            int ty = (int) (TEXTURE_WIDTH * (floorY - cellY)) & (TEXTURE_HEIGHT - 1);

            floorX += floorStepX;
            floorY += floorStepY;

            int floorTexture = 2;
            int ceilingTexture = 3;

            //  floor texture
            Uint32 floorPixelColour = g_texture[floorTexture][TEXTURE_WIDTH * ty + tx];
            floorPixelColour = (floorPixelColour >> 1) & 8355711;
            floorPixelColour = floorPixelColour | 0xFF000000;

            floorPixelColour = gfx_apply_fog(floorPixelColour, rowDistance);
            g_buffer[y][x] = floorPixelColour;

            //  ceiling texture
            Uint32 ceilingPixelColour = g_texture[ceilingTexture][TEXTURE_WIDTH * ty + tx];
            ceilingPixelColour = (ceilingPixelColour >> 1) & 8355711;
            ceilingPixelColour = ceilingPixelColour | 0xFF000000;
            ceilingPixelColour = gfx_apply_fog(ceilingPixelColour, rowDistance);
            g_buffer[SCREEN_HEIGHT - y - 1][x] = ceilingPixelColour;
        }

    }


    //  WALLCASTING
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        double cameraX = 2 * x / (double) SCREEN_WIDTH - 1;
        double rayDirX = player->dirX + player->planeX * cameraX;
        double rayDirY = player->dirY + player->planeY * cameraX;
        int side;
        int mapX;
        int mapY;

        double perpWallDist = gfx_dda(player->posX, player->posY, rayDirX, rayDirY, &side, &mapX, &mapY);


        //  find lowest and highest pixel to fill in
        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        double wallX;
        if (side == 0) wallX = player->posY + perpWallDist * rayDirY;
        else wallX = player->posX + perpWallDist * rayDirX;
        
        //  quick hack 2: floor() replaced by int cast
        wallX -= (int)wallX;

        int textureX = (int)(wallX * (double) TEXTURE_WIDTH);
        if (side == 0 && rayDirX > 0) textureX = TEXTURE_WIDTH - textureX - 1;
        if (side == 1 && rayDirY < 0) textureX = TEXTURE_WIDTH - textureX - 1;

        double wallStep = 1.0 * TEXTURE_HEIGHT / lineHeight;
        double texturePos = (drawStart - SCREEN_HEIGHT / 2 + lineHeight / 2) * wallStep;

        //  quick hack 1: move texture lookup outside vertical loop
        int textureNum = g_worldMap[(mapY * g_mapCols) + mapX].textureId;
        if (textureNum < 0 || textureNum >= NUM_TEXTURES) textureNum = 0;

        for (int y = drawStart; y < drawEnd; y++) {
            int textureY = (int)texturePos & (TEXTURE_HEIGHT - 1);
            texturePos += wallStep;

            Uint32 wallPixelColour = g_texture[textureNum][TEXTURE_HEIGHT * textureY + textureX];
            wallPixelColour = gfx_apply_fog(wallPixelColour, perpWallDist);
            g_buffer[y][x] = wallPixelColour;

            //slightly darken wall sides
            if (side == 1) {
                Uint8 r = ((wallPixelColour >> 16) & 0xFF) * 0.8;
                Uint8 g = ((wallPixelColour >> 8) & 0xFF) * 0.8;
                Uint8 b = (wallPixelColour & 0xFF) * 0.8;
                wallPixelColour = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
        g_zBuffer[x] = perpWallDist;
    }

    //  SPRITE CASTING
    //  sort sprites from furthest to closest if sprite data exists
    if (g_spriteDataExists) {
        for (int i = 0; i < g_numSprites ; i++) {
            g_spriteOrder[i] = i;
            // Interpolate sprite position
            double sx = g_sprites[i].x * alpha + g_sprites[i].prevX * (1.0 - alpha);
            double sy = g_sprites[i].y * alpha + g_sprites[i].prevY * (1.0 - alpha);
            g_spriteDistance[i] = ((player->posX - sx) * (player->posX - sx) + (player->posY - sy) * (player->posY - sy));
        }

        sprite_sort(g_spriteOrder, g_spriteDistance, g_numSprites);
        for (int i = 0; i < g_numSprites; i++) {
            // Interpolate sprite position for rendering
            double sx = g_sprites[g_spriteOrder[i]].x * alpha + g_sprites[g_spriteOrder[i]].prevX * (1.0 - alpha);
            double sy = g_sprites[g_spriteOrder[i]].y * alpha + g_sprites[g_spriteOrder[i]].prevY * (1.0 - alpha);

            double spriteX = sx - player->posX;
            double spriteY = sy - player->posY;

            //  transform sprite with the inverse camera matrix
            double invDet = 1.0 / (player->planeX * player->dirY - player->dirX * player->planeY);
            double transformX = invDet * (player->dirY * spriteX - player->dirX * spriteY);
            double transformY = invDet * (-player->planeY * spriteX + player->planeX * spriteY);
            int spriteScreenX = (int) ((SCREEN_WIDTH/2) * (1 + transformX/transformY));

            int spriteHeight = abs((int) (SCREEN_HEIGHT/(transformY)));
            int drawStartY = -spriteHeight / 2 + SCREEN_HEIGHT / 2;
            if (drawStartY < 0) drawStartY = 0;
            int drawEndY = spriteHeight / 2 + SCREEN_HEIGHT / 2;
            if (drawEndY >= SCREEN_HEIGHT) drawEndY = SCREEN_HEIGHT - 1;

            int spriteWidth = abs((int)(SCREEN_HEIGHT/(transformY)));
            int drawStartX = -spriteWidth / 2 + spriteScreenX;
            if (drawStartX < 0) drawStartX = 0;
            int drawEndX = spriteWidth / 2 + spriteScreenX;
            if (drawEndX >= SCREEN_WIDTH) drawEndX = SCREEN_WIDTH - 1;

            for (int stripe = drawStartX; stripe < drawEndX; stripe ++) {
                int texX = (int) (256* (stripe - (-spriteWidth / 2 + spriteScreenX)) * TEXTURE_WIDTH / spriteWidth) / 256;
                if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH && transformY < g_zBuffer[stripe]) {
                    for (int y = drawStartY; y < drawEndY; y++) {
                        //  128 and 256 are to avoid floats
                        int d = (y) * 256 - SCREEN_HEIGHT * 128 + spriteHeight * 128;
                        int texY = ((d * TEXTURE_HEIGHT)/spriteHeight) / 256;

                        Uint32 spritePixelColour = g_texture[g_sprites[g_spriteOrder[i]].texture][TEXTURE_HEIGHT * texY + texX];

                        if ((spritePixelColour & 0x00FFFFFF) != 0) {
                            g_buffer[y][stripe] = gfx_apply_fog(spritePixelColour,transformY);
                        }
                    }
                }
            }
        }
    }
}

//  Finalizes the frame by uploading the CPU buffer to the GPU texture and rendering it
void gfx_present(SDL_Renderer* renderer) {
    SDL_UpdateTexture(g_screenTexture, NULL, g_buffer, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, g_screenTexture, NULL, NULL);
}

//  Renders a string of text to the screen buffer using the 8x8 font map
void gfx_draw_text(const char* text, int x, int y, Uint32 color) {
    if (text == NULL) return;

    int currentX = x;
    int currentY = y;

    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char c = (unsigned char)text[i];
        
        // Handle newlines
        if (c == '\n') {
            currentX = x;
            currentY += 32;
            continue;
        }

        // Calculate glyph position in 512x512 font map (16x16 grid of 32x32)
        int glyphCol = c % 16;
        int glyphRow = c / 16;
        int glyphX = glyphCol * 32;
        int glyphY = glyphRow * 32;

        // Draw the 32x32 glyph
        for (int gy = 0; gy < 32; gy++) {
            for (int gx = 0; gx < 32; gx++) {
                int screenX = currentX + gx;
                int screenY = currentY + gy;

                // Bounds check
                if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT) {
                    // Sample font texture
                    Uint32 texPixel = g_fontTexture[(glyphY + gy) * 512 + (glyphX + gx)];
                    
                    // Check alpha channel (0xFF in high byte for ARGB)
                    if ((texPixel >> 24) > 128) {
                        g_buffer[screenY][screenX] = color;
                    }
                }
            }
        }

        currentX += 24; // Adjusted spacing for large serif font map
    }
}