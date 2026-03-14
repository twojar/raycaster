//
// Created by Alan Pitcher on 12/1/2025.
//
#include "engine/sprite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  buffer length file io
#define MAX_LINE_LENGTH 1024

int g_numSprites;
int g_spriteDataExists = 0;

int *g_spriteOrder;
double *g_spriteDistance;

Sprite *g_sprites;

//  sprite_compare (helper function) for qsort in sprite_sort tells us whether which sprites are furthest away
//  we want to sort sprites furthest to closest
//  if spriteA is closer, spriteB needs to come first so we return 1 (put A after B)
//  if spriteB is closer, spriteA needs to come first so we return -1 (put B after A)
//  else if they're the same distance, return 0
static int sprite_compare(const void *a, const void *b) {
    int indexA = *((int *)a);
    int indexB = *((int *)b);
    if (g_spriteDistance[indexB] > g_spriteDistance[indexA]) return 1;
    if (g_spriteDistance[indexB] < g_spriteDistance[indexA]) return -1;
    return 0;
}
void sprite_sort(int* order, double* distance, int n) {
  qsort(g_spriteOrder, g_numSprites, sizeof(int), sprite_compare);
}

//  randomly generate sprites
void sprite_random(int num) {
    g_numSprites = num; // Create specified amount of default entity sprites
    
    if (g_sprites != NULL) free(g_sprites);
    g_sprites = malloc(sizeof(Sprite) * g_numSprites);
    
    for (int i = 0; i < g_numSprites; i++) {
        g_sprites[i].spriteType = SPRITE_ENTITY;
        g_sprites[i].texture = 7; // Default texture ID for entities
        g_sprites[i].x = 0; // Will be randomized by entity system
        g_sprites[i].y = 0;
    }

    if (g_spriteDistance != NULL) free(g_spriteDistance);
    g_spriteDistance = malloc(sizeof(double) * g_numSprites);

    if (g_spriteOrder != NULL) free(g_spriteOrder);
    g_spriteOrder = malloc(sizeof(int) * g_numSprites);
    
    for (int i = 0; i < g_numSprites; i++) {
        g_spriteOrder[i] = i;
        g_spriteDistance[i] = 0.0;
    }

    printf("Random sprites generated: %d\n", g_numSprites);
}

//  loads sprite from a path to a *.SPRITEDATA file into the global sprites array
void sprite_load(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr,"Could not open file %s\n", path);
        return;
    }
    char line[MAX_LINE_LENGTH];
    g_numSprites = 0;
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        g_numSprites++;
    }
    rewind(fp);

    if (g_sprites != NULL) free(g_sprites);
    g_sprites = malloc(sizeof(Sprite) * g_numSprites);
    if (g_sprites == NULL) fprintf(stderr, "Could not allocate memory for sprites\n");

    int turn = 1;
    for (int i = 0; i < g_numSprites; i++) {
        if (fgets(line, MAX_LINE_LENGTH, fp) == NULL) break;
        line[strcspn(line, "\r\n")] = 0;
        char *token = strtok(line, " \t");
        while (token != NULL) {
            if (turn == TURN_X_POS) {
                g_sprites[i].x = atof(token);
                turn++;
            }
            else if (turn == TURN_Y_POS) {
                g_sprites[i].y = atof(token);
                turn++;
            }
            else if (turn == TURN_TEXTURE) {
                g_sprites[i].texture = atoi(token);
                turn++;
            }
            else if (turn == TURN_TYPE) {
                g_sprites[i].spriteType = atoi(token);
                turn = TURN_X_POS;
            }

            token = strtok(NULL, " \t");
        }
    }
    fclose(fp);

    if (g_spriteDistance != NULL) free(g_spriteDistance);
    g_spriteDistance = malloc(sizeof(double) * g_numSprites);
    if (g_spriteDistance == NULL) fprintf(stderr, "Could not allocate memory for spriteDistance\n");

    if (g_spriteOrder != NULL) free(g_spriteOrder);
    g_spriteOrder = malloc(sizeof(int) * g_numSprites);
    if (g_spriteOrder == NULL) fprintf(stderr, "Could not allocate memory for spriteOrder\n");

    printf("Sprites loaded: %d\n", g_numSprites);
    g_spriteDataExists = 1;
}

void sprite_free() {
    if (g_sprites != NULL) free(g_sprites);
    if (g_spriteDistance != NULL) free(g_spriteDistance);
    if (g_spriteOrder != NULL) free(g_spriteOrder);
    printf("All sprites freed\n");
}
