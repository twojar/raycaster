//
// Created by Alan Pitcher on 12/28/2025.
//

#ifndef MAP_H
#define MAP_H
#include "game/player.h"

//  Single grid tile data for collision, texture, and pathfinding
typedef struct {
    int textureId;  //  0 for floor, >0 for wall textures
    int posX;
    int posY;
    int tagId;      //  Special event ID
    float scent;    //  Player trail tracker for entities
} MapTile;

//  Dynamic world dimensions
extern int g_mapRows;
extern int g_mapCols;

//  Global grid array of size [g_mapRows * g_mapCols]
extern MapTile *g_worldMap;

//  Triggers the random generator to build a maze or cavern
void map_generate_random(Player *player);

//  Loads a pre-defined layout from a .SAMD file
void map_load(char *path);

//  Cleans up map memory and resets globals
void map_free();

#endif //MAP_H
