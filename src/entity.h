//
// Created by Alan Pitcher on 12/30/2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <stdbool.h>

#include "player.h"
#include "sprite.h"
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

extern double *scentMap;
extern Entity *entities;

void entity_Init(Player *player, Sprite *sprites);
void create_random_entities();
SDL_AppResult entities_update(double frameTime);
SDL_AppResult entity_update(Entity *entity, double frameTime);
void update_scentMap(Player *player);
void entities_free();


#endif //ENTITY_H
