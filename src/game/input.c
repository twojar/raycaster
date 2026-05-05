#include "game/input.h"
#include <stddef.h>

void input_init(InputState *input) {
    if (input == NULL) return;

    input->up = 0;
    input->down = 0;
    input->left = 0;
    input->right = 0;
    input->rotateLeft = 0;
    input->rotateRight = 0;
    input->sprint = 0;
    input->firePressed = false;
    input->reloadPressed = false;
    input->pausePressed = false;
    input->quitRequested = false;
    input->mouseXRel = 0.0f;
}

void input_handle_event(SDL_Event *event, InputState *input) {
    if (event == NULL || input == NULL) return;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            input->quitRequested = true;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event->key.repeat) break;

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
                case SDL_SCANCODE_R:
                    input->reloadPressed = true;
                    break;
                case SDL_SCANCODE_TAB:
                    input->pausePressed = true;
                    break;
                case SDL_SCANCODE_ESCAPE:
                    input->quitRequested = true;
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

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                input->firePressed = true;
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:
            input->mouseXRel += event->motion.xrel;
            break;
    }
}

void input_clear_transient(InputState *input) {
    if (input == NULL) return;

    input->firePressed = false;
    input->reloadPressed = false;
    input->pausePressed = false;
}
