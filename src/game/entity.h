//
// Created by Alan Pitcher on 12/30/2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <stdbool.h>

#include "game/player.h"
#include "game/gamestate.h"
#include "engine/sprite.h"
#include <SDL3/SDL.h>

//  AI behavior states for hostile or interactive entities
typedef enum {
    ENTITY_STATE_WAIT = 0,      // Watching but frozen
    ENTITY_STATE_INACTIVE = 1,  // Dormant far from player
    ENTITY_STATE_ACTIVE = 2,    // Actively stalking
} EntityState;

//  Dynamic world object with behavior and movement
typedef struct {
    EntityState state;
    Player *player;
    Sprite *sprite;
    double speed;
    double activationRange;
    double moveTimer;
    bool isVisible;

    double targetX, targetY; // Target tile center
    double prevX, prevY;     // Previous position for interpolation
} Entity;

extern Entity *g_entities;

//  Initializes all entities linked to the SPRITE_ENTITY sprite pool
void entity_init(Player *player, Sprite *sprites);

//  Creates a random amount of entities at varied positions based on difficulty
void entity_create_random(Difficulty difficulty);

//  Process global logic and collisions for all entities in the pool
SDL_AppResult entity_update_all(double frameTime);

//  Process individual AI state machines and pathfinding steps
SDL_AppResult entity_update(Entity *entity, double frameTime);

//  Generates a scent map using BFS from player to tiles for AI tracking
void entity_update_scent_map(Player *player, double frameTime);

//  Frees all entity memory and cleans up tracking pointers
void entity_free();


#endif //ENTITY_H
