#ifndef CLOCK_H
#define CLOCK_H

#include <SDL.h>

//clock stuff
void ClockInit();
int ClockGetFps();
float ClockGetDeltaTime();
void ClockTick(float d_fps);

#endif