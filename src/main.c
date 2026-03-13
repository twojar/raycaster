#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <time.h>
#include "engine/graphics.h"
#include "game/player.h"
#include "audio/audio.h"
#include "game/entity.h"
#include "engine/sprite.h"
#include "game/map.h"
#include "game/input.h"

#define WINDOW_TITLE "Raycaster Engine"

SDL_Window *window;
SDL_Renderer *renderer;
Player *player;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    srand((unsigned int)time(NULL));

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowRelativeMouseMode(window, true);

    player = (Player *)malloc(sizeof(Player));
    if (!player) return SDL_APP_FAILURE;
    player_Init(player);

    // Logic for loading or generating content
    bool mapLoaded = false;
    bool spritesLoaded = false;

    if (argc >= 2) {
        // Attempt to load map if provided
        load_map(argv[1]);
        mapLoaded = true;
    }

    if (argc >= 3) {
        // Attempt to load sprites if provided
        load_sprites(argv[2]);
        spritesLoaded = true;
    }

    // Fallback to random generation if needed
    if (!mapLoaded) {
        printf("No map file provided. Generating random maze...\n");
        random_map(player);
    }

    if (!spritesLoaded) {
        printf("No sprite data provided. Generating random entities...\n");
        // We ensure a minimum number of sprites are available for entities
        create_random_entities(); 
        spriteDataExists = 1;
    }

    init_Graphics(renderer);
    audio_Init();

    // Initialize entity system
    entity_Init(player, sprites);

    play_music("../assets/audio/nightmare_haven.wav");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.scancode == SDL_SCANCODE_ESCAPE) {
        return SDL_APP_SUCCESS;
    }

    handle_input_event(event, player);
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    static Uint64 lastTick = 0;
    Uint64 currentTick = SDL_GetTicks();
    if (lastTick == 0) lastTick = currentTick;
    
    double frameTime = (double)(currentTick - lastTick) / 1000.0;
    lastTick = currentTick;

    // Cap frameTime to prevent large jumps during lag
    if (frameTime > 0.1) frameTime = 0.1;

    player_update(player, frameTime);
    update_scentMap(player, frameTime);
    
    SDL_AppResult entityResult = entities_update(frameTime);
    if (entityResult == SDL_APP_SUCCESS) return entityResult;

    draw_frame(renderer, player);
    update_music();
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    player_free(player);
    worldMap_free();
    audio_free();
    sprites_free();
    entities_free();
    SDL_Log("Application exiting safely.");
}
