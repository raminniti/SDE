#include "game.h"
#include "engine.h"
#include "entity.h"
#include "level.h"
#include "data.h"
#include "player.h"
#include "projectile.h"
#include "vfx.h"
#include "hud.h"

static GameState currentState = STATE_TITLE;

void GameInit(void) {
    DatabaseLoadAssets();
    EntityManagerInit();
    //init player first to be at index[0], so we can clear everything when changing rooms. could also have isPlayer bool
    PlayerInit(10, 5);
    LevelInit("assets/levels/0.bin");
    ProjectileInit();
    VfxInit();

    // Set state
    currentState = STATE_GAMEPLAY;
}

void GameRestart(void) {
    LevelSelectRoom(0);
    currentState = STATE_GAMEPLAY;
    gPlayer->health = gPlayer->data->maxHealth;
    gPlayer->frame = gPlayer->data->frameStart;
    gPlayer->playingAnim = true;
}

void GameUpdate(void) {
    float dt = ClockGetDeltaTime();
    if (InputIsKeyPressed(SDL_SCANCODE_ESCAPE)) WindowQuitMainLoop();
    if (InputIsKeyPressed(SDLK_q))
        WindowQuitMainLoop();
    if (InputIsKeyPressed(SDLK_l))
        WindowToggleFullscreen();

    switch (currentState) {
    case STATE_TITLE:
        if (InputIsKeyPressed(SDL_SCANCODE_RETURN)) {
            // Transition logic here
            currentState = STATE_GAMEPLAY;
        }
        break;

    case STATE_GAMEPLAY: {
        // Update Game Objects (AI, Animations, Life Cycles)
        LevelUpdate(dt);
        EntityUpdateAll(dt);
        ProjectileUpdateAll(dt);
        VfxUpdateAll(dt);

        EntityHandleAllCollisions();
        ProjectileHandleAllCollisions(EntityGetPool(),EntityGetActiveCount());
        PlayerUpdate(dt);

        //Game Over
        if (gPlayer->health < 1)
            currentState = STATE_GAMEOVER;

        //debug
        if (InputIsKeyPressed(SDLK_p)){
            printf("Fps: %f \n", ClockGetFps());
        }

        if (InputIsMousePressed(1)) {
            int mx, my, scale;
            mx = 0;
            my = 0;
            scale = WindowGetScale();
            SDL_GetMouseState(&mx, &my);
            // It's scale/2, wire up vars later
            printf("Tileid= %d \n", LevelGetTileId((mx + scale / 2) / scale, (my + scale / 2) / scale));
        }

        //change rooms
        //TODO: refactor and add actual player dimensions later. move to levelupdate?
        if (gPlayer->pos.x < 1){
            LevelSelectRoom(LevelFindAdjectId(3));
            ProjectileDestroyAll();
            gPlayer->pos.x = CHUNK_WIDTH * 16 - 18;
        }
        else if (gPlayer->pos.x+16 > CHUNK_WIDTH*16-2){
            LevelSelectRoom(LevelFindAdjectId(1));
            ProjectileDestroyAll();
            gPlayer->pos.x = 2;
        }
        else if (gPlayer->pos.y+16 > CHUNK_HEIGHT * 16-2){
            LevelSelectRoom(LevelFindAdjectId(2));
            ProjectileDestroyAll();
            gPlayer->pos.y = 2;
        }
        else if (gPlayer->pos.y < 1){
            LevelSelectRoom(LevelFindAdjectId(0));
            ProjectileDestroyAll();
            gPlayer->pos.y = CHUNK_HEIGHT * 16 - 18;
        }
    }
        
    case STATE_GAMEOVER:
        ImageDrawText(48, 120, TEX_GUI, "Game Over - press 'r' to restart");
        if (InputIsKeyPressed(SDLK_r)) {
            GameRestart(); // reset
        }
        break;
    }
}

void GameDraw(void) {
    switch (currentState) {
    case STATE_TITLE:
        // Draw title background image or text
        ImageDrawText(64, 80, TEX_GUI, "Demon Knight");
        ImageDrawText(48, 120, TEX_GUI, "Press Enter to Start");
        break;

    case STATE_GAMEPLAY:

        LevelDraw();   // Layer 1 - Floors, Paths, Water
        EntityDrawAll();          // Layer 2 - Player, Enemies, Items (Y-Sorted?)
        ProjectileDrawAll();
        VfxDrawAll();
        //LevelDrawForeground();  // Layer 3 - Overhead door frames, tree tops
        HudDraw(0,152);              // Layer 4 - UI

#ifdef DEBUG_MODE
        int offset_x= 0;
        int offset_y = 0;

        char debug_buf[64];

        // Format and draw debug info
        snprintf(debug_buf, sizeof(debug_buf), "FPS: %d", ClockGetFps());
        ImageDrawText(0+offset_x, 0+offset_y, TEX_GUI, debug_buf);

        snprintf(debug_buf, sizeof(debug_buf), "Entities: %d / %d",
            EntityGetActiveCount(), MAX_ENTITIES);
        ImageDrawText(0 + offset_x, 8 + offset_y, TEX_GUI, debug_buf);

        snprintf(debug_buf, sizeof(debug_buf), "Projectiles: %d / %d",
            ProjectileGetActiveCount(), MAX_PROJECTILES);
        ImageDrawText(0 + offset_x, 16 + offset_y, TEX_GUI, debug_buf);

        snprintf(debug_buf, sizeof(debug_buf), "Vfx: %d / %d",
            VfxGetActiveCount(), MAX_VFX);
        ImageDrawText(0 + offset_x, 24 + offset_y, TEX_GUI, debug_buf);

        snprintf(debug_buf, sizeof(debug_buf), "RoomId: %d", LevelGetRoomId());
        ImageDrawText(0 + offset_x, 32 + offset_y, TEX_GUI, debug_buf);
#endif // DEBUG
        break;

    case STATE_GAMEOVER:
        ImageDrawText(96, 90, TEX_GUI, "GAME OVER");
        break;
    }
}

void GameQuit(void) {
    //LevelQuit();
    //EntityManagerQuit();
    //DataQuit();
}

GameState GameGetState(void) { return currentState; }
void GameSetState(GameState newState) { currentState = newState; }


