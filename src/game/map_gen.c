//
// Created by Alan Pitcher on 03/14/2026.
//

#include "game/map_gen.h"
#include "utils/queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

// Helper to carve a small, jagged blob
static void carve_cramped_blob(int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int tx = cx + dx;
            int ty = cy + dy;
            if (tx >= 2 && tx < g_mapCols - 2 && ty >= 2 && ty < g_mapRows - 2) {
                // High randomness for jagged edges
                if ((rand() % 100) < 70) { 
                    g_worldMap[ty * g_mapCols + tx].textureId = 0;
                }
            }
        }
    }
}

// Tight, erratic walker for corridors
static void carve_claustrophobic_corridor(int x1, int y1, int x2, int y2) {
    int curX = x1;
    int curY = y1;

    while (curX != x2 || curY != y2) {
        // High randomness (60%) makes it very winding/inefficient
        if ((rand() % 100) < 60) {
            int dir = rand() % 4;
            if (dir == 0) curX++;
            else if (dir == 1) curX--;
            else if (dir == 2) curY++;
            else curY--;
        } else {
            if (curX < x2) curX++;
            else if (curX > x2) curX--;
            else if (curY < y2) curY++;
            else if (curY > y2) curY--;
        }

        if (curX < 1) curX = 1;
        if (curX >= g_mapCols - 1) curX = g_mapCols - 2;
        if (curY < 1) curY = 1;
        if (curY >= g_mapRows - 1) curY = g_mapRows - 2;

        g_worldMap[curY * g_mapCols + curX].textureId = 0;
        // No extra carving in corridors to keep them 1-tile wide
    }
}

void map_gen_organic_caverns(Player *player, int rows, int cols, float fillPercent) {
    g_mapRows = rows;
    g_mapCols = cols;

    if (g_worldMap == NULL) {
        g_worldMap = (MapTile *)malloc(sizeof(MapTile) * g_mapRows * g_mapCols);
    } else {
        g_worldMap = (MapTile *)realloc(g_worldMap, sizeof(MapTile) * g_mapRows * g_mapCols);
    }

    if (g_worldMap == NULL) return;

    // Start with solid stone
    for (int i = 0; i < g_mapRows * g_mapCols; i++) {
        g_worldMap[i].textureId = 1;
        g_worldMap[i].posX = i % g_mapCols;
        g_worldMap[i].posY = i / g_mapCols;
        g_worldMap[i].scent = 0.0f;
    }

    // 1. Fewer, smaller "Nests" (instead of big rooms)
    const int NUM_NESTS = (rows * cols) / 400; // Much lower density
    typedef struct { int x, y; } Point;
    Point *nests = malloc(sizeof(Point) * NUM_NESTS);

    for (int i = 0; i < NUM_NESTS; i++) {
        nests[i].x = (rand() % (cols - 10)) + 5;
        nests[i].y = (rand() % (rows - 10)) + 5;
        
        // Small, cramped nest
        int nx = nests[i].x;
        int ny = nests[i].y;
        for (int j = 0; j < 6; j++) {
            carve_cramped_blob(nx, ny, 1);
            nx += (rand() % 3) - 1;
            ny += (rand() % 3) - 1;
        }
    }

    // 2. Erratic, winding corridors
    for (int i = 0; i < NUM_NESTS - 1; i++) {
        carve_claustrophobic_corridor(nests[i].x, nests[i].y, nests[i+1].x, nests[i+1].y);
    }
    carve_claustrophobic_corridor(nests[NUM_NESTS-1].x, nests[NUM_NESTS-1].y, nests[0].x, nests[0].y);

    // 3. random dead ends
    for (int i = 0; i < NUM_NESTS; i++) {
        int tx = (rand() % (cols - 4)) + 2;
        int ty = (rand() % (rows - 4)) + 2;
        carve_claustrophobic_corridor(nests[i].x, nests[i].y, tx, ty);
    }

    // 4. add pillars to break up large areas
    for (int y = 2; y < g_mapRows - 2; y++) {
        for (int x = 2; x < g_mapCols - 2; x++) {
            if (g_worldMap[y * g_mapCols + x].textureId == 0) {
                // Check if this is a 3x3 open area
                bool open = true;
                for (int ny = -1; ny <= 1; ny++) {
                    for (int nx = -1; nx <= 1; nx++) {
                        if (g_worldMap[(y+ny) * g_mapCols + (x+nx)].textureId != 0) {
                            open = false; break;
                        }
                    }
                }
                // If it's a big open space, put a pillar in the middle
                if (open && (rand() % 100) < 40) {
                    g_worldMap[y * g_mapCols + x].textureId = 1;
                }
            }
        }
    }

    free(nests);

    map_gen_ensure_reachability(cols / 2, rows / 2);

    bool spawned = false;
    for (int y = 1; y < g_mapRows - 1 && !spawned; y++) {
        for (int x = 1; x < g_mapCols - 1; x++) {
            if (g_worldMap[y * g_mapCols + x].textureId == 0) {
                player_teleport(player, (double)x + 0.5, (double)y + 0.5);
                spawned = true;
                break;
            }
        }
    }

    printf("Catacombs generation finished!\n");
}

void map_gen_ensure_reachability(int startX, int startY) {
    int size = g_mapRows * g_mapCols;
    bool *reachable = (bool*)calloc(size, sizeof(bool));
    if (!reachable) return;

    Queue q = queue_create(size);
    if (!q.arr) { free(reachable); return; }

    int foundX = -1, foundY = -1;
    for (int y = 1; y < g_mapRows - 1 && foundX == -1; y++) {
        for (int x = 1; x < g_mapCols - 1; x++) {
            if (g_worldMap[y * g_mapCols + x].textureId == 0) {
                foundX = x; foundY = y; break;
            }
        }
    }

    if (foundX == -1) { queue_free(&q); free(reachable); return; }

    queue_enqueue(&q, (void*)(intptr_t)(foundY * g_mapCols + foundX));
    reachable[foundY * g_mapCols + foundX] = true;

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!queue_is_empty(&q)) {
        int currIdx = (int)(intptr_t)queue_dequeue(&q);
        int cx = currIdx % g_mapCols;
        int cy = currIdx / g_mapCols;

        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < g_mapCols && ny >= 0 && ny < g_mapRows) {
                int nIdx = ny * g_mapCols + nx;
                if (!reachable[nIdx] && g_worldMap[nIdx].textureId == 0) {
                    reachable[nIdx] = true;
                    queue_enqueue(&q, (void*)(intptr_t)nIdx);
                }
            }
        }
    }

    for (int i = 0; i < size; i++) {
        if (g_worldMap[i].textureId == 0 && !reachable[i]) {
            g_worldMap[i].textureId = 1;
        }
    }

    queue_free(&q);
    free(reachable);
}
