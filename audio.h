//
// Created by Alan Pitcher on 11/22/2025.
//
#ifndef AUDIO_H
#define AUDIO_H
#include <SDL3/SDL.h>

void audio_Init();
void play_music(char* path);
void update_music();
void free_audio();

#endif //AUDIO_H
