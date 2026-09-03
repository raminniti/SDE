#include "window.h"

//static unsigned char state = 0;
static unsigned short windowWidth = 640;
static unsigned short windowHeight = 360;
static int sScale = 3;
static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Event event;
static bool running = true;
static bool windowed = true;

void WindowToggleFullscreen(){
	if (true == windowed){
		windowed = false;
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else{
		windowed = true;
		SDL_SetWindowFullscreen(window, 0);
	}
}

bool WindowIsRunning(){
	return running;
}

void WindowQuitMainLoop(){
	running = false;
}

void WindowInitialize(unsigned short w, unsigned short h, const char * title, int scale){
	bool success = true;
	running = true;
	window = nullptr;
	renderer = nullptr;
	sScale = scale;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0){
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else{
		//Initialize SDL_mixer
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}

		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN );// | SDL_WINDOW_FULLSCREEN_DESKTOP
		if (window == NULL){
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else{
			//Get window surface
			//SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &sdlWindow, &sdlRenderer);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);// | SDL_RENDERER_PRESENTVSYNC);// | SDL_RENDERER_TARGETTEXTURE);
			if (renderer == NULL){
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else{
				//Initialize renderer color
				SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0x0);//(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
		//SDL_Texture* canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_RENDERER_TARGETTEXTURE);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		SDL_RenderSetLogicalSize(renderer, (int)(w / sScale), (int)(h / sScale));//SDL_RenderSetScale(renderer, sScale, sScale);
		//Mix_Init(MIX_INIT_OGG)
	}
}

void WindowQuit(){
	//Destroy window
	if (window) {
		SDL_SetWindowFullscreen(window, 0);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
	
	//Quit SDL subsystems
	SDL_Quit();

	window = NULL;
	renderer = NULL;
}

void WindowUpdate()
{
	//Draw renderer to window
	SDL_RenderPresent(renderer);
}

void WindowClear()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//Draw renderer to window
	SDL_RenderClear(renderer);
}

SDL_Renderer* WindowGetRenderer()
{
	return renderer;
}

unsigned char WindowGetScale() {
	return sScale;
}
