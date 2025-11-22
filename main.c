#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "graphics.h"
#include "player.h"

#define WINDOW_TITLE "adbadabdabdabdadbadabdabdad"

SDL_Window *window;
SDL_Renderer *renderer;

Player *player;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init: %s\n", SDL_GetError());
    }
    if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT,0,&window, &renderer)) {
        printf("SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    player = (Player *)malloc(sizeof(Player));
    player_Init(player);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    return SDL_APP_SUCCESS;
                case SDL_SCANCODE_W:
                    //move forward
                    player->isMovingForward = 1;
                    break;
                case SDL_SCANCODE_S:
                    //move backward:
                        player->isMovingBackward = 1;
                        break;
                case SDL_SCANCODE_A:
                    //move left;
                        break;
                case SDL_SCANCODE_D:
                    //move right;
                        break;
                case SDL_SCANCODE_UP:
                    //also move forward
                        player->isMovingForward = 1;
                        break;
                case SDL_SCANCODE_DOWN:
                    //also move backward
                        player->isMovingBackward = 1;
                        break;
                case SDL_SCANCODE_LEFT:
                    //turn left
                        player->isRotatingLeft = 1;
                        break;
                case SDL_SCANCODE_RIGHT:
                    //turn right;
                        player->isRotatingRight = 1;
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
                    //player->isMovingLeft = 0;
                    break;
                case SDL_SCANCODE_D:
                    //player->isMovingRight = 0;
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
            }

        break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    static Uint64 lastTick = 0;
    Uint64 currentTick = SDL_GetTicks();
    double frameTime = (double) (currentTick - lastTick) / 1000.0;
    lastTick = currentTick;
    player_update(player, frameTime);
    draw_frame(renderer, player);
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    free(player);
}