//
// Created by Alan Pitcher on 11/22/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>



static SDL_AudioDeviceID audioDevice = 0;
static SDL_AudioStream *musicStream = NULL;
static SDL_AudioStream *sfxStream = NULL;

static Uint8 *musicWavBuffer = NULL;
static Uint32 musicWavLength = 0;
static SDL_AudioSpec musicWavSpec;

#define NUM_FOOTSTEPS 9
static Uint8 *footstepBuffers[NUM_FOOTSTEPS];
static Uint32 footstepLengths[NUM_FOOTSTEPS];
static SDL_AudioSpec footstepSpec;



void audio_Init() {
    audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (audioDevice == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
    }

    for (int i = 0; i < NUM_FOOTSTEPS; i++) {
        char path[256];
        sprintf(path, "../audio/footsteps/%d.wav",i);

        if (!SDL_LoadWAV(path, &footstepSpec, &footstepBuffers[i], &footstepLengths[i])) {
            printf("Failed to load wav %s %s \n", path, SDL_GetError());
        }
    }
    sfxStream = SDL_CreateAudioStream(&footstepSpec,NULL);
    if (!sfxStream) {
        printf("Failed to create audio stream: %s \n", SDL_GetError());
        return;
    }

    if (SDL_BindAudioStream(audioDevice, sfxStream) == -1) {
        printf("Failed to bind audio stream: %s \n", SDL_GetError());
    }

    srand(time(NULL));
    SDL_ResumeAudioDevice(audioDevice);
}

void play_footstep() {
    if (sfxStream == NULL) return;
    int r = rand() % NUM_FOOTSTEPS;
    if (footstepBuffers[r] != NULL) {
        SDL_PutAudioStreamData(sfxStream,footstepBuffers[r], footstepLengths[r]);
    }
}

void play_music(char* path) {
    if (musicWavBuffer != NULL) {
        SDL_free(musicWavBuffer);
        musicWavBuffer = NULL;
    }
    if (musicStream != NULL) {
        SDL_DestroyAudioStream(musicStream);
    }

    if (!SDL_LoadWAV(path, &musicWavSpec, &musicWavBuffer, &musicWavLength)) {
        printf("Failed to load wav %s\n", path);
        return;
    }

    musicStream = SDL_CreateAudioStream(&musicWavSpec, NULL);
    if (musicStream == NULL) {
        printf("Failed to create audio stream: %s\n", SDL_GetError());
        return;
    }

    SDL_BindAudioStream(audioDevice, musicStream);
    SDL_PutAudioStreamData(musicStream, musicWavBuffer, musicWavLength);
    SDL_ResumeAudioDevice(audioDevice);
}


void update_music() {
    if (musicStream == NULL || musicWavBuffer == NULL) return;
    int bytesLeft = SDL_GetAudioStreamAvailable(musicStream);
    //repeat when over
    if (bytesLeft == 0) {
        SDL_PutAudioStreamData(musicStream, musicWavBuffer, musicWavLength);
    }
}
void free_audio() {
    if (musicStream) SDL_DestroyAudioStream(musicStream);
    if (musicWavBuffer) SDL_free(musicWavBuffer);
    if (audioDevice) SDL_CloseAudioDevice(audioDevice);
    if (sfxStream) SDL_DestroyAudioStream(sfxStream);
    for (int i = 0; i < NUM_FOOTSTEPS; i++) {
        if (footstepBuffers[i] != NULL) SDL_free(footstepBuffers[i]);
    }
    SDL_CloseAudioDevice(audioDevice);
}