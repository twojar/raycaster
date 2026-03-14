//
// Created by Alan Pitcher on 12/28/2025.
//

#ifndef MAP_H
#define MAP_H
#include "game/player.h"
//  Represents each tile in the nxm g_worldMap grid
typedef struct {
    int textureId;
    int posX;
    int posY;
    int tagId;
    float scent;
} MapTile;

extern int g_mapRows;
extern int g_mapCols;

extern MapTile *g_worldMap;

void map_generate_random(Player *player);
void map_load(char* path);
void map_free();

#endif //MAP_H
