//
// Created by there on 12/1/2025.
//

#ifndef SPRITE_H
#define SPRITE_H

#define NUM_SPRITES 19

extern int spriteOrder[NUM_SPRITES];
extern double spriteDistance[NUM_SPRITES];


typedef struct{
    double x;
    double y;
    int texture;
} Sprite;

extern Sprite sprite[NUM_SPRITES];

void sort_sprites(int* order, double* distance, int n);

#endif //SPRITE_H
