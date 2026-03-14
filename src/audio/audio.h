//
// Created by Alan Pitcher on 11/22/2025.
//
#ifndef AUDIO_H
#define AUDIO_H
#include <SDL3/SDL.h>

void audio_init();
void audio_play_music(char* path);
void audio_play_footstep();
void audio_update_music();
void audio_free();

#endif //AUDIO_H
