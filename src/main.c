#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <time.h>
#include "engine/graphics.h"
#include "engine/draw2d.h"
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
static float g_pendingMouseXRel = 0.0f;
static bool g_pendingFirePressed = false;
static bool g_pendingReloadPressed = false;

static void draw_weapon_hud(Player *player) {
    Weapon *weapon = player_get_current_weapon(player);
    if (weapon == NULL) return;

    char ammoText[32];
    const int panelWidth = 72;
    const int panelHeight = 28;
    const int panelX = SCREEN_WIDTH - panelWidth - 8;
    const int panelY = SCREEN_HEIGHT - panelHeight - 8;
    const Uint32 panelFill = 0xAA000000;
    const Uint32 panelBorder = 0xFFFFFFFF;
    const Uint32 ammoColor = 0xFFFFFF00;

    SDL_snprintf(ammoText, sizeof(ammoText), "%02d %02d", weapon->ammoInMag, weapon->reserveAmmo);

    draw2d_rect(panelX, panelY, panelWidth, panelHeight, panelFill);
    draw2d_rect_outline(panelX, panelY, panelWidth, panelHeight, panelBorder);
    draw2d_text_scaled(ammoText, panelX + 8, panelY + 6, ammoColor, 0.5f);
}

static void sync_relative_mouse_mode(void) {
    bool captureMouse = (g_gamestate != NULL && g_gamestate->mode == STATE_PLAYING);
    SDL_SetWindowRelativeMouseMode(g_window, captureMouse);
}

static void toggle_pause(void) {
    if (g_gamestate == NULL) return;

    if (g_gamestate->mode == STATE_PLAYING) {
        gamestate_set_mode(g_gamestate, STATE_PAUSED);
        printf("Game Paused\n");
    } else if (g_gamestate->mode == STATE_PAUSED) {
        gamestate_set_mode(g_gamestate, STATE_PLAYING);
        printf("Game Resumed\n");
    }

    g_gamestate->input.mouseXRel = 0.0f;
    g_pendingMouseXRel = 0.0f;
    g_pendingFirePressed = false;
    g_pendingReloadPressed = false;
    sync_relative_mouse_mode();
}

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

    g_player = (Player *)malloc(sizeof(Player));
    if (!g_player) return SDL_APP_FAILURE;
    player_init(g_player);

    g_gamestate = (GameState *)malloc(sizeof(GameState));
    if (!g_gamestate) return SDL_APP_FAILURE;
    gamestate_init(g_gamestate);
    sync_relative_mouse_mode();

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

    audio_play_music("../assets/audio/bgm/sign_of_evil_good.wav");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    input_handle_event(event, &g_gamestate->input);

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

    if (g_gamestate->input.quitRequested) {
        return SDL_APP_SUCCESS;
    }

    if (g_gamestate->input.pausePressed) {
        toggle_pause();
    }

    double frameTime = get_delta_time();
    g_pendingMouseXRel += g_gamestate->input.mouseXRel;
    g_pendingFirePressed = g_pendingFirePressed || g_gamestate->input.firePressed;
    g_pendingReloadPressed = g_pendingReloadPressed || g_gamestate->input.reloadPressed;

    g_gamestate->input.mouseXRel = 0.0f;
    input_clear_transient(&g_gamestate->input);

    // Cap frameTime to prevent large jumps during lag
    if (frameTime > 0.1) frameTime = 0.1;
    accumulator += frameTime;
    int updatesToRun = (int)(accumulator / dt);
    float mouseXRelPerTick = updatesToRun > 0 ? (g_pendingMouseXRel / (float)updatesToRun) : 0.0f;

    while (accumulator >= dt) {
        switch (g_gamestate->mode) {
            case STATE_PLAYING:
                // Save previous state for interpolation
                g_player->prevPosX = g_player->posX;
                g_player->prevPosY = g_player->posY;
                g_player->prevDirX = g_player->dirX;
                g_player->prevDirY = g_player->dirY;
                g_player->prevPlaneX = g_player->planeX;
                g_player->prevPlaneY = g_player->planeY;

                for (int i = 0; i < g_numSprites; i++) {
                    g_sprites[i].prevX = g_sprites[i].x;
                    g_sprites[i].prevY = g_sprites[i].y;
                }

                InputState tickInput = g_gamestate->input;
                tickInput.mouseXRel = mouseXRelPerTick;
                tickInput.firePressed = g_pendingFirePressed;
                tickInput.reloadPressed = g_pendingReloadPressed;

                player_update(g_player, &tickInput, dt);
                entity_update_scent_map(g_player, dt);
                
                if (entity_update_all(dt) == SDL_APP_SUCCESS) {
                    g_gamestate->mode = STATE_DEAD;
                }
                break;

            case STATE_PAUSED:
                // Do nothing in update loop when paused
                break;

            case STATE_DEAD:
            case STATE_WIN:
                return SDL_APP_SUCCESS; // Exit for now

            default:
                break;
        }
        accumulator -= dt;
    }

    if (updatesToRun > 0) {
        g_pendingMouseXRel = 0.0f;
        g_pendingFirePressed = false;
        g_pendingReloadPressed = false;
    }

    double alpha = accumulator / dt;
    // Create an interpolated player for rendering
    Player interpolatedPlayer = *g_player;
    if (g_gamestate->mode == STATE_PLAYING || g_gamestate->mode == STATE_PAUSED) {
        interpolatedPlayer.posX = g_player->posX * alpha + g_player->prevPosX * (1.0 - alpha);
        interpolatedPlayer.posY = g_player->posY * alpha + g_player->prevPosY * (1.0 - alpha);
        interpolatedPlayer.dirX = g_player->dirX * alpha + g_player->prevDirX * (1.0 - alpha);
        interpolatedPlayer.dirY = g_player->dirY * alpha + g_player->prevDirY * (1.0 - alpha);
        interpolatedPlayer.planeX = g_player->planeX * alpha + g_player->prevPlaneX * (1.0 - alpha);
        interpolatedPlayer.planeY = g_player->planeY * alpha + g_player->prevPlaneY * (1.0 - alpha);
    }

    gfx_draw_frame(g_renderer, &interpolatedPlayer, alpha);

    // Draw UI Text
    if (g_gamestate->mode == STATE_PAUSED) {
        // "PAUSED" is 6 chars. At 24px spacing, that's ~144px wide
        // SCREEN_WIDTH/2 (160) - 72 = 88
        gfx_draw_text("PAUSED", SCREEN_WIDTH/2 - 72, SCREEN_HEIGHT/2 - 16, 0xFFFFFF00); // Yellow
    }

    if (g_gamestate->mode == STATE_PLAYING || g_gamestate->mode == STATE_PAUSED) {
        draw_weapon_hud(g_player);
    }
    
    char debugInfo[64];
    //SDL_snprintf(debugInfo, sizeof(debugInfo), "FPS: %.0f", 1.0 / frameTime);
    //gfx_draw_text(debugInfo, 10, 10, 0xFFFFFFFF); // White

    audio_update_music();
    gfx_present(g_renderer);
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
