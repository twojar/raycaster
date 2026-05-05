#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <SDL3/SDL.h>

#define MOUSE_SENSITIVITY 0.0025

// Tracks the "pressed/released" status of keys and mouse delta
typedef struct {
    int up, down, left, right;
    int rotateLeft, rotateRight;
    int sprint;
    bool firePressed;
    bool reloadPressed;
    bool pausePressed;
    bool quitRequested;
    float mouseXRel;
} InputState;

// Initializes all input state to defaults
void input_init(InputState *input);

// Maps SDL keyboard/window events to InputState
void input_handle_event(SDL_Event *event, InputState *input);

// Clears one-shot action flags after the game loop consumes them
void input_clear_transient(InputState *input);

#endif // INPUT_H
