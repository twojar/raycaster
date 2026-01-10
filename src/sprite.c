//
// Created by Alan Pitcher on 12/1/2025.
//
#include "sprite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int numSprites;
int spriteDataExists = 0;

int *spriteOrder;
double *spriteDistance;

Sprite *sprites;

//  compare_sprites (helper function) for qsort in sort_sprites tells us whether which sprites are furthest away
//  we want to sort sprites furthest to closest
//  if spriteA is closer, spriteB needs to come first so we return 1 (put A after B)
//  if spriteB is closer, spriteA needs to come first so we return -1 (put B after A)
//  else if they're the same distance, return 0
int compare_sprites(const void *a, const void *b) {
    int indexA = *((int *)a);
    int indexB = *((int *)b);
    if (spriteDistance[indexB] > spriteDistance[indexA]) return 1;
    if (spriteDistance[indexB] < spriteDistance[indexA]) return -1;
    return 0;
}
void sort_sprites(int* order, double* distance, int n) {
  qsort(spriteOrder, numSprites, sizeof(int), compare_sprites);
}

// loads sprite from a path to a *.SPRITEDATA file into the sprites array
void load_sprites(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr,"Could not open file %s\n", path);
        return;
    }
    char line[MAX_LINE_LENGTH];
    numSprites = 0;
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        numSprites++;
    }
    rewind(fp);

    if (sprites != NULL) free(sprites);
    sprites = malloc(sizeof(Sprite) * numSprites);
    if (sprites == NULL) fprintf(stderr, "Could not allocate memory for sprites\n");

    int turn = 1;
    for (int i = 0; i < numSprites; i++) {
        if (fgets(line, MAX_LINE_LENGTH, fp) == NULL) break;
        line[strcspn(line, "\r\n")] = 0;
        char *token = strtok(line, " \t");
        while (token != NULL) {
            if (turn == TURN_X_POS) {
                sprites[i].x = atof(token);
                turn++;
            }
            else if (turn == TURN_Y_POS) {
                sprites[i].y = atof(token);
                turn++;
            }
            else if (turn == TURN_TEXTURE) {
                sprites[i].texture = atoi(token);
                turn++;
            }
            else if (turn == TURN_TYPE) {
                sprites[i].spriteType = atoi(token);
                turn = TURN_X_POS;
            }

            token = strtok(NULL, " \t");
        }
    }
    fclose(fp);

    if (spriteDistance != NULL) free(spriteDistance);
    spriteDistance = malloc(sizeof(double) * numSprites);
    if (spriteDistance == NULL) fprintf(stderr, "Could not allocate memory for spriteDistance\n");

    if (spriteOrder != NULL) free(spriteOrder);
    spriteOrder = malloc(sizeof(int) * numSprites);
    if (spriteOrder == NULL) fprintf(stderr, "Could not allocate memory for spriteOrder\n");

    printf("Sprites loaded: %d\n", numSprites);
}

void sprites_free() {
    if (sprites != NULL) free(sprites);
    if (spriteDistance != NULL) free(spriteDistance);
    if (spriteOrder != NULL) free(spriteOrder);
    printf("All sprites freed\n");
}
