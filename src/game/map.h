//
// Created by Alan Pitcher on 12/28/2025.
//

#ifndef MAP_H
#define MAP_H

//  Represents each tile in the nxm worldMap grid
typedef struct {
    int textureID;
    int posX;
    int posY;
    int tagID;
    float scent;
} MapTile;

extern int mapRows;
extern int mapCols;

extern MapTile *worldMap;

void random_map(Player *player);
void load_map(char* path);

#endif //MAP_H
