#include "game/player.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine/graphics.h"
#include "audio/audio.h"

// Boundary around the player for collision detection
#define PLAYER_BOUNDARY 0.1

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
    player->movSpeed = 2.0;
    player->rotSpeed = 4.0;
    player->footstepTimer = 0.0;
    player->health = 100.0;
}

//  debug purposes
void player_teleport(Player *player, double posX, double posY) {
    player->posX = posX;
    player->posY = posY;
    printf("Player teleported to (%d,%d)\n",(int) player->posX,(int) player->posY);
}

//  runs every frame
void player_update(Player *player, InputState *input, double frameTime) {
    double distance = player->movSpeed * frameTime;
    double rotAngle = player->rotSpeed * frameTime;

    if (input->up) player_move_forward(player, distance);
    if (input->down) player_move_backward(player, distance);
    if (input->left) player_move_left(player, distance);
    if (input->right) player_move_right(player, distance);
    if (input->rotateRight) player_rotate_right(player, rotAngle);
    if (input->rotateLeft) player_rotate_left(player, rotAngle);

    // Mouse rotation
    if (input->mouseXRel != 0) {
        if (input->mouseXRel > 0) {
            player_rotate_right(player, input->mouseXRel * MOUSE_SENSITIVITY);
        } else {
            player_rotate_left(player, -input->mouseXRel * MOUSE_SENSITIVITY);
        }
        // Note: Resetting mouseXRel should happen after ALL fixed-step updates in the frame
        // or we can consume it here. If we consume it here, and there are multiple fixed steps,
        // it only applies to the first one. That's actually usually desired for mouse delta.
        input->mouseXRel = 0;
    }

    if (input->sprint) {
        player->movSpeed = 4.0;
    }
    else {
        player->movSpeed = 2.0;
    }

    int isMoving = input->up || input->down || input->left || input->right;
    if (isMoving) {
        player->footstepTimer -= frameTime;
        if (player->footstepTimer <= 0) {
            audio_play_footstep();
            if (input->sprint) {
                player->footstepTimer = 0.25;
            } else {
                player->footstepTimer = 0.45;
            }
        }
    }
    else {
        player->footstepTimer = 0;
    }
    //printf("%f,%f\n",player->posX,player->posY);
}

void player_move_forward(Player *player, double distance) {

    double newPosX = player->posX + player->dirX * distance;
    double newPosY = player->posY + player->dirY * distance;

    double hitboxX = (player->dirX > 0) ? (newPosX + PLAYER_BOUNDARY) : (newPosX - PLAYER_BOUNDARY);
    if (g_worldMap[(int) player->posY * g_mapCols + (int) hitboxX].textureId == 0) player->posX = newPosX;

    double hitboxY = (player->dirY > 0) ? (newPosY + PLAYER_BOUNDARY) : (newPosY - PLAYER_BOUNDARY);
    if (g_worldMap[ (int) hitboxY * g_mapCols + (int) player->posX].textureId == 0) player->posY = newPosY;
}

void player_move_backward(Player *player, double distance) {
    double newPosX = player->posX - player->dirX * distance;
    double newPosY = player->posY - player->dirY * distance;

    double hitboxX = (player->dirX > 0) ? (newPosX - PLAYER_BOUNDARY) : (newPosX + PLAYER_BOUNDARY);
    if (g_worldMap[(int) player->posY * g_mapCols + (int) hitboxX].textureId == 0) player->posX = newPosX;

    double hitboxY = (player->dirY > 0) ? (newPosY - PLAYER_BOUNDARY) : (newPosY + PLAYER_BOUNDARY);
    if (g_worldMap[(int) hitboxY * g_mapCols + (int)player->posX].textureId == 0) player->posY = newPosY;
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
    double newPosX = player->posX - (player->planeX) * distance;
    double newPosY = player->posY - (player->planeY) * distance;

    double hitboxX = (player->planeX > 0) ? (newPosX - PLAYER_BOUNDARY) : (newPosX + PLAYER_BOUNDARY);
    if (g_worldMap[(int) player->posY * g_mapCols + (int) hitboxX].textureId == 0) player->posX = newPosX;

    double hitboxY = (player->planeY > 0) ? (newPosY - PLAYER_BOUNDARY) : (newPosY + PLAYER_BOUNDARY);
    if (g_worldMap[(int) hitboxY * g_mapCols + (int)player->posX].textureId == 0) player->posY = newPosY;
}

void player_move_right(Player *player, double distance) {
    double newPosX = player->posX + player->planeX * distance;
    double newPosY = player->posY + player->planeY * distance;

    double hitboxX = (player->planeX > 0) ? (newPosX + PLAYER_BOUNDARY) : (newPosX - PLAYER_BOUNDARY);
    if (g_worldMap[(int) player->posY * g_mapCols + (int) hitboxX].textureId == 0) player->posX = newPosX;

    double hitboxY = (player->planeY > 0) ? (newPosY + PLAYER_BOUNDARY) : (newPosY - PLAYER_BOUNDARY);
    if (g_worldMap[(int) hitboxY * g_mapCols + (int) player->posX].textureId == 0) player->posY = newPosY;
}

void player_free(Player *player) {
    if (player != NULL) free(player);
    printf("Player freed\n");
}




