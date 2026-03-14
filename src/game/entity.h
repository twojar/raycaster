//
// Created by Alan Pitcher on 12/30/2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <stdbool.h>

#include "game/player.h"
#include "engine/sprite.h"
#include <SDL3/SDL.h>

typedef enum {
    ENTITY_STATE_WAIT = 0,
    ENTITY_STATE_INACTIVE = 1,
    ENTITY_STATE_ACTIVE = 2,
} EntityState;

typedef struct {
    EntityState state;
    Player *player;
    Sprite *sprite;
    double speed;
    double activationRange;
    double moveTimer;
    bool isVisible;
} Entity;

extern Entity *g_entities;

void entity_init(Player *player, Sprite *sprites);
void entity_create_random();
SDL_AppResult entity_update_all(double frameTime);
SDL_AppResult entity_update(Entity *entity, double frameTime);
void entity_update_scent_map(Player *player, double frameTime);
void entity_free();


#endif //ENTITY_H
