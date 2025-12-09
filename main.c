#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <time.h>
#include "graphics.h"
#include "player.h"
#include "audio.h"
#include "sprite.h"

#define WINDOW_TITLE "adbadabdabdabdadbadabdabdad"
#define MOUSE_SENSITIVITY 0.002

SDL_Window *window;
SDL_Renderer *renderer;
Player *player;

size_t CHUNK_SIZE;
void **blocks;
size_t count;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

    /*
    // Purposeful memory leak
    CHUNK_SIZE = 10 * 1024 * 1024; // 10 MB per allocation
    blocks = NULL;
    count = 0;
    */

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init: %s\n", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT,SDL_WINDOW_FULLSCREEN,&window, &renderer)) {
        printf("SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        printf("SDL_Init: %s\n", SDL_GetError());
    }

    SDL_SetWindowRelativeMouseMode(window,true);

    player = (Player *)malloc(sizeof(Player));

    // ../levels/floor1.SAMD
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Wrong amount of args! argv[1] = *.SAMD, argv[2] = *.SPRITEDATA \n");
        return SDL_APP_FAILURE;
    }

    load_map(argv[1]);
    load_sprites(argv[2]);

    player_Init(player);
    init_Graphics(renderer);
    audio_Init();
    play_music("../audio/nightmare_haven.wav");

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
                    player->isMovingLeft = 1;
                    break;
                case SDL_SCANCODE_D:
                    player->isMovingRight = 1;
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
                case SDL_SCANCODE_LSHIFT:
                    player->isSprinting = 1;
                    break;
                case SDL_SCANCODE_M:
                    //play_music("../audio/Myuu.wav");
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
                rotate_player_right(player, mouseRotation * MOUSE_SENSITIVITY);
            }
            else if (mouseRotation < 0) {
                rotate_player_left(player, -mouseRotation * MOUSE_SENSITIVITY);
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
    double fps = 1/frameTime;
    //printf("FPS: %f\n", fps);
    player_update(player, frameTime);
    draw_frame(renderer, player);
    update_music();
    SDL_RenderPresent(renderer);

    /*
    //memory leak
    void **new_blocks = realloc(blocks, (count + 1) * sizeof(void *));
    blocks = new_blocks;
    void *mem = malloc(CHUNK_SIZE);
    memset(mem, 0xAA, CHUNK_SIZE);
    blocks[count] = mem;
    count++;
    */

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    free(player);
    free_audio();
    free(worldMap);
    free(sprites);
}