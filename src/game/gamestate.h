//
// Created by Alan Pitcher on 1/13/2026.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

typedef enum {
    //  UI
    STATE_MENU,

    //  MAP EDITOR TOOLS
    STATE_EDITOR,

    //  NORMAL GAMEPLAY
    STATE_PLAYING,

    //  UI + FREEZE WORLD STATE
    STATE_PAUSED,

    //  DEATH SCREEN, RESTART / MENU
    STATE_DEAD,

    //  WIN SCREEN, RESTART / MENU
    STATE_WIN,
}GameMode;

typedef enum {
    DIFF_EASY,
    DIFF_NORMAL,
    DIFF_HARD,
    DIFF_NIGHTMARE,
} Difficulty;

//  GameState struct
//  for keeping track of general gameplay logic
typedef struct {
    GameMode mode;
    int objectivesTotal;
    int objectivesDone;
}GameState;

#endif //GAMESTATE_H
