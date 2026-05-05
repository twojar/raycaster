#include "game/player.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine/graphics.h"
#include "audio/audio.h"

// Boundary around the player for collision detection
#define PLAYER_BOUNDARY 0.1

// Movement tuning
#define PLAYER_BASE_MOVE_SPEED 3.2
#define PLAYER_SPRINT_MOVE_SPEED 5.2
#define PLAYER_STRAFE_SPEED_MULTIPLIER 0.95
#define PLAYER_KEYBOARD_ROTATION_SPEED 5.4
#define PLAYER_MOUSE_ROTATION_SPEED 1.0
#define PLAYER_ACCELERATION 30.0
#define PLAYER_DECELERATION 22.0
#define PLAYER_TURN_ACCELERATION 40.0
#define PLAYER_TURN_DECELERATION 30.0
#define PLAYER_WALK_FOOTSTEP_INTERVAL 0.35
#define PLAYER_SPRINT_FOOTSTEP_INTERVAL 0.20

static double approach(double current, double target, double delta) {
    if (current < target) {
        current += delta;
        if (current > target) current = target;
    } else if (current > target) {
        current -= delta;
        if (current < target) current = target;
    }

    return current;
}

static void player_move_delta(Player *player, double deltaX, double deltaY) {
    if (deltaX != 0.0) {
        double newPosX = player->posX + deltaX;
        double hitboxX = (deltaX > 0) ? (newPosX + PLAYER_BOUNDARY) : (newPosX - PLAYER_BOUNDARY);
        if (g_worldMap[(int) player->posY * g_mapCols + (int) hitboxX].textureId == 0) {
            player->posX = newPosX;
        }
    }

    if (deltaY != 0.0) {
        double newPosY = player->posY + deltaY;
        double hitboxY = (deltaY > 0) ? (newPosY + PLAYER_BOUNDARY) : (newPosY - PLAYER_BOUNDARY);
        if (g_worldMap[(int) hitboxY * g_mapCols + (int) player->posX].textureId == 0) {
            player->posY = newPosY;
        }
    }
}

void player_init(Player *player) {
    player->posX = 12;
    player->posY = 20;
    player->dirX = -1;
    player->dirY = 0;
    player->planeX = 0;
    player->planeY = 0.66;
    
    player->prevPosX = player->posX;
    player->prevPosY = player->posY;
    player->prevDirX = player->dirX;
    player->prevDirY = player->dirY;
    player->prevPlaneX = player->planeX;
    player->prevPlaneY = player->planeY;
    player->movSpeed = PLAYER_BASE_MOVE_SPEED;
    player->rotSpeed = PLAYER_KEYBOARD_ROTATION_SPEED;
    player->forwardVelocity = 0.0;
    player->strafeVelocity = 0.0;
    player->turnVelocity = 0.0;
    player->footstepTimer = 0.0;
    player->health = 100.0;
    player->weaponCount = 1;
    player->currentWeaponIndex = 0;

    for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
        player->weapons[i].name = NULL;
        player->weapons[i].id = -1;
        player->weapons[i].damage = 0;
        player->weapons[i].ammoInMag = 0;
        player->weapons[i].magCapacity = 0;
        player->weapons[i].reserveAmmo = 0;
        player->weapons[i].state = WEAPON_STATE_EMPTY;
        player->weapons[i].type = WEAPON_TYPE_HITSCAN;
        player->weapons[i].fireCooldown = 0.0;
        player->weapons[i].fireTimer = 0.0;
        player->weapons[i].reloadDuration = 0.0;
        player->weapons[i].reloadTimer = 0.0;
        player->weapons[i].range = 0.0;
    }

    weapon_init_pistol(&player->weapons[0]);
}

//  debug purposes
void player_teleport(Player *player, double posX, double posY) {
    player->posX = posX;
    player->posY = posY;
    printf("Player teleported to (%d,%d)\n",(int) player->posX,(int) player->posY);
}

//  runs every frame
void player_update(Player *player, InputState *input, double frameTime) {
    double moveSpeed = input->sprint ? PLAYER_SPRINT_MOVE_SPEED : PLAYER_BASE_MOVE_SPEED;
    double targetForwardVelocity = 0.0;
    double targetStrafeVelocity = 0.0;
    double targetTurnVelocity = 0.0;
    double moveAcceleration = PLAYER_ACCELERATION * frameTime;
    double moveDeceleration = PLAYER_DECELERATION * frameTime;
    double turnAcceleration = PLAYER_TURN_ACCELERATION * frameTime;
    double turnDeceleration = PLAYER_TURN_DECELERATION * frameTime;

    player->movSpeed = moveSpeed;
    player->rotSpeed = PLAYER_KEYBOARD_ROTATION_SPEED;

    if (input->up) targetForwardVelocity += moveSpeed;
    if (input->down) targetForwardVelocity -= moveSpeed;
    if (input->right) targetStrafeVelocity += moveSpeed * PLAYER_STRAFE_SPEED_MULTIPLIER;
    if (input->left) targetStrafeVelocity -= moveSpeed * PLAYER_STRAFE_SPEED_MULTIPLIER;
    if (input->rotateRight) targetTurnVelocity += PLAYER_KEYBOARD_ROTATION_SPEED;
    if (input->rotateLeft) targetTurnVelocity -= PLAYER_KEYBOARD_ROTATION_SPEED;

    player->forwardVelocity = approach(
        player->forwardVelocity,
        targetForwardVelocity,
        targetForwardVelocity == 0.0 ? moveDeceleration : moveAcceleration
    );
    player->strafeVelocity = approach(
        player->strafeVelocity,
        targetStrafeVelocity,
        targetStrafeVelocity == 0.0 ? moveDeceleration : moveAcceleration
    );
    player->turnVelocity = approach(
        player->turnVelocity,
        targetTurnVelocity,
        targetTurnVelocity == 0.0 ? turnDeceleration : turnAcceleration
    );

    double moveDeltaX = ((player->dirX * player->forwardVelocity) + (player->planeX * player->strafeVelocity)) * frameTime;
    double moveDeltaY = ((player->dirY * player->forwardVelocity) + (player->planeY * player->strafeVelocity)) * frameTime;
    player_move_delta(player, moveDeltaX, moveDeltaY);

    if (player->turnVelocity > 0.0) {
        player_rotate_right(player, player->turnVelocity * frameTime);
    } else if (player->turnVelocity < 0.0) {
        player_rotate_left(player, -player->turnVelocity * frameTime);
    }

    // Mouse rotation
    if (input->mouseXRel != 0) {
        if (input->mouseXRel > 0) {
            player_rotate_right(player, input->mouseXRel * MOUSE_SENSITIVITY * PLAYER_MOUSE_ROTATION_SPEED);
        } else {
            player_rotate_left(player, -input->mouseXRel * MOUSE_SENSITIVITY * PLAYER_MOUSE_ROTATION_SPEED);
        }
    }

    int isMoving = fabs(player->forwardVelocity) > 0.1 || fabs(player->strafeVelocity) > 0.1;
    if (isMoving) {
        player->footstepTimer -= frameTime;
        if (player->footstepTimer <= 0) {
            audio_play_footstep();
            if (input->sprint) {
                player->footstepTimer = PLAYER_SPRINT_FOOTSTEP_INTERVAL;
            } else {
                player->footstepTimer = PLAYER_WALK_FOOTSTEP_INTERVAL;
            }
        }
    }
    else {
        player->footstepTimer = 0;
    }
    //printf("%f,%f\n",player->posX,player->posY);
}

void player_move_forward(Player *player, double distance) {
    player_move_delta(player, player->dirX * distance, player->dirY * distance);
}

void player_move_backward(Player *player, double distance) {
    player_move_delta(player, -player->dirX * distance, -player->dirY * distance);
}

void player_rotate_right(Player *player, double rotAngle) {
    double oldDirX = player->dirX;
    player->dirX = player->dirX * cos(-rotAngle) - player->dirY * sin(-rotAngle);
    player->dirY = oldDirX * sin(-rotAngle) + player->dirY * cos(-rotAngle);

    double oldPlaneX = player->planeX;
    player->planeX = player->planeX * cos(-rotAngle) - player->planeY * sin(-rotAngle);
    player->planeY = oldPlaneX * sin(-rotAngle) + player->planeY * cos(-rotAngle);
}

void player_rotate_left(Player *player, double rotAngle) {
    double oldDirX = player->dirX;
    player->dirX = player->dirX * cos(rotAngle) - player->dirY * sin(rotAngle);
    player->dirY = oldDirX * sin(rotAngle) + player->dirY * cos(rotAngle);

    double oldPlaneX = player->planeX;
    player->planeX = player->planeX * cos(rotAngle) - player->planeY * sin(rotAngle);
    player->planeY = oldPlaneX * sin(rotAngle) + player->planeY * cos(rotAngle);
}

void player_move_left(Player *player, double distance) {
    player_move_delta(player, -(player->planeX) * distance, -(player->planeY) * distance);
}

void player_move_right(Player *player, double distance) {
    player_move_delta(player, player->planeX * distance, player->planeY * distance);
}

Weapon *player_get_current_weapon(Player *player) {
    if (player == NULL) return NULL;
    if (player->weaponCount <= 0) return NULL;
    if (player->currentWeaponIndex < 0 || player->currentWeaponIndex >= player->weaponCount) return NULL;

    return &player->weapons[player->currentWeaponIndex];
}

void player_free(Player *player) {
    if (player != NULL) free(player);
    printf("Player freed\n");
}
