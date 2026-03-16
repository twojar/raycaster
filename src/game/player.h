//
// Created by alan pitcher on 11/22/2025.
//
#ifndef PLAYER_H
#define PLAYER_H

#include "input.h"

//  Camera and collision data for the player-controlled view
typedef struct {
    double posX, posY;    //  Current grid position
    double dirX, dirY;    //  Current direction vector
    double planeX, planeY;//  Current camera plane for FOV
    
    double prevPosX, prevPosY;    //  Previous grid position
    double prevDirX, prevDirY;    //  Previous direction vector
    double prevPlaneX, prevPlaneY;//  Previous camera plane for FOV
    double movSpeed;
    double rotSpeed;

    double footstepTimer;
    double health;
} Player;

//  Sets default values for FOV, health, and starting speeds
void player_init(Player *player);

//  Forced movement (e.g. at map start)
void player_teleport(Player *player, double posX, double posY);

//  Updates position, rotation, and animation timers for the current frame
void player_update(Player *player, InputState *input, double frameTime);

//  Directional movement handlers with built-in collision detection
void player_move_forward(Player *player, double distance);
void player_move_backward(Player *player, double distance);
void player_move_left(Player *player, double distance);
void player_move_right(Player *player, double distance);

//  Camera rotation handlers
void player_rotate_right(Player *player, double rotAngle);
void player_rotate_left(Player *player, double rotAngle);

//  Cleanup (placeholder for future player-specific heap memory)
void player_free(Player *player);

#endif //PLAYER_H
