#include "game/input.h"
#include <stdio.h>
#include "engine/graphics.h"
#include "game/gamestate.h"

extern GameState *g_gamestate;

void input_handle_event(SDL_Event *event, InputState *input) {
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    input->up = 1;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    input->down = 1;
                    break;
                case SDL_SCANCODE_A:
                    input->left = 1;
                    break;
                case SDL_SCANCODE_D:
                    input->right = 1;
                    break;
                case SDL_SCANCODE_LEFT:
                    input->rotateLeft = 1;
                    break;
                case SDL_SCANCODE_RIGHT:
                    input->rotateRight = 1;
                    break;
                case SDL_SCANCODE_LSHIFT:
                    input->sprint = 1;
                    break;
                case SDL_SCANCODE_C:
                    //debug
                    break;
            }
            break;

        case SDL_EVENT_KEY_UP:
            switch (event->key.scancode) {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    input->up = 0;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    input->down = 0;
                    break;
                case SDL_SCANCODE_A:
                    input->left = 0;
                    break;
                case SDL_SCANCODE_D:
                    input->right = 0;
                    break;
                case SDL_SCANCODE_LEFT:
                    input->rotateLeft = 0;
                    break;
                case SDL_SCANCODE_RIGHT:
                    input->rotateRight = 0;
                    break;
                case SDL_SCANCODE_LSHIFT:
                    input->sprint = 0;
                    break;
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:
            if (g_gamestate && g_gamestate->mode != STATE_PLAYING) break;
            input->mouseXRel += event->motion.xrel;
            break;
    }
}
