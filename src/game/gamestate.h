//
// Created by Alan Pitcher on 1/13/2026.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

//  Main game logic loops and UI context
typedef enum {
    STATE_MENU,     //  Initial UI
    STATE_EDITOR,   //  MAP EDITOR TOOLS
    STATE_PLAYING,  //  NORMAL GAMEPLAY
    STATE_PAUSED,   //  UI + FREEZE WORLD STATE
    STATE_DEAD,     //  DEATH SCREEN, RESTART / MENU
    STATE_WIN,      //  WIN SCREEN, RESTART / MENU
}GameMode;

//  Difficulty presets
typedef enum {
    DIFF_EASY,
    DIFF_NORMAL,
    DIFF_HARD,
    DIFF_NIGHTMARE,
    DIFF_NA
} Difficulty;

//  Persistent tracker for global session logic and progress
typedef struct {
    GameMode mode;
    int objectivesTotal;
    int objectivesDone;
    Difficulty difficulty;
}GameState;

//  Initializes the game state defaults and sets mode to MENU or PLAYING
void gamestate_init(GameState *state);

//  Updates the current gameplay mode (e.g. Menu, Playing, Dead)
void gamestate_set_mode(GameState *state, GameMode mode);

//  Updates the session difficulty level
void gamestate_set_difficulty(GameState *state, Difficulty difficulty);

//  Sets the total number of objectives required to win the level
void gamestate_set_objectives_total(GameState *state, int total);

//  Increments the count of completed objectives and checks for win condition
void gamestate_increment_objective(GameState *state);

#endif //GAMESTATE_H
