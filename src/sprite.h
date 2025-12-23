//
// Created by Alan Pitcher on 12/1/2025.
//

#ifndef SPRITE_H
#define SPRITE_H

extern int spriteDataExists;
extern int numSprites;
extern int *spriteOrder;
extern double *spriteDistance;


typedef struct{
    double x;
    double y;
    int texture;
} Sprite;

extern Sprite *sprites;

void sort_sprites(int* order, double* distance, int n);
void load_sprites(char *path);
void free_sprites();

#endif //SPRITE_H
