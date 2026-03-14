//
// Created by Alan Pitcher on 12/30/2025.
//
#include "game/entity.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "engine/graphics.h"
#include "utils/queue.h"
#include <stdint.h>

//  handles pointer math
#define SCENT(x,y) (g_worldMap[(int)y * g_mapCols + (int)x].scent)

//  the range around an entity that players must be in for the entity to switch from INACTIVE -> ACTIVE
#define ENTITY_ACTIVATION_RANGE 4.0

//  how fast the entity moves per movement tick
#define ENTITY_SPEED 2.0

Entity *g_entities;
int g_numEntities = 0;
int g_scentMapRows = 0;
int g_scentMapCols = 0;

//  Creates a random amount of entities in the world based on difficulty
void entity_create_random(Difficulty difficulty) {
    int num = 0;
    switch (difficulty) {
        case DIFF_EASY:
            num = (rand() % 2) + 2; // 2-3
            break;
        case DIFF_NORMAL:
            num = (rand() % 3) + 4; // 4-6
            break;
        case DIFF_HARD:
            num = (rand() % 5) + 8; // 8-12
            break;
        case DIFF_NIGHTMARE:
            num = (rand() % 9) + 16; // 16-24
            break;
        default:
            num = 5;
            break;
    }

    // Generate some default sprites if none exist
    if (g_numSprites == 0) {
        sprite_random(num);
    }
}

// Randomizes spawn positions of all entities in the world
void entity_randomize_positions() {
    for (int i = 0; i < g_numEntities; i++) {
        g_entities[i].sprite->x = (float)(rand() % g_mapCols) + 0.5f;
        g_entities[i].sprite->y = (float)(rand() % g_mapRows) + 0.5f;
        while (g_worldMap[(int) g_entities[i].sprite->y * g_mapCols + (int) g_entities[i].sprite->x].textureId > 0) {
            g_entities[i].sprite->x = (float)(rand() % g_mapCols) + 0.5f;
            g_entities[i].sprite->y = (float)(rand() % g_mapRows) + 0.5f;
        }
        g_entities[i].targetX = g_entities[i].sprite->x;
        g_entities[i].targetY = g_entities[i].sprite->y;
        g_entities[i].prevX = g_entities[i].sprite->x;
        g_entities[i].prevY = g_entities[i].sprite->y;
    }

    for (int i = 0; i < g_numEntities; i++) {
        printf("Entity %d: Spawned at %d, %d\n", i+1, (int) g_entities[i].sprite->x, (int) g_entities[i].sprite->y);
    }
}

//  Initializes all entities
void entity_init(Player* player, Sprite *sprites) {
    g_numEntities = 0;
    for (int i = 0; i < g_numSprites; i++) {
        if (sprites[i].spriteType == SPRITE_ENTITY) {
            g_numEntities++;
        }
    }

    if (g_numEntities == 0) return;

    g_entities = (Entity*) malloc(sizeof(Entity) * g_numEntities);

    int j = 0;
    for (int i = 0; i < g_numSprites; i++) {
        if (sprites[i].spriteType == SPRITE_ENTITY) {
            g_entities[j].player = player;
            g_entities[j].speed = ENTITY_SPEED;
            g_entities[j].sprite = &sprites[i];
            g_entities[j].state = ENTITY_STATE_INACTIVE;
            g_entities[j].activationRange = ENTITY_ACTIVATION_RANGE;
            g_entities[j].isVisible = false;
            g_entities[j].moveTimer = 0.0;
            g_entities[j].targetX = sprites[i].x;
            g_entities[j].targetY = sprites[i].y;
            g_entities[j].prevX = sprites[i].x;
            g_entities[j].prevY = sprites[i].y;
            j++;
        }
    }


    g_scentMapRows = g_mapRows;
    g_scentMapCols = g_mapCols;

    entity_randomize_positions();
}

//  Handles entity logic and movement
//  runs every frame
SDL_AppResult entity_update_all(double frameTime) {
    if (g_numEntities == 0) return SDL_APP_CONTINUE;
    for (int i = 0; i < g_numEntities; i++) {
        g_entities[i].prevX = g_entities[i].sprite->x;
        g_entities[i].prevY = g_entities[i].sprite->y;
        SDL_AppResult result = entity_update(&g_entities[i], frameTime);

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
    if (g_numEntities == 0) return SDL_APP_CONTINUE;

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

    // Entity is "seen" if it is in the player's field of view (FOV),
    // even if behind a wall, to prevent it from "teleporting" while the player is looking that way.
    bool isInFOV = dp > 0.5 ? true : false; // Using 0.5 for a wider FOV (~120 degrees)

    if (isInFOV) {
        entity->isVisible = true;
        entity->state = ENTITY_STATE_WAIT;
    } else {
        entity->isVisible = false;
        entity->state = ENTITY_STATE_ACTIVE;
    }

    if (entity->state == ENTITY_STATE_ACTIVE) {
        // Move towards target
        double dx = entity->targetX - entity->sprite->x;
        double dy = entity->targetY - entity->sprite->y;
        double distToTarget = sqrt(dx * dx + dy * dy);

        if (distToTarget > 0.05) {
            entity->sprite->x += (float)((dx / distToTarget) * entity->speed * frameTime);
            entity->sprite->y += (float)((dy / distToTarget) * entity->speed * frameTime);
        } else {
            // Reached target, pick next one
            int currX = (int) entity->sprite->x;
            int currY = (int) entity->sprite->y;

            int nextX = currX;
            int nextY = currY;
            double maxScent = -1.0;

            int deltaX[] = {0, 0, 1, -1};
            int deltaY[] = {1, -1, 0, 0};

            for (int i = 0; i < 4; i++) {
                int nearX = currX + deltaX[i];
                int nearY = currY + deltaY[i];

                if (nearX >= 0 && nearY >= 0 && nearX < g_mapCols && nearY < g_mapRows) {
                    if (g_worldMap[nearY * g_mapCols + nearX].textureId == 0) {
                        if (SCENT(nearX, nearY) > maxScent) {
                            maxScent = SCENT(nearX, nearY);
                            nextX = nearX;
                            nextY = nearY;
                        }
                    }
                }
            }

            if (maxScent <= 0.0) {
                // Backup logic
                int bdx = (int)entity->player->posX - currX;
                int bdy = (int)entity->player->posY - currY;
                if (bdx != 0 || bdy != 0) {
                    int stepX = (bdx > 0) ? 1 : (bdx < 0 ? -1 : 0);
                    int stepY = (bdy > 0) ? 1 : (bdy < 0 ? -1 : 0);
                    if (abs(bdx) > abs(bdy)) {
                        if (currX + stepX >= 0 && currX + stepX < g_mapCols && g_worldMap[currY * g_mapCols + (currX + stepX)].textureId == 0) nextX = currX + stepX;
                    } else {
                        if (currY + stepY >= 0 && currY + stepY < g_mapRows && g_worldMap[(currY + stepY) * g_mapCols + currX].textureId == 0) nextY = currY + stepY;
                    }
                }
            }
            entity->targetX = nextX + 0.5;
            entity->targetY = nextY + 0.5;
        }

        //  Check for collision with player (game over)
        double pdx = entity->sprite->x - entity->player->posX;
        double pdy = entity->sprite->y - entity->player->posY;
        float r = 0.4f;
        if (pdx * pdx + pdy * pdy < r * r) {
            printf("I caught you!\n");
            return SDL_APP_SUCCESS;
        }
    }

    return SDL_APP_CONTINUE;
}

// Generates a scent map using BFS from the player's position
// This creates a gradient that entities can follow from anywhere in the maze
void entity_update_scent_map(Player *player, double frameTime) {
    if (g_worldMap == NULL) return;

    // Reset scent for all tiles
    for (int i = 0; i < g_mapRows * g_mapCols; i++) {
        g_worldMap[i].scent = 0.0f;
    }

    int px = (int)player->posX;
    int py = (int)player->posY;

    if (px < 0 || px >= g_mapCols || py < 0 || py >= g_mapRows) return;

    // BFS queue
    Queue queue = queue_create(g_mapRows * g_mapCols);
    if (!queue.arr) return;

    queue_enqueue(&queue, (void*)(intptr_t)(py * g_mapCols + px));
    g_worldMap[py * g_mapCols + px].scent = 1.0f;

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!queue_is_empty(&queue)) {
        int currIdx = (int)(intptr_t)queue_dequeue(&queue);
        int cx = currIdx % g_mapCols;
        int cy = currIdx / g_mapCols;
        float currentScent = g_worldMap[currIdx].scent;

        // Spread scent to neighbors
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            if (nx >= 0 && nx < g_mapCols && ny >= 0 && ny < g_mapRows) {
                int nIdx = ny * g_mapCols + nx;
                if (g_worldMap[nIdx].textureId == 0 && g_worldMap[nIdx].scent == 0.0f) {
                    // Scent slightly decays with distance to create a gradient
                    g_worldMap[nIdx].scent = currentScent * 0.99f;
                    queue_enqueue(&queue, (void*)(intptr_t)nIdx);
                }
            }
        }
    }

    queue_free(&queue);
}

void entity_free() {
    if (g_entities != NULL) free(g_entities);
    printf("All entities freed\n");
}