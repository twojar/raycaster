//
// Created by Alan Pitcher on 12/30/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include "entity.h"
#include <math.h>
#include <SDL3/SDL.h>
#include "graphics.h"

//  handles pointer math
#define SCENT(x,y) (worldMap[(int)y * mapCols + (int)x].scent)

//  the rate at which the player's scent decays at on scentMap
#define SCENT_DECAY_RATE 0.1

//  the range around an entity that players must be in for the entity to switch from INACTIVE -> ACTIVE
#define ENTITY_ACTIVATION_RANGE 4.0

//  how fast the entity moves per movement tick
#define ENTITY_SPEED 2.0

Entity *entities;
double *scentMap;
int numEntities;
int scentMapRows = 0;
int scentMapCols = 0;

//  Creates a random amount of entities in the world
//  Will either be difficulty based or dependent on size of worldMap
void create_random_entities() {}

// Randomizes spawn positions of all entities in the world
void randomize_entities() {
    for (int i = 0; i < numEntities; i++) {
        entities[i].sprite->x = rand() % mapCols;
        entities[i].sprite->y = rand() % mapRows;
        while (worldMap[(int) entities[i].sprite->y * mapCols + (int) entities[i].sprite->x].textureID > 0) {
            entities[i].sprite->x = rand() % mapCols;
            entities[i].sprite->y = rand() % mapRows;
        }
    }

    for (int i = 0; i < numEntities; i++) {
        printf("Entity %d: Spawned at %d, %d\n", i+1, (int) entities[i].sprite->x, (int) entities[i].sprite->y);
    }
}

//  Initializes all entities
void entity_Init(Player* player, Sprite *sprites) {
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


    scentMapRows = mapRows;
    scentMapCols = mapCols;

    randomize_entities();
}

//  Handles entity logic and movement
//  runs every frame
SDL_AppResult entities_update(double frameTime) {
    if (numEntities == 0) return SDL_APP_CONTINUE;
    for (int i = 0; i < numEntities; i++) {
        SDL_AppResult result = entity_update(&entities[i], frameTime);

        if (result == SDL_APP_SUCCESS) return result;
    }

    return SDL_APP_CONTINUE;
}

// Entities start in the INACTIVE state
// If a player moves within 4 tiles of the entity it will get "aggravated"
// Weeping Angel type logic, entity only moves if it is not visible to the player
// Players leave a scent wherever they step corresponding to scentMap
// The entity will move to the position with the highest "scent" number
// Backup logic just moves the entity closer to player position lol
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


    //  DP (Dot Product)
    //  1.0 = Entity is in front of the player (player looking at entity)
    //  0.7 - 0.9 = Entity is within the peripheral vision of the player
    //  0.0 = Entity is 90 degrees to the player's side
    //  -1.0 = Entity is behind the player

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

                        if (SCENT(nearX, nearY) > maxScent) {
                            maxScent = SCENT(nearX, nearY);
                            nextX = nearX;
                            nextY = nearY;
                        }
                    }
                }
            }

            //  backup path finding
            if (maxScent < 0) {
                int dx = (int) entity->player->posX - currX;
                int dy = (int) entity->player->posY - currY;

                // check walls before moving
                int testX = currX;
                int testY = currY;

                if (abs(dx) > abs(dy)) {
                    testX += (dx > 0) ? 1 : -1;
                } else {
                    testY += (dy > 0) ? 1 : -1;
                }

                // only move if the destination is valid
                if (testX >= 0 && testX < mapCols && testY >= 0 && testY < mapRows) {
                    if (worldMap[testY * mapCols + testX].textureID == 0) {
                        nextX = testX;
                        nextY = testY;
                    }
                }
            }

            entity->sprite->x = nextX + 0.5;
            entity->sprite->y = nextY + 0.5;
            entity->moveTimer = 1.0 / entity->speed;

            //  currently will crash the game if caught
            int dx = entity->sprite->x - entity->player->posX;
            int dy = entity->sprite->y - entity->player->posY;
            float r = 0.35f;
            if (dx*dx + dy*dy < r*r) {
                printf("I caught you! \n");
                return SDL_APP_SUCCESS;
            }
        }
    }

    return SDL_APP_CONTINUE;
}

// updates scentMap
// runs every frame
void update_scentMap(Player *player) {
    SCENT(player->posX, player->posY) = 1.0;

    for (int y = 0; y < scentMapRows; y++) {
        for (int x = 0; x < scentMapCols; x++) {
            SCENT(x,y) -= SCENT_DECAY_RATE * 0.016;
            if (SCENT(x,y) < 0.001) SCENT(x,y) = 0.0;
        }
    }
}

void entities_free() {
    if (entities != NULL) free(entities);
    printf("All entities freed\n");
}