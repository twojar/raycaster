//
// Created by Alan Pitcher on 12/1/2025.
//

#ifndef SPRITE_H
#define SPRITE_H
#define NUM_SPRITES 19

extern int spriteDataExists;
extern int numSprites;
extern int spriteOrder[NUM_SPRITES];
extern double spriteDistance[NUM_SPRITES];


typedef struct{
    double x;
    double y;
    int texture;
} Sprite;

extern Sprite *sprites;

void sort_sprites(int* order, double* distance, int n);
void load_sprites(char *path);

#endif //SPRITE_H
