#include "audio.h"

static Mix_Chunk* sounds[MAX_SOUNDS] = { 0 };
static Mix_Music* music = nullptr;
static signed char volume = 128; // Max volume is 128

void AudioInit() {
	Mix_AllocateChannels(8);
}

void AudioSetVolumeSound(int vol){
	Mix_Volume(-1, vol);
}

void AudioSetVolumeMusic(int vol){
	Mix_VolumeMusic(vol);
}

void AudioSetVolume(int vol){
	Mix_VolumeMusic(vol);
	Mix_Volume(-1, vol);
}

void AudioLoadSound(const char * fname, unsigned int id){
	assert(id < MAX_SOUNDS);

	if (sounds[id] == nullptr){
		sounds[id] = Mix_LoadWAV(fname);
		if (sounds[id] == nullptr)
			printf("Failed to load %s sound effect! SDL_mixer Error: %s\n", fname, Mix_GetError());
	}
}

void AudioLoadMusic(const char* fname){
	if (music == nullptr){
		//Load music
		music = Mix_LoadMUS(fname);
		if (music == nullptr)
			printf("Failed to load %s music! SDL_mixer Error: %s\n", fname, Mix_GetError());
	}
	else{
		//Free up memory from old music
		Mix_FreeMusic(music);

		//Load music
		music = Mix_LoadMUS(fname);
	}

}

void AudioPlaySound(unsigned short id)
{
	assert(id < MAX_SOUNDS);
	if (sounds[id] == nullptr)
	{
		printf("PlaySound(): Trying to play sound that doesn't exist id: %d", id);
		return;
	}
	//Play sound - (channel, sound, repeat)
	Mix_PlayChannel(-1, sounds[id], 0);
}

void AudioPlayMusic(){
	if (Mix_PlayingMusic() == 0){
		Mix_PlayMusic(music, -1);
	}
}

void AudioPauseMusic(){
	if (Mix_PausedMusic() == 1){
		Mix_ResumeMusic();
	}
	else{
		Mix_PauseMusic();
	}
}

void AudioStopMusic(){
	Mix_HaltMusic();
}

void AudioQuit(){
	//Free up memory from sounds
	for (unsigned char i = 0; i < MAX_SOUNDS; ++i){
		if (sounds[i] != nullptr){
			Mix_FreeChunk(sounds[i]);
			sounds[i] = nullptr;
		}
	}

	//Free up memory from music
	Mix_FreeMusic(music);

	//Quit SDL subsystems
	Mix_Quit();
}