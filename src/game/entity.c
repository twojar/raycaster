//
// Created by Alan Pitcher on 12/30/2025.
//
#include "game/entity.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "engine/graphics.h"

//  handles pointer math
#define SCENT(x,y) (worldMap[(int)y * mapCols + (int)x].scent)

//  the range around an entity that players must be in for the entity to switch from INACTIVE -> ACTIVE
#define ENTITY_ACTIVATION_RANGE 4.0

//  how fast the entity moves per movement tick
#define ENTITY_SPEED 2.0

Entity *entities;
int numEntities = 0;
int scentMapRows = 0;
int scentMapCols = 0;

//  Creates a random amount of entities in the world
//  Will either be difficulty based or dependent on size of worldMap
void create_random_entities() {
    // Generate some default sprites if none exist
    if (numSprites == 0) {
        random_sprites();
    }
}

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
    numEntities = 0;
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

            int deltaX[] = {0, 0, 1, -1};
            int deltaY[] = {1, -1, 0, 0};

            //  Follow the scent gradient towards the player
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

            //  Backup: if no path found via scent, try moving in the general direction of the player
            if (maxScent <= 0.0) {
                int dx = (int)entity->player->posX - currX;
                int dy = (int)entity->player->posY - currY;

                if (dx != 0 || dy != 0) {
                    int stepX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
                    int stepY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

                    // Try moving in the direction of the larger difference first
                    if (abs(dx) > abs(dy)) {
                        if (currX + stepX >= 0 && currX + stepX < mapCols &&
                            worldMap[currY * mapCols + (currX + stepX)].textureID == 0) {
                            nextX = currX + stepX;
                        } else if (stepY != 0 && currY + stepY >= 0 && currY + stepY < mapRows &&
                                   worldMap[(currY + stepY) * mapCols + currX].textureID == 0) {
                            nextY = currY + stepY;
                        }
                    } else {
                        if (currY + stepY >= 0 && currY + stepY < mapRows &&
                            worldMap[(currY + stepY) * mapCols + currX].textureID == 0) {
                            nextY = currY + stepY;
                        } else if (stepX != 0 && currX + stepX >= 0 && currX + stepX < mapCols &&
                                   worldMap[currY * mapCols + (currX + stepX)].textureID == 0) {
                            nextX = currX + stepX;
                        }
                    }
                }
            }

            entity->sprite->x = nextX + 0.5;
            entity->sprite->y = nextY + 0.5;
            entity->moveTimer = 1.0 / entity->speed;

            //  Check for collision with player (game over)
            double pdx = entity->sprite->x - entity->player->posX;
            double pdy = entity->sprite->y - entity->player->posY;
            float r = 0.4f;
            if (pdx * pdx + pdy * pdy < r * r) {
                printf("I caught you!\n");
                return SDL_APP_SUCCESS;
            }
        }
    }

    return SDL_APP_CONTINUE;
}

// Generates a scent map using BFS from the player's position
// This creates a gradient that entities can follow from anywhere in the maze
void update_scentMap(Player *player, double frameTime) {
    if (worldMap == NULL) return;

    // Reset scent for all tiles
    for (int i = 0; i < mapRows * mapCols; i++) {
        worldMap[i].scent = 0.0f;
    }

    int px = (int)player->posX;
    int py = (int)player->posY;

    if (px < 0 || px >= mapCols || py < 0 || py >= mapRows) return;

    // BFS queue
    int *queue = (int*)malloc(sizeof(int) * mapRows * mapCols);
    if (!queue) return;

    int head = 0, tail = 0;
    queue[tail++] = py * mapCols + px;
    worldMap[py * mapCols + px].scent = 1.0f;

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (head < tail) {
        int currIdx = queue[head++];
        int cx = currIdx % mapCols;
        int cy = currIdx / mapCols;
        float currentScent = worldMap[currIdx].scent;

        // Spread scent to neighbors
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < mapCols && ny >= 0 && ny < mapRows) {
                int nIdx = ny * mapCols + nx;
                if (worldMap[nIdx].textureID == 0 && worldMap[nIdx].scent == 0.0f) {
                    // Scent slightly decays with distance to create a gradient
                    worldMap[nIdx].scent = currentScent * 0.99f;
                    queue[tail++] = nIdx;
                }
            }
        }
    }

    free(queue);
}

void entities_free() {
    if (entities != NULL) free(entities);
    printf("All entities freed\n");
}