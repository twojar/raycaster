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
#include "entity.h"
#include "sprite.h"
#include "maptile.h"

#define WINDOW_TITLE "abdadbadabdadbadabdad"
#define MOUSE_SENSITIVITY 0.002

SDL_Window *window;
SDL_Renderer *renderer;
Player *player;


size_t CHUNK_SIZE;
void **blocks;
size_t count;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init: %s\n", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT,0,&window, &renderer)) {
        printf("SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        printf("SDL_Init: %s\n", SDL_GetError());
    }

    SDL_SetWindowRelativeMouseMode(window,true);

    player = (Player *)malloc(sizeof(Player));

    int mapArgExists = 0;
    spriteDataExists = 0;

    if (argc == 2) {
        mapArgExists = 1;
        spriteDataExists = 0;
    }
    if (argc > 2) {
        mapArgExists = 1;
        spriteDataExists = 1;
    }

    // ../levels/floor1.SAMD
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Wrong amount of args! argv[1] = *.SAMD, argv[2] = *.SPRITEDATA \n");
        return SDL_APP_FAILURE;
    }


    player_Init(player);

    if (spriteDataExists) load_sprites(argv[2]);
    //if (mapArgExists) load_map(argv[1]);
    //else random_map(player);
    random_map(player);

    init_Graphics(renderer);
    audio_Init();

    if (spriteDataExists) {
        entity_Init(player, sprites);
    }

    play_music("../assets/audio/nightmare_haven.wav");


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
                    //play_music("../assets/audio/Myuu.wav");
                    break;
                case SDL_SCANCODE_C:
                    printf("Player Position: (%d, %d) \n", (int)player->posX, (int)player->posY);
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
            //printf("%f\n", mouseRotation);
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
    update_scentMap(player);
    update_music();
    SDL_RenderPresent(renderer);

    if (spriteDataExists) {
        SDL_AppResult entityResult = entities_update(frameTime);
        if (entityResult == SDL_APP_SUCCESS) return entityResult;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    player_free(player);
    worldMap_free();
    audio_free();
    scentMap_free();
    sprites_free();
    entities_free();
}