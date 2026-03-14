#include "game/input.h"
#include "engine/graphics.h"

#define MOUSE_SENSITIVITY 0.002

void input_handle_event(SDL_Event *event, Player *player) {
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.scancode) {
                case SDL_SCANCODE_W:
                    player->isMovingForward = 1;
                    break;
                case SDL_SCANCODE_S:
                    player->isMovingBackward = 1;
                    break;
                case SDL_SCANCODE_A:
                    player->isMovingLeft = 1;
                    break;
                case SDL_SCANCODE_D:
                    player->isMovingRight = 1;
                    break;
                case SDL_SCANCODE_UP:
                    player->isMovingForward = 1;
                    break;
                case SDL_SCANCODE_DOWN:
                    player->isMovingBackward = 1;
                    break;
                case SDL_SCANCODE_LEFT:
                    player->isRotatingLeft = 1;
                    break;
                case SDL_SCANCODE_RIGHT:
                    player->isRotatingRight = 1;
                    break;
                case SDL_SCANCODE_LSHIFT:
                    player->isSprinting = 1;
                    break;
                case SDL_SCANCODE_C:
                    printf("Player Position: (%.2f, %.2f) \n", player->posX, player->posY);
                    break;
            }
            break;

        case SDL_EVENT_KEY_UP:
            switch (event->key.scancode) {
                case SDL_SCANCODE_W:
                    player->isMovingForward = 0;
                    break;
                case SDL_SCANCODE_S:
                    player->isMovingBackward = 0;
                    break;
                case SDL_SCANCODE_A:
                    player->isMovingLeft = 0;
                    break;
                case SDL_SCANCODE_D:
                    player->isMovingRight = 0;
                    break;
                case SDL_SCANCODE_UP:
                    player->isMovingForward = 0;
                    break;
                case SDL_SCANCODE_DOWN:
                    player->isMovingBackward = 0;
                    break;
                case SDL_SCANCODE_LEFT:
                    player->isRotatingLeft = 0;
                    break;
                case SDL_SCANCODE_RIGHT:
                    player->isRotatingRight = 0;
                    break;
                case SDL_SCANCODE_LSHIFT:
                    player->isSprinting = 0;
                    break;
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:
            float mouseRotation = event->motion.xrel;
            if (mouseRotation > 0) {
                player_rotate_right(player, mouseRotation * MOUSE_SENSITIVITY);
            }
            else if (mouseRotation < 0) {
                player_rotate_left(player, -mouseRotation * MOUSE_SENSITIVITY);
            }
            break;
    }
}
