#ifndef AUDIO_H
#define AUDIO_H

#include <assert.h>
#include <SDL_mixer.h>
#include <SDL.h>
#include "config.h"

//audio stuff
void AudioSetVolumeSound(int vol);
void AudioSetVolumeMusic(int vol);
void AudioSetVolume(int vol);
void AudioLoadSound(const char * fname, unsigned int id);
void AudioLoadMusic(const char* fname);
void AudioPlaySound(unsigned short id);
void AudioPlayMusic();
void AudioPauseMusic();
void AudioStopMusic();
void AudioInit();
void AudioQuit();

#endif