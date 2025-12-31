//
// Created by Alan Pitcher on 12/30/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include "entity.h"
#include <math.h>
#include "graphics.h"

#define SCENT(x,y) (scentMap[(int)y * mapCols + (int)x])
#define SCENT_DECAY_RATE 0.1
#define ENTITY_ACTIVATION_RANGE 4.0
#define ENTITY_SPEED 4.0

double *scentMap;
int scentMapRows = 0;
int scentMapCols = 0;

void init_scentMap() {
    scentMap = (double*) malloc(sizeof(double) * mapCols * mapRows);
    if (scentMap == NULL) {
        printf("Error allocating memory for scentMap\n");
        exit(1);
    }

    scentMapRows = mapRows;
    scentMapCols = mapCols;
}

void entity_Init(Entity* entity, Player* player, Sprite* sprite) {
    init_scentMap();
    //entity = (Entity*) malloc(sizeof(Entity));
    if (entity == NULL) {
        return;
    }

    entity->player = player;
    entity->sprite = sprite;
    entity->speed = ENTITY_SPEED;
    entity->activationRange = ENTITY_ACTIVATION_RANGE;
    entity->isVisible = false;
    entity->moveTimer = 0.0;

}

void entity_update(Entity* entity, double frameTime) {

    double dirToEntityX = entity->sprite->x - entity->player->posX;
    double dirToEntityY = entity->sprite->y - entity->player->posY;
    double dirToEntityX_N = dirToEntityX;
    double dirToEntityY_N = dirToEntityY;

    double entityDist = sqrt(dirToEntityX * dirToEntityX + dirToEntityY * dirToEntityY);

    if (entity->state == ENTITY_STATE_INACTIVE) {
        if (entityDist <= ENTITY_ACTIVATION_RANGE) {
            entity->state = ENTITY_STATE_ACTIVE;
        } else return;
    }

    if (entityDist > 0) {
        dirToEntityX_N /= entityDist;
        dirToEntityY_N /= entityDist;
    } else return;


    /* DP
     * 1.0 = Entity is in front of the player
     * 0.7 - 0.9 = Entity is within the peripheral vision of the player
     * 0.0 = Entity is 90 degrees to the player's side
     * -1.0 = Entity is behind the player
     */
    double dp = (dirToEntityX_N * entity->player->dirX) + (dirToEntityY_N * entity->player->dirY);

    bool isInFOV = dp > 0.5 ? true : false;

    if (isInFOV) {
        double wallDist = dda(entity->player->posX, entity->player->posY, dirToEntityX_N, dirToEntityY_N,NULL,NULL,NULL);

        if (entityDist <= wallDist) entity->isVisible = true;
        else entity->isVisible = false;


        if (entity->isVisible == true) entity->state = ENTITY_STATE_WAIT;
        else entity->state = ENTITY_STATE_ACTIVE;
    } else {
        entity->isVisible = false;
        entity->state = ENTITY_STATE_ACTIVE;
    }

    if (entity->state == ENTITY_STATE_ACTIVE) {
        entity->moveTimer -= frameTime;
        if (entity->moveTimer <= 0) {
            int currX = (int) entity->sprite->x;
            int currY = (int) entity->sprite->y;

            int nextX = currX;
            int nextY = currY;
            double maxScent = -1.0;

            int deltaX[] = {0,0,1,-1};
            int deltaY[] = {1,-1,0,0};

            for (int i = 0; i < 4; i++) {
                int nearX = currX + deltaX[i];
                int nearY = currY + deltaY[i];

                if (nearX >= 0 && nearY >= 0 && nearX < mapCols && nearY < mapRows) {
                    if (worldMap[nearY * mapCols + nearX].textureID == 0) {
                        if (SCENT(nearX, nearY) > maxScent) {
                            maxScent = SCENT(nearX, nearY);
                            nextX = nearX;
                            nextY = nearY;
                        }
                    }
                }
            }
            entity->sprite->x = nextX + 0.5;
            entity->sprite->y = nextY + 0.5;
            entity->moveTimer = 1.0 / entity->speed;
        }
    }
}

void update_scentMap(Player *player) {
    SCENT(player->posX, player->posY) = 1.0;

    for (int y = 0; y < scentMapRows; y++) {
        for (int x = 0; x < scentMapCols; x++) {
            SCENT(x,y) -= SCENT_DECAY_RATE * 0.016;
            if (SCENT(x,y) < 0.0) SCENT(x,y) = 0.0;
        }
    }
}

