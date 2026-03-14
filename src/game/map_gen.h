//
// Created by Alan Pitcher on 03/14/2026.
//

#ifndef MAP_GEN_H
#define MAP_GEN_H

#include "game/map.h"
#include "game/player.h"

// map generation agents
typedef struct {
    int x, y;
    int lifespan;
    float turnChance;
} Walker;

//  Generates a jagged, claustrophobic labyrinth with winding corridors and rare room nests
void map_gen_organic_caverns(Player *player, int rows, int cols, float fillPercent);

//  Uses BFS from player spawn to fill isolated pockets of floor tiles with wall
void map_gen_ensure_reachability(int startX, int startY);

#endif //MAP_GEN_H
