#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h>

#define MOUSE_SENSITIVITY 0.002

// Tracks the "pressed/released" status of keys and mouse delta
typedef struct {
    int up, down, left, right;
    int rotateLeft, rotateRight;
    int sprint;
    float mouseXRel;
} InputState;

// Maps SDL keyboard/window events to InputState
void input_handle_event(SDL_Event *event, InputState *input);

#endif // INPUT_H
