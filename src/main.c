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
        printf("No map file provided. Generating random map...\n");
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

    audio_play_music("../assets/audio/bgm/divine_drone.wav");

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

double get_delta_time() {
    static Uint64 lastTick = 0;
    Uint64 currentTick = SDL_GetPerformanceCounter();
    if (lastTick == 0) lastTick = currentTick;
    
    double delta = (double)(currentTick - lastTick) / (double)SDL_GetPerformanceFrequency();
    lastTick = currentTick;
    return delta;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const double dt = 1.0 / 60.0;
    static double accumulator = 0.0;
    
    double frameTime = get_delta_time();

    // Cap frameTime to prevent large jumps during lag
    if (frameTime > 0.1) frameTime = 0.1;
    accumulator += frameTime;

    while (accumulator >= dt) {
        if (g_gamestate->mode == STATE_PLAYING) {
            // Save previous state for interpolation
            g_player->prevPosX = g_player->posX;
            g_player->prevPosY = g_player->posY;
            g_player->prevDirX = g_player->dirX;
            g_player->prevDirY = g_player->dirY;
            g_player->prevPlaneX = g_player->planeX;
            g_player->prevPlaneY = g_player->planeY;

            player_update(g_player, dt);
            entity_update_scent_map(g_player, dt);
            
            SDL_AppResult entityResult = entity_update_all(dt);
            if (entityResult == SDL_APP_SUCCESS) {
                //  Death state
                g_gamestate->mode = STATE_DEAD;
            }
        }
        accumulator -= dt;
    }

    if (g_gamestate->mode == STATE_DEAD) {
        //  For now, just exit on death
        return SDL_APP_SUCCESS;
    } else if (g_gamestate->mode == STATE_WIN) {
        //  For now, just exit on win
        return SDL_APP_SUCCESS;
    }

    double alpha = accumulator / dt;
    // Create an interpolated player for rendering
    Player interpolatedPlayer = *g_player;
    if (g_gamestate->mode == STATE_PLAYING) {
        interpolatedPlayer.posX = g_player->posX * alpha + g_player->prevPosX * (1.0 - alpha);
        interpolatedPlayer.posY = g_player->posY * alpha + g_player->prevPosY * (1.0 - alpha);
        
        // Direction vectors need careful interpolation (or just use current if rotation is fast)
        // For simplicity, we'll lerp them, though slerp is technically better for rotations
        interpolatedPlayer.dirX = g_player->dirX * alpha + g_player->prevDirX * (1.0 - alpha);
        interpolatedPlayer.dirY = g_player->dirY * alpha + g_player->prevDirY * (1.0 - alpha);
        interpolatedPlayer.planeX = g_player->planeX * alpha + g_player->prevPlaneX * (1.0 - alpha);
        interpolatedPlayer.planeY = g_player->planeY * alpha + g_player->prevPlaneY * (1.0 - alpha);
    }

    gfx_draw_frame(g_renderer, &interpolatedPlayer);
    audio_update_music();
    SDL_RenderPresent(g_renderer);

    return SDL_APP_CONTINUE;
}

void walk_em_down_and_free_they_memory() {
    if (g_player) {
        free(g_player);
        g_player = NULL;
    }
    if (g_gamestate) {
        free(g_gamestate);
        g_gamestate = NULL;
    }
    map_free();
    audio_free();
    sprite_free();
    entity_free();

    if (g_renderer) {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = NULL;
    }
    if (g_window) {
        SDL_DestroyWindow(g_window);
        g_window = NULL;
    }
    
    SDL_Quit();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    walk_em_down_and_free_they_memory();
    SDL_Log("Application exiting safely.");
}
