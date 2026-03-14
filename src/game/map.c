//
// Created by Alan Pitcher on 1/9/2026.
//
#include "game/map.h"
#include "game/map_gen.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils/stack.h"
#include "game/player.h"

#define MAX_LINE_LENGTH 1024

int g_mapRows = 0;
int g_mapCols = 0;
MapTile *g_worldMap = NULL;


// generate a random map (maze or caverns)
void map_generate_random(Player *player) {
    srand(time(NULL));
    
    // Choose between Maze (0) and Organic Caverns (1)
    // For now, let's default to the new Organic Caverns
    int type = 1; 

    if (type == 0) {
        // --- OLD MAZE GENERATION ---
        g_mapRows = (rand() % 12) + 24;
        g_mapCols = (rand() % 12) + 24;

        if ((g_mapRows & 1) == 0) g_mapRows++;
        if ((g_mapCols & 1) == 0) g_mapCols++;

        if (g_worldMap == NULL) {
            g_worldMap = (MapTile *)malloc(sizeof(MapTile) * g_mapRows * g_mapCols);
            if (g_worldMap == NULL) fprintf(stderr, "Failed to allocate memory for worldMap\n");
        } else {
            g_worldMap = (MapTile *)realloc(g_worldMap, sizeof(MapTile) * g_mapRows * g_mapCols);
            if (g_worldMap == NULL) fprintf(stderr, "Failed to reallocate memory for worldMap\n");
        }

        for (int i = 0; i < g_mapRows; i++) {
            for (int j = 0; j < g_mapCols; j++) {
                g_worldMap[i * g_mapCols +j].textureId = 1;
                g_worldMap[i * g_mapCols +j].posX = j;
                g_worldMap[i * g_mapCols +j].posY = i;
            }
        }

        int visited[g_mapRows][g_mapCols];
        memset(visited, 0, sizeof(visited));
        Stack cellStack = stack_create(g_mapRows * g_mapCols);

        g_worldMap[1 * g_mapCols + 1].textureId = 0;
        visited[1][1] = 1;
        stack_push(&cellStack, &g_worldMap[1 * g_mapCols + 1]);
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

                if (ny <= 0 || ny >= g_mapRows - 1|| nx <= 0 || nx >= g_mapCols - 1) continue;
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


                g_worldMap[cell->posY * g_mapCols + cell->posX].textureId = 0;
                g_worldMap[((cell->posY + ny) / 2) * g_mapCols + ((cell->posX + nx) / 2)].textureId = 0;
                g_worldMap[ny * g_mapCols + nx].textureId = 0;

                visited[ny][nx] = 1;
                stack_push(&cellStack, &g_worldMap[ny * g_mapCols + nx]);
            }
        }
        printf("Maze generation finished!\n");
        stack_free(&cellStack);
    } else {
        // --- NEW ORGANIC CAVERN GENERATION ---
        int rows = (rand() % 10) + 30;
        int cols = (rand() % 10) + 30;
        map_gen_organic_caverns(player, rows, cols, 0.45f);
    }

    printf("Map generation finished!\n");
}

void map_load(char* path) {
    char line[MAX_LINE_LENGTH];
    g_mapRows = 0;
    g_mapCols = 0;
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open file %s\n", path);
        return;
    }

    if (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        g_mapRows++;
        char *temp = strdup(line);
        char *token = strtok(temp, " \t\r\n");
        while (token != NULL) {
            g_mapCols++;
            token = strtok(NULL, " \t\r\n");
        }
        free(temp);
    }

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        g_mapRows++;
    }

    printf("g_mapCols: %d\ng_mapRows: %d\n", g_mapCols, g_mapRows);
    if (g_worldMap != NULL) free(g_worldMap);
    g_worldMap = (MapTile*)malloc(sizeof(MapTile) * g_mapCols * g_mapRows);
    if (g_worldMap == NULL) fprintf(stderr, "Unable to allocate memory for g_worldMap\n");
    rewind(fp);
    int y = 0;
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        line[strcspn(line, "\r\n")] = 0;
        int x = 0;
        char *token = strtok(line, " \t");
        while (token != NULL) {
            g_worldMap[y * g_mapCols + x].textureId = atoi(token);
            g_worldMap[y * g_mapCols + x].posX = x;
            g_worldMap[y * g_mapCols + x].posY = y;
            x++;
            token = strtok(NULL, " \t");
        }
        y++;
    }
    fclose(fp);
    return;
}

void map_free() {
    if (g_worldMap != NULL) {
        free(g_worldMap);
        g_worldMap = NULL;
    }
    printf("g_worldMap freed\n");
}


