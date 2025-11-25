//
// Created by Alan Pitcher on 11/22/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
static SDL_AudioDeviceID audioDevice = 0;
static SDL_AudioStream *stream = NULL;

static Uint8 *wavBuffer = NULL;
static Uint32 wavLength = 0;
static SDL_AudioSpec wavSpec;

void audio_Init() {
    audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (audioDevice == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
    }
}

void play_music(char* path) {
    if (wavBuffer != NULL) {
        SDL_free(wavBuffer);
        wavBuffer = NULL;
    }
    if (stream != NULL) {
        SDL_DestroyAudioStream(stream);
    }

    if (!SDL_LoadWAV(path, &wavSpec, &wavBuffer, &wavLength)) {
        printf("Failed to load wav %s\n", path);
        return;
    }

    stream = SDL_CreateAudioStream(&wavSpec, NULL);
    if (stream == NULL) {
        printf("Failed to create audio stream: %s\n", SDL_GetError());
        return;
    }

    SDL_BindAudioStream(audioDevice, stream);
    SDL_PutAudioStreamData(stream, wavBuffer, wavLength);
    SDL_ResumeAudioDevice(audioDevice);
}


void update_music() {
    if (stream == NULL || wavBuffer == NULL) return;
    int bytesLeft = SDL_GetAudioStreamAvailable(stream);
    //repeat when over
    if (bytesLeft == 0) {
        SDL_PutAudioStreamData(stream, wavBuffer, wavLength);
    }
}
void free_audio() {
    if (stream) SDL_DestroyAudioStream(stream);
    if (wavBuffer) SDL_free(wavBuffer);
    if (audioDevice) SDL_CloseAudioDevice(audioDevice);
}