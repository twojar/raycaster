//
// Created by Alan Pitcher on 12/30/2025.
//

#ifndef ENTITY_H
#define ENTITY_H
#include <stdbool.h>

#include "player.h"
#include "sprite.h"

extern double *scentMap;

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

void entity_Init(Entity *entity, Player *player, Sprite *sprite);
void entity_update(Entity *entity, double frameTime);
void update_scentMap(Player *player);



#endif //ENTITY_H
