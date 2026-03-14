#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h>
#include "game/player.h"

//  Maps SDL keyboard/window events to player actions and movement states
void input_handle_event(SDL_Event *event, Player *player);

#endif // INPUT_H
