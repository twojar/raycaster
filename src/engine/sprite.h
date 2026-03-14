//
// Created by Alan Pitcher on 12/1/2025.
//

#ifndef SPRITE_H
#define SPRITE_H

//  Internal sorting and tracking globals
extern int g_spriteDataExists;
extern int g_numSprites;
extern int *g_spriteOrder;
extern double *g_spriteDistance;

//  Sprite property tags for internal processing
enum {
    TURN_X_POS = 1,
    TURN_Y_POS = 2,
    TURN_TEXTURE = 3,
    TURN_TYPE = 4,
};

//  Classification for sprite rendering and entity logic
typedef enum {
    SPRITE_STATIC = 0,
    SPRITE_ENTITY = 1,
    SPRITE_CONSUMABLE = 2,
} SpriteType;

//  Represents a single sprite in the world view
typedef struct {
    double x;
    double y;
    double prevX;
    double prevY;
    int texture;
    int spriteType;
} Sprite;

extern Sprite *g_sprites;

//  Sorts an array of sprite indices by their distance to the camera (Z-Sorting)
void sprite_sort(int* order, double* distance, int n);

//  Creates a randomized pool of static objects/sprites in the world
void sprite_random(int num);

//  Loads custom sprite metadata from a .SPRITEDATA file
void sprite_load(char *path);

//  Cleans up sprite resources and resets globals
void sprite_free();

#endif //SPRITE_H
