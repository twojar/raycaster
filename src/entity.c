//
// Created by Alan Pitcher on 12/30/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include "entity.h"
#include <math.h>
#include <SDL3/SDL.h>

#include "graphics.h"

#define SCENT(x,y) (scentMap[(int)y * mapCols + (int)x])
#define SCENT_DECAY_RATE 0.1
#define ENTITY_ACTIVATION_RANGE 4.0
#define ENTITY_SPEED 4.0

Entity *entities;
double *scentMap;
int numEntities;
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

void entity_Init(Player* player, Sprite *sprites) {
    init_scentMap();
    for (int i = 0; i < numSprites; i++) {
        if (sprites[i].spriteType == SPRITE_ENTITY) {
            numEntities++;
        }
    }

    if (numEntities == 0) return;

    entities = (Entity*) malloc(sizeof(Entity) * numEntities);

    int j = 0;
    for (int i = 0; i < numSprites; i++) {
        if (sprites[i].spriteType == SPRITE_ENTITY) {
            entities[j].player = player;
            entities[j].speed = ENTITY_SPEED;
            entities[j].sprite = &sprites[i];
            entities[j].state = ENTITY_STATE_INACTIVE;
            entities[j].activationRange = ENTITY_ACTIVATION_RANGE;
            entities[j].isVisible = false;
            entities[j].moveTimer = 0.0;
            j++;
        }
    }
}


SDL_AppResult entities_update(double frameTime) {
    if (numEntities == 0) return SDL_APP_CONTINUE;
    for (int i = 0; i < numEntities; i++) {
        SDL_AppResult result = entity_update(&entities[i], frameTime);

        if (result == SDL_APP_SUCCESS) return result;
    }

    return SDL_APP_CONTINUE;
}


SDL_AppResult entity_update(Entity* entity, double frameTime) {
    if (numEntities == 0) return SDL_APP_CONTINUE;

    double dirToEntityX = entity->sprite->x - entity->player->posX;
    double dirToEntityY = entity->sprite->y - entity->player->posY;
    double dirToEntityX_N = dirToEntityX;
    double dirToEntityY_N = dirToEntityY;

    double entityDist = sqrt(dirToEntityX * dirToEntityX + dirToEntityY * dirToEntityY);

    if (entity->state == ENTITY_STATE_INACTIVE) {
        if (entityDist <= ENTITY_ACTIVATION_RANGE) {
            entity->state = ENTITY_STATE_ACTIVE;
        } else return SDL_APP_CONTINUE;
    }

    if (entityDist > 0) {
        dirToEntityX_N /= entityDist;
        dirToEntityY_N /= entityDist;
    } else return SDL_APP_CONTINUE;


    /* DP
     * 1.0 = Entity is in front of the player
     * 0.7 - 0.9 = Entity is within the peripheral vision of the player
     * 0.0 = Entity is 90 degrees to the player's side
     * -1.0 = Entity is behind the player
     */
    double dp = (dirToEntityX_N * entity->player->dirX) + (dirToEntityY_N * entity->player->dirY);

    bool isInFOV = dp > 0.7 ? true : false;

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
                        if (SCENT(nearX, nearY) > maxScent && SCENT(nearX, nearY) > 0) {
                            maxScent = SCENT(nearX, nearY);
                            nextX = nearX;
                            nextY = nearY;
                        }
                    }
                }
            }

            //backup path finding
            if (maxScent <= 0) {
                int dx = (int) entity->player->posX - currX;
                int dy = (int) entity->player->posY - currY;

                if (abs(dx) > abs(dy)) nextX += (dx > 0) ? 1 : -1;
                else nextY += (dy > 0) ? 1 : -1;

                if (worldMap[nextY * mapCols + nextX].textureID != 0) {
                    nextX = currX;
                    nextY = currY;
                }
            }

            entity->sprite->x = nextX + 0.5;
            entity->sprite->y = nextY + 0.5;
            entity->moveTimer = 1.0 / entity->speed;

            //will crash the game if caught
            if ( (int) entity->sprite->x == (int) entity->player->posX && (int) entity->sprite->y == (int) entity->player->posY) {
                printf("I caught you! \n");
                return SDL_APP_SUCCESS;
            }
        }
    }

    return SDL_APP_CONTINUE;
}

void update_scentMap(Player *player) {
    SCENT(player->posX, player->posY) = 1.0;

    for (int y = 0; y < scentMapRows; y++) {
        for (int x = 0; x < scentMapCols; x++) {
            SCENT(x,y) -= SCENT_DECAY_RATE * 0.016;
            if (SCENT(x,y) < 0.001) SCENT(x,y) = 0.0;
        }
    }
}

void scentMap_free() {
    if (scentMap != NULL) free(scentMap);
    printf("scentMap freed\n");
}

void entities_free() {
    if (entities != NULL) free(entities);
    printf("All entities freed\n");
}

