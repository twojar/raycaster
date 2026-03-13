#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h>
#include "game/player.h"

void handle_input_event(SDL_Event *event, Player *player);

#endif // INPUT_H
