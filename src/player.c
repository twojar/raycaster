#include "player.h"
#include <math.h>
#include <stdio.h>
#include "graphics.h"
#include "audio.h"

#define P_BOUNDARY 0.1

void player_Init(Player *player) {
    player->posX = 12;
    player->posY = 20;
    player->dirX = -1;
    player->dirY = 0;
    player->planeX = 0;
    player->planeY = 0.66;
    player->movSpeed = 2.0;
    player->rotSpeed = 4.0;
    player->isMovingForward = 0;
    player->isMovingBackward = 0;
    player->isMovingLeft = 0;
    player->isMovingRight = 0;
    player->isRotatingLeft = 0;
    player->isRotatingRight = 0;
    player->isSprinting = 0;
    player->footsteptimer = 0.0;
}

void player_update(Player *player, double frameTime) {
    double distance = player->movSpeed * frameTime;
    double rotAngle = player->rotSpeed * frameTime;
    if (player->isMovingForward == 1) move_player_forward(player, distance);
    if (player->isMovingBackward == 1) move_player_backward(player, distance);
    if (player->isMovingLeft == 1) move_player_left(player, distance);
    if (player->isMovingRight == 1) move_player_right(player, distance);
    if (player->isRotatingRight == 1) rotate_player_right(player, rotAngle);
    if (player->isRotatingLeft == 1) rotate_player_left(player, rotAngle);

    if (player->isSprinting == 1) {
        player->movSpeed = 4.0;
    }
    else {
        player->movSpeed = 2.0;
    }

    int isMoving = player->isMovingForward || player->isMovingBackward || player->isMovingLeft || player->isMovingRight;
    if (isMoving == 1) {
        player->footsteptimer -= frameTime;
        if (player->footsteptimer <= 0) {
            play_footstep();
            if (player->isSprinting == 1) {
                player->footsteptimer = 0.3;
            } else {
                player->footsteptimer = 0.5;
            }
        }
    }
    else {
        player->footsteptimer = 0;
    }
   // printf("%f,%f\n",player->posX,player->posY);
}

void move_player_forward(Player *player, double distance) {

    double newPosX = player->posX + player->dirX * distance;
    double newPosY = player->posY + player->dirY * distance;

    double hitboxX = (player->dirX > 0) ? (newPosX + P_BOUNDARY) : (newPosX - P_BOUNDARY);
    if (worldMap[(int) player->posY * mapCols + (int) hitboxX] == 0) player->posX = newPosX;

    double hitboxY = (player->dirY > 0) ? (newPosY + P_BOUNDARY) : (newPosY - P_BOUNDARY);
    if (worldMap[ (int) hitboxY * mapCols + (int) player->posX] == 0) player->posY = newPosY;
}

void move_player_backward(Player *player, double distance) {
    double newPosX = player->posX - player->dirX * distance;
    double newPosY = player->posY - player->dirY * distance;

    double hitboxX = (player->dirX > 0) ? (newPosX - P_BOUNDARY) : (newPosX + P_BOUNDARY);
    if (worldMap[(int) player->posY * mapCols + (int) hitboxX] == 0) player->posX = newPosX;

    double hitboxY = (player->dirY > 0) ? (newPosY - P_BOUNDARY) : (newPosY + P_BOUNDARY);
    if (worldMap[(int) hitboxY * mapCols + (int)player->posX] == 0) player->posY = newPosY;
}

void rotate_player_right(Player *player, double rotAngle) {
    double oldDirX = player->dirX;
    player->dirX = player->dirX * cos(-rotAngle) - player->dirY * sin(-rotAngle);
    player->dirY = oldDirX * sin(-rotAngle) + player->dirY * cos(-rotAngle);

    double oldPlaneX = player->planeX;
    player->planeX = player->planeX * cos(-rotAngle) - player->planeY * sin(-rotAngle);
    player->planeY = oldPlaneX * sin(-rotAngle) + player->planeY * cos(-rotAngle);
}

void rotate_player_left(Player *player, double rotAngle) {
    double oldDirX = player->dirX;
    player->dirX = player->dirX * cos(rotAngle) - player->dirY * sin(rotAngle);
    player->dirY = oldDirX * sin(rotAngle) + player->dirY * cos(rotAngle);

    double oldPlaneX = player->planeX;
    player->planeX = player->planeX * cos(rotAngle) - player->planeY * sin(rotAngle);
    player->planeY = oldPlaneX * sin(rotAngle) + player->planeY * cos(rotAngle);
}

void move_player_left(Player *player, double distance) {
    double newPosX = player->posX - (player->planeX) * distance;
    double newPosY = player->posY - (player->planeY) * distance;

    double hitboxX = (player->planeX > 0) ? (newPosX - P_BOUNDARY) : (newPosX + P_BOUNDARY);
    if (worldMap[(int) player->posY * mapCols + (int)newPosX] == 0) player->posX = newPosX;

    double hitboxY = (player->planeY > 0) ? (newPosY - P_BOUNDARY) : (newPosY + P_BOUNDARY);
    if (worldMap[(int) hitboxY * mapCols + (int)player->posX] == 0) player->posY = newPosY;
}

void move_player_right(Player *player, double distance) {
    double newPosX = player->posX + player->planeX * distance;
    double newPosY = player->posY + player->planeY * distance;

    double hitboxX = (player->planeX > 0) ? (newPosX + P_BOUNDARY) : (newPosX - P_BOUNDARY);
    if (worldMap[(int) player->posY * mapCols + (int) hitboxX] == 0) player->posX = newPosX;

    double hitboxY = (player->planeY > 0) ? (newPosY + P_BOUNDARY) : (newPosY - P_BOUNDARY);
    if (worldMap[(int) hitboxY * mapCols + (int) player->posX] == 0) player->posY = newPosY;
}




