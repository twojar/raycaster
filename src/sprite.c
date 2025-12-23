//
// Created by Alan Pitcher on 12/1/2025.
//
#include "sprite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 1024
#define X_POS_TURN 1
#define Y_POS_TURN 2
#define TEXTURE_TURN 3

int numSprites;
int spriteDataExists = 0;

int *spriteOrder;
double *spriteDistance;

Sprite *sprites;

void sort_sprites(int* order, double* distance, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1 ; j < n; j++) {
            if (distance[i] < distance[j]) {
                double temp_d = distance[i];
                distance[i] = distance[j];
                distance[j] = temp_d;

                int temp_o = order[i];
                order[i] = order[j];
                order[j] = temp_o;
            }
        }
    }
}

void load_sprites(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Could not open file %s\n", path);
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
            if (turn == X_POS_TURN) {
                sprites[i].x = atof(token);
                turn++;
            }
            else if (turn == Y_POS_TURN) {
                sprites[i].y = atof(token);
                turn++;
            }
            else if (turn == TEXTURE_TURN) {
                sprites[i].texture = atoi(token);
                turn = X_POS_TURN;
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

    printf("%d\n", numSprites);
}

void free_sprites() {
    if (sprites != NULL) free(sprites);
    if (spriteDistance != NULL) free(spriteDistance);
    if (spriteOrder != NULL) free(spriteOrder);
    printf("All sprites freed\n");
}
