//
// Created by alan pitcher on 11/22/2025.
//
#ifndef PLAYER_H
#define PLAYER_H
typedef struct {
    double posX;
    double posY;
    double dirX;
    double dirY;
    double planeX;
    double planeY;
    double movSpeed;
    double rotSpeed;
    int isMovingForward;
    int isMovingBackward;
    int isRotatingLeft;
    int isRotatingRight;
} Player;

void player_Init(Player *player);
void player_update(Player *player, double frameTime);
void move_player_forward(Player *player, double distance);
void move_player_backward(Player *player, double distance);
void rotate_player_right(Player *player, double rotAngle);
void rotate_player_left(Player *player, double rotAngle);


#endif //PLAYER_H
