#include "src/game.h"

int main(int argc, char* args[])
{
	//Init engine and game
	EngineInit("Game", 1280, 720);// maybe 320, 180 or 640, 360 -  960, 540?
	GameInit(); 
	
	//--------------------------------------------------------
	// Main loop
	//--------------------------------------------------------
	while (EngineIsRunning()){
		EngineUpdate();
		GameUpdate();
		EngineRenderStart();
		GameDraw();
		EngineRenderEnd();
	}
	GameQuit();
	EngineQuit();
	return 0;
}
