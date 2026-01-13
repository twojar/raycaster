//
// Created by Alan Pitcher on 12/28/2025.
//

#ifndef MAPTILE_H
#define MAPTILE_H

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

void random_map();
void load_map(char* path);

#endif //MAPTILE_H
