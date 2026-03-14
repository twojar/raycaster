//
// Created by Alan Pitcher on 11/22/2025.
//
#ifndef AUDIO_H
#define AUDIO_H
#include <SDL3/SDL.h>

//  Initializes the audio subsystem and loads default sounds
void audio_init();

//  Loads and starts playing background music from the given path
void audio_play_music(char* path);

//  Plays a randomized footstep sound effect
void audio_play_footstep();

//  Updates audio state (used for volume fading or music transitions)
void audio_update_music();

//  Cleans up audio resources and shuts down the subsystem
void audio_free();

#endif //AUDIO_H
