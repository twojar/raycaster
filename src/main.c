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
#include "game/gamestate.h"

#define WINDOW_TITLE "Engine"

SDL_Window *g_window;
SDL_Renderer *g_renderer;
Player *g_player;
GameState *g_gamestate;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    srand((unsigned int)time(NULL));

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0, &g_window, &g_renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetWindowPosition(g_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowRelativeMouseMode(g_window, true);

    g_player = (Player *)malloc(sizeof(Player));
    if (!g_player) return SDL_APP_FAILURE;
    player_init(g_player);

    g_gamestate = (GameState *)malloc(sizeof(GameState));
    if (!g_gamestate) return SDL_APP_FAILURE;
    gamestate_init(g_gamestate);

    // Logic for loading or generating content
    bool mapLoaded = false;
    bool spritesLoaded = false;

    //  arg parsing
    for (int i = 1; i < argc; i++) {
        if ((SDL_strcmp(argv[i], "-m") == 0 || SDL_strcmp(argv[i], "--map") == 0) && i + 1 < argc) {
            map_load(argv[++i]);
            mapLoaded = true;
        } else if ((SDL_strcmp(argv[i], "-s") == 0 || SDL_strcmp(argv[i], "--sprites") == 0) && i + 1 < argc) {
            sprite_load(argv[++i]);
            spritesLoaded = true;
        } else if ((SDL_strcmp(argv[i], "-d") == 0 || SDL_strcmp(argv[i], "--difficulty") == 0) && i + 1 < argc) {
            char *diffArg = argv[++i];
            if (SDL_strcasecmp(diffArg, "easy") == 0) gamestate_set_difficulty(g_gamestate, DIFF_EASY);
            else if (SDL_strcasecmp(diffArg, "normal") == 0) gamestate_set_difficulty(g_gamestate, DIFF_NORMAL);
            else if (SDL_strcasecmp(diffArg, "hard") == 0) gamestate_set_difficulty(g_gamestate, DIFF_HARD);
            else if (SDL_strcasecmp(diffArg, "nightmare") == 0) gamestate_set_difficulty(g_gamestate, DIFF_NIGHTMARE);
            printf("Difficulty set to: %s\n", diffArg);
        }
    }

    // Fallback to random generation if needed
    if (!mapLoaded) {
        printf("No map file provided. Generating random maze...\n");
        map_generate_random(g_player);
    }

    if (!spritesLoaded) {
        printf("No sprite data provided. Generating random entities...\n");
        // We ensure a minimum number of sprites are available for entities
        entity_create_random(g_gamestate->difficulty); 
        g_spriteDataExists = 1;
    }

    gfx_init(g_renderer);
    audio_init();

    // Initialize entity system
    entity_init(g_player, g_sprites);

    audio_play_music("../assets/audio/nightmare_haven.wav");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.scancode == SDL_SCANCODE_ESCAPE) {
        return SDL_APP_SUCCESS;
    }

    input_handle_event(event, g_player);
    
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

    if (g_gamestate->mode == STATE_PLAYING) {
        player_update(g_player, frameTime);
        entity_update_scent_map(g_player, frameTime);
        
        SDL_AppResult entityResult = entity_update_all(frameTime);
        if (entityResult == SDL_APP_SUCCESS) {
            //  Death state
            g_gamestate->mode = STATE_DEAD;
        }
    } else if (g_gamestate->mode == STATE_DEAD) {
        //  For now, just exit on death
        return SDL_APP_SUCCESS;
    } else if (g_gamestate->mode == STATE_WIN) {
        //  For now, just exit on win
        return SDL_APP_SUCCESS;
    }

    gfx_draw_frame(g_renderer, g_player);
    audio_update_music();
    SDL_RenderPresent(g_renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    if (g_player) free(g_player);
    if (g_gamestate) free(g_gamestate);
    map_free();
    audio_free();
    sprite_free();
    entity_free();
    SDL_Log("Application exiting safely.");
}
