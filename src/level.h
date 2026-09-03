#ifndef LEVEL_H
#define LEVEL_H

#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.h"

#define CHUNK_WIDTH 15
#define CHUNK_HEIGHT 9
#define CHUNK_SIZE CHUNK_WIDTH*CHUNK_HEIGHT
#define LEVEL_WIDTH 8
#define LEVEL_HEIGHT 8
#define LEVEL_SIZE LEVEL_HEIGHT*LEVEL_WIDTH
#define ARRAY_SIZE CHUNK_SIZE*LEVEL_SIZE
#define NUM_ANIM_TILE_FRAMES 2-1
#define ANIM_TILE_RATE 0.30f

typedef enum {
	TILE_FLAG_GROUND = 0,
	TILE_FLAG_LOW = 1, // pit or water collision
	TILE_FLAG_MID = 2, // rocks and decos
	TILE_FLAG_HIGH = 4,// unpassable, like walls
	TILE_FLAG_ANIM = 8, // animated, but capped at 2 frames for now
	TILE_FLAG_SPECIAL = 16, // special, like interactable, lighting, etc
	TILE_FLAG_DAMAGING = 32, // does damage, like lava or spikes
	TILE_FLAG_BREAKABLE = 64, // projectiles can break it
	TILE_FLAG_SPLODEABLE = 128// only blast damage breaks it
}TileFlags;

void LevelInit(const char* level);
void LevelSave(const char* filename);
void LevelLoad(const char* filename);
void LevelSelectRoom(int id);
int LevelGetRoomId(void);
int LevelGetTileId(int x, int y);
void LevelSetTileId(int x, int y, int id);
void LevelSetObjectId(int x, int y, int id);
int LevelGetObjectId(int x, int y);
void LevelUpdate(float dt);
void LevelDraw(void);
short LevelFindAdjectId(short dir);
char LevelGetTileFlags(int x, int y);
#endif
