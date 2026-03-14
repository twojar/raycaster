//
// Created by Alan Pitcher on 11/22/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

#define NUM_FOOTSTEPS 9

static SDL_AudioDeviceID g_audioDevice = 0;
static SDL_AudioStream *g_musicStream = NULL;
static SDL_AudioStream *g_sfxStream = NULL;

static Uint8 *g_musicWavBuffer = NULL;
static Uint32 g_musicWavLength = 0;
static SDL_AudioSpec g_musicWavSpec;


static Uint8 *g_footstepBuffers[NUM_FOOTSTEPS];
static Uint32 g_footstepLengths[NUM_FOOTSTEPS];
static SDL_AudioSpec g_footstepSpec;

void audio_init() {
    g_audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (g_audioDevice == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
    }

    for (int i = 0; i < NUM_FOOTSTEPS; i++) {
        char path[256];
        sprintf(path, "../assets/audio/footsteps/%d.wav",i);

        if (!SDL_LoadWAV(path, &g_footstepSpec, &g_footstepBuffers[i], &g_footstepLengths[i])) {
            printf("Failed to load wav %s %s \n", path, SDL_GetError());
        }
    }
    g_sfxStream = SDL_CreateAudioStream(&g_footstepSpec,NULL);
    if (!g_sfxStream) {
        printf("Failed to create audio stream: %s \n", SDL_GetError());
        return;
    }

    if (SDL_BindAudioStream(g_audioDevice, g_sfxStream) == -1) {
        printf("Failed to bind audio stream: %s \n", SDL_GetError());
    }

    srand(time(NULL));
    SDL_ResumeAudioDevice(g_audioDevice);
}

void audio_play_footstep() {
    if (g_sfxStream == NULL) return;
    int r = rand() % NUM_FOOTSTEPS;
    if (g_footstepBuffers[r] != NULL) {
        SDL_PutAudioStreamData(g_sfxStream,g_footstepBuffers[r], g_footstepLengths[r]);
    }
}

void audio_play_music(char* path) {
    if (g_musicWavBuffer != NULL) {
        SDL_free(g_musicWavBuffer);
        g_musicWavBuffer = NULL;
    }
    if (g_musicStream != NULL) {
        SDL_DestroyAudioStream(g_musicStream);
    }

    if (!SDL_LoadWAV(path, &g_musicWavSpec, &g_musicWavBuffer, &g_musicWavLength)) {
        printf("Failed to load wav %s\n", path);
        return;
    }

    g_musicStream = SDL_CreateAudioStream(&g_musicWavSpec, NULL);
    if (g_musicStream == NULL) {
        printf("Failed to create audio stream: %s\n", SDL_GetError());
        return;
    }

    SDL_BindAudioStream(g_audioDevice, g_musicStream);
    SDL_PutAudioStreamData(g_musicStream, g_musicWavBuffer, g_musicWavLength);
    SDL_ResumeAudioDevice(g_audioDevice);
}


void audio_update_music() {
    if (g_musicStream == NULL || g_musicWavBuffer == NULL) return;
    int bytesLeft = SDL_GetAudioStreamAvailable(g_musicStream);
    //repeat when over
    if (bytesLeft == 0) {
        SDL_PutAudioStreamData(g_musicStream, g_musicWavBuffer, g_musicWavLength);
    }
}

void audio_free() {
    if (g_musicStream) SDL_DestroyAudioStream(g_musicStream);
    if (g_musicWavBuffer) SDL_free(g_musicWavBuffer);
    if (g_audioDevice) SDL_CloseAudioDevice(g_audioDevice);
    if (g_sfxStream) SDL_DestroyAudioStream(g_sfxStream);
    for (int i = 0; i < NUM_FOOTSTEPS; i++) {
        if (g_footstepBuffers[i] != NULL) SDL_free(g_footstepBuffers[i]);
    }
    printf("Audio freed\n");
}