//
// Created by Alan Pitcher on 1/9/2026.
//
#include "maptile.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stack.h"
#include "player.h"

#define MAX_LINE_LENGTH 1024

int mapRows = 0;
int mapCols = 0;
MapTile *worldMap = NULL;


// generate a random map (maze)
void random_map(Player *player) {
    srand(time(NULL));
    mapRows = (rand() % 12) + 24;
    mapCols = (rand() % 12) + 24;

    if ((mapRows & 1) == 0) mapRows++;
    if ((mapCols & 1) == 0) mapCols++;

    if (worldMap == NULL) {
        worldMap = (MapTile *)malloc(sizeof(MapTile) * mapRows * mapCols);
        if (worldMap == NULL) fprintf(stderr, "Failed to allocate memory for worldMap\n");
    } else {
        worldMap = (MapTile *)realloc(worldMap, sizeof(MapTile) * mapRows * mapCols);
        if (worldMap == NULL) fprintf(stderr, "Failed to reallocate memory for worldMap\n");
    }

    //  Iterative DFS algorithm for pseudo-random maze generation
    //  will probably switch later to a better algorithm for more branching in the maze
    //  maybe i should make my own random number generator as well?
    for (int i = 0; i < mapRows; i++) {
        for (int j = 0; j < mapCols; j++) {
            worldMap[i * mapCols +j].textureID = 1;
            worldMap[i * mapCols +j].posX = j;
            worldMap[i * mapCols +j].posY = i;
        }
    }

    int visited[mapRows][mapCols];
    memset(visited, 0, sizeof(visited));
    Stack cellStack = stack_create(mapRows * mapCols);

    worldMap[1 * mapCols + 1].textureID = 0;
    visited[1][1] = 1;
    stack_push(&cellStack, &worldMap[1 * mapCols + 1]);
    while (!stack_is_empty(&cellStack)) {
        MapTile *cell = stack_pop(&cellStack);
        int dy[4] = {-2, 2, 0, 0};
        int dx[4] = {0, 0, -2, 2};

        int neighX[4];
        int neighY[4];
        int count = 0;

        for (int k = 0; k < 4; k++) {
            int ny = dy[k] + cell->posY;
            int nx = dx[k] + cell->posX;

            // skip if out of bounds or neighbour is already visited
            if (ny <= 0 || ny >= mapRows - 1|| nx <= 0 || nx >= mapCols - 1) continue;
            if (visited[ny][nx] == 1) continue;

            neighX[count] = nx;
            neighY[count] = ny;
            count++;
        }

        if (count > 0) {
            stack_push(&cellStack, cell);
            int randPick = rand() % count;
            int nx = neighX[randPick];
            int ny = neighY[randPick];


            worldMap[cell->posY * mapCols + cell->posX].textureID = 0;
            worldMap[((cell->posY + ny) / 2) * mapCols + ((cell->posX + nx) / 2)].textureID = 0;
            worldMap[ny * mapCols + nx].textureID = 0;

            visited[ny][nx] = 1;
            stack_push(&cellStack, &worldMap[ny * mapCols + nx]);
        }
    }
    printf("Maze generation finished!\n");
    stack_free(&cellStack);

    int spawned = 0;
    for (int y = 0; y < mapRows && !spawned; y++) {
        for (int x = 0; x < mapCols; x++) {
            if (worldMap[y * mapCols + x].textureID == 0) {
                player_teleport(player, (double) x + 0.5, (double) y + 0.5);
                spawned = 1;
                break;
            }
        }
    }
}

void load_map(char* path) {
    char line[MAX_LINE_LENGTH];
    mapRows = 0;
    mapCols = 0;
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file %s\n", path);
        return;
    }

    if (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        mapRows++;
        char *temp = strdup(line);
        char *token = strtok(temp, " \t\r\n");
        while (token != NULL) {
            mapCols++;
            token = strtok(NULL, " \t\r\n");
        }
        free(temp);
    }

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        mapRows++;
    }

    printf("mapCols: %d\nmapRows: %d\n", mapCols, mapRows);
    if (worldMap != NULL) free(worldMap);
    worldMap = (MapTile*)malloc(sizeof(MapTile) * mapCols * mapRows);
    if (worldMap == NULL) fprintf(stderr, "Unable to allocate memory for worldMap\n");
    rewind(fp);
    int y = 0;
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line[strcspn(line, "\r\n")] = 0;
        int x = 0;
        char *token = strtok(line, " \t");
        while (token != NULL) {
            worldMap[y * mapCols + x].textureID = atoi(token);
            worldMap[y * mapCols + x].posX = x;
            worldMap[y * mapCols + x].posY = y;
            x++;
            token = strtok(NULL, " \t");
        }
        y++;
    }
    fclose(fp);
    return;
}


