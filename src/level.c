#include "level.h"
#include "entity.h"

static unsigned int tileData[ARRAY_SIZE];
static unsigned int objectData[ARRAY_SIZE];
static char sTileFlags[NUM_TILES];
static short chunkId = 0;
static float sAnimTimer = 0.0f;
static int sAnimOffset = 0;

void LevelInit(const char* level) {
    SDL_memset(tileData, 0, ARRAY_SIZE);
    SDL_memset(objectData, 0, ARRAY_SIZE);
    SDL_memset(sTileFlags, 0, NUM_TILES);
    LevelLoad(level);
    LevelSelectRoom(0);

    //load tile properties - from file later 
    sTileFlags[1] = TILE_FLAG_ANIM | TILE_FLAG_LOW;
    sTileFlags[2] = TILE_FLAG_ANIM | TILE_FLAG_LOW;
    sTileFlags[79] = TILE_FLAG_HIGH;
    sTileFlags[148] = TILE_FLAG_HIGH;
    sTileFlags[149] = TILE_FLAG_HIGH;
    sTileFlags[178] = TILE_FLAG_MID;
    sTileFlags[216] = TILE_FLAG_ANIM | TILE_FLAG_MID;
    sTileFlags[529] = TILE_FLAG_HIGH | TILE_FLAG_BREAKABLE;

}

void LevelSave(const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(tileData, sizeof(int), ARRAY_SIZE, file);
        fclose(file);
    }
}

void LevelLoad(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file != NULL) {
        fread(tileData, sizeof(int), ARRAY_SIZE, file);
        fread(objectData, sizeof(int), ARRAY_SIZE, file);
        fclose(file);
    }
}

void LevelSelectRoom(int id) {
    /*
    # Store the active chunk  ID so the level knows where it is
    self.chunkId = chunk_id
    calc chunk from the id
    note: start loading rooms from file rather than storing them all in memory?
    */
    chunkId = id;
    printf("room id= %d \n", chunkId);
    if (chunkId > LEVEL_SIZE - 1 || chunkId < 0) {
        chunkId = 0;
        return;
    }
    EntityClearAll();
    int i = 0;
    int chunk_offset = CHUNK_SIZE * id;
    for (int r = 0; r < CHUNK_HEIGHT; r++) {
        for (int c = 0; c < CHUNK_WIDTH; c++) {
            i = objectData[r * CHUNK_WIDTH + c + chunk_offset];// + CHUNK_SIZE
            //Do not spawn dummy or player objects
            if (i < 2)
                continue;
            Entity* ent = EntitySpawn(c*TILE_SIZE, r * TILE_SIZE, i);
            //load tileData
        }
    }
}
int LevelGetRoomId(void) {
    return chunkId;
}
short LevelFindAdjectId(short dir) {
    //up, right, down, left
    switch (dir) {
    case 0: return (chunkId - LEVEL_WIDTH) < 0 ? 0 : chunkId - LEVEL_WIDTH;
    case 1: return (chunkId + 1 < LEVEL_SIZE) ? chunkId + 1: LEVEL_SIZE-1;
    case 2: return (chunkId + LEVEL_WIDTH > LEVEL_SIZE-1) ? LEVEL_SIZE-1 : chunkId + LEVEL_WIDTH;
    case 3: return (chunkId - 1 > 0) ? chunkId - 1 : 0;
    }
    return 0;
}

void LevelDraw(void) {
    unsigned  int i = 0;
    unsigned  int offset = chunkId * CHUNK_SIZE;

    for (unsigned int r = 0; r < CHUNK_HEIGHT;  r++){
        for (unsigned int c = 0; c < CHUNK_WIDTH; c++){
            i = tileData[r * CHUNK_WIDTH + c + offset];
            ImageDrawTile(c * 16, r * 16, TEX_ATLAS, i + (sAnimOffset*!!(sTileFlags[i] & TILE_FLAG_ANIM)));
        }
    }
}

char LevelGetTileFlags(int x, int y) {
    int c = x >> 4;
    int r = y >> 4;
    int local_id = r * CHUNK_WIDTH + c;
    int offset = chunkId * CHUNK_SIZE;
    return sTileFlags[tileData[local_id + offset]];
}

int LevelGetTileId(int x, int y) {
    int c = x >> 4;
    int r = y >> 4;
    int local_id = r * CHUNK_WIDTH + c;
    int offset = chunkId * CHUNK_SIZE;
    return tileData[local_id + offset];
}

void LevelSetTileId(int x, int y, int id) {
    int c = x >> 4;
    int r = y >> 4;
    int local_id = r * CHUNK_WIDTH + c;
    int offset = chunkId * CHUNK_SIZE;
    tileData[local_id + offset] = id;
}

void LevelSetObjectId(int x, int y, int id) {
    int c = x >> 4;
    int r = y >> 4;
    int local_id = r * CHUNK_WIDTH + c;
    int offset = chunkId * CHUNK_SIZE;
    objectData[local_id + offset] = id;
}

int LevelGetObjectId(int x, int y) {
    int c = x >> 4;
    int r = y >> 4;
    int local_id = r * CHUNK_WIDTH + c;
    int offset = chunkId * CHUNK_SIZE;
    return objectData[local_id + offset];
}

void LevelUpdate(float dt) {
    sAnimTimer += dt;
    if (sAnimTimer > ANIM_TILE_RATE) {
        sAnimTimer = 0.0f;
        sAnimOffset++;
        if (sAnimOffset > NUM_ANIM_TILE_FRAMES) {
            sAnimOffset = 0;
        }
    }
}