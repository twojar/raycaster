//
// Created by Alan Pitcher on 11/22/2025.
//
#ifndef AUDIO_H
#define AUDIO_H
#include <SDL3/SDL.h>

void audio_Init();
void play_music(char* path);
void play_footstep();
void update_music();
void audio_free();

#endif //AUDIO_H
