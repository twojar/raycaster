//
// Created by Alan Pitcher on 1/13/2026.
//
#include "gamestate.h"
#include <stdlib.h>

//  Init the game state defaults
void gamestate_init(GameState *state) {
    if (state == NULL) return;
    state->mode = STATE_PLAYING; // Default to playing for now
    state->objectivesTotal = 0;
    state->objectivesDone = 0;
    state->difficulty = DIFF_NORMAL;
    
    // Initialize input state to zero
    state->input.up = 0;
    state->input.down = 0;
    state->input.left = 0;
    state->input.right = 0;
    state->input.rotateLeft = 0;
    state->input.rotateRight = 0;
    state->input.sprint = 0;
    state->input.mouseXRel = 0.0f;
}

//  Updates the current gameplay mode
void gamestate_set_mode(GameState *state, GameMode mode) {
    if (state == NULL) return;
    state->mode = mode;
}

//  Updates the session difficulty level
void gamestate_set_difficulty(GameState *state, Difficulty difficulty) {
    if (state == NULL) return;
    state->difficulty = difficulty;
}

//  Sets the total number of objectives required to win the level
//  The concept of objectives is just temporary while I figure out what gameloop to do lol
void gamestate_set_objectives_total(GameState *state, int total) {
    if (state == NULL) return;
    state->objectivesTotal = total;
}

//  Increments the count of completed objectives and checks for win condition
void gamestate_increment_objective(GameState *state) {
    if (state == NULL) return;
    state->objectivesDone++;
    
    // Check if player has won the level
    if (state->objectivesTotal > 0 && state->objectivesDone >= state->objectivesTotal) {
        state->mode = STATE_WIN;
    }
}
