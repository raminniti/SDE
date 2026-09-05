#include "clock.h"

// Time stuff
//static double msPerFrame = 1.0 / 60;
//static unsigned int intervalTime = 0;
static double timer = 0.0;
static double prevTime = 0.0;
static double curTime = 0.0;
static double tick = 0.0;
static double deltaTime = 0.0;
static unsigned short fps = 0;
static unsigned short frameCount = 0;

void ClockInit()
{}

int ClockGetFps(){
    return fps;
}

float ClockGetDeltaTime(){
	return (float)deltaTime;
}

void ClockTick(float target_fps) {
    float target_ms = 1000.0f / target_fps;
    
    curTime = SDL_GetTicks();
    uint32_t frame_ticks = curTime - prevTime;
    
    // Cap the frame rate (Delay only the remaining time)
    if (frame_ticks < target_ms) {
        SDL_Delay((uint32_t)(target_ms - frame_ticks));
        // Recalculate curTime after the delay for accurate deltaTime
        curTime = SDL_GetTicks();
    }

    // Calculate deltaTime
    deltaTime = (curTime - prevTime) / 1000.0f;
    
    // Safety Cap to prevent "teleporting"
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    prevTime = curTime;
    
    //Optional- track FPS
    frameCount++;
    timer += deltaTime;

    if (timer > 1.0f) {
        fps = frameCount;
        timer = 0;
        frameCount = 0;
    }
}