//
// Created by Alan Pitcher on 12/1/2025.
//

#ifndef SPRITE_H
#define SPRITE_H

extern int g_spriteDataExists;
extern int g_numSprites;
extern int *g_spriteOrder;
extern double *g_spriteDistance;

enum {
    TURN_X_POS = 1,
    TURN_Y_POS = 2,
    TURN_TEXTURE = 3,
    TURN_TYPE = 4,
};

typedef enum {
    SPRITE_STATIC = 0,
    SPRITE_ENTITY = 1,
    SPRITE_CONSUMABLE = 2,
} SpriteType;

typedef struct {
    double x;
    double y;
    int texture;
    int spriteType;
} Sprite;

extern Sprite *g_sprites;

void sprite_sort(int* order, double* distance, int n);

void sprite_random();
void sprite_load(char *path);
void sprite_free();

#endif //SPRITE_H
