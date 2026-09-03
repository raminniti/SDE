#include "data.h"
#include "ai.h" //just need for enums

void DatabaseLoadAssets(void) {
    // Load Graphic Atlas Sheets
    ImageLoadTilesheet("images/atlas.bmp", TEX_ATLAS, 16, 16);
    ImageLoadTilesheet("images/gui.bmp", TEX_GUI, 8, 8);

    // Music & Sounds
    AudioLoadMusic("audio/euo.mp3");
    AudioLoadSound("audio/Hit.wav", SND_HIT);
    AudioLoadSound("audio/bfxrShoot.ogg", SND_SHOOT);
    AudioLoadSound("audio/mdeath.wav", SND_DEATH);
    AudioLoadSound("audio/mpain.wav", SND_HURT);
    AudioLoadSound("audio/step.wav", SND_STEP);
    AudioLoadSound("audio/pickup.wav",SND_PICKUP);
    AudioLoadSound("audio/break.wav", SND_BREAK);

    // Configure
    AudioSetVolume(10);
    AudioPlayMusic();
}

struct EntityBlueprint entityDatabase[ENT_COUNT] = {
    // ENT_DUMMY
    {
        .type = TYPE_STATIC, .ai = AI_NONE, .faction = FACTION_NEUTRAL,
        .maxHealth = 0, .touchDamage = 0, .speed = 0,
        .width = 16, .height = 16, .radius = 8, .rotateSprite = false,
        .projectileId = 0, .frameStart = 0, .frameAttack = 0, .numFrames = 0, .animRate = 0.0f
    },
    // ENT_PLAYER
    {
        .type = TYPE_CREATURE, .ai = AI_NONE, .faction = FACTION_PLAYER,
        .maxHealth = 100, .touchDamage = 0, .speed = 1,
        .width = 15, .height = 15, .radius = 8, .rotateSprite = false,
        .projectileId = 0, .frameStart = 320, .frameAttack = 8, .numFrames = 4, .animRate = 0.12f
    },
    // ENT_ZOMBIE
    {
        .type = TYPE_CREATURE, .ai = AI_CHASE_PLAYER,.faction = FACTION_ENEMY,
        .maxHealth = 30, .touchDamage = 10, .speed = 1,
        .width = 15, .height = 15, .radius = 8, .rotateSprite = false,
        .projectileId = 0, .frameStart = 484, .frameAttack = 16, .numFrames = 4, .animRate = 0.16f
    },
    // ENT_SOUL 
    {
        .type = TYPE_PICKUP, .ai = AI_NONE,
        .width = 15, .height = 15, .radius = 8, .rotateSprite = false,
        .frameStart = 285, .numFrames = 1, .animRate = 0.0f
    },
    // ENT_STAIRS_DOWN
    {
        .type = TYPE_SYSTEM,
        .width = 15, .height = 15, .radius = 8, .rotateSprite = false,
        .frameStart = 201, .numFrames = 1, .animRate = 0.0f
    },
    // ENT_STAIRS_UP
    {
        .type = TYPE_SYSTEM,
        .width = 15, .height = 15, .radius = 8, .rotateSprite = false,
        .frameStart = 200, .numFrames = 1, .animRate = 0.0f
    },
    // ENT_UPGRADE
    {
        .type = TYPE_PICKUP, .ai = AI_NONE,
        .width = 15, .height = 15, .radius = 8, .rotateSprite = false,
        .frameStart = 260, .numFrames = 1, .animRate = 0.0f
    }
};

/*
* old method
// type,maxHealth,damage,speed,attackSpeed,ai,width,height,rotateSprite,projectileId, frameStart,frameAttack,frameOffset,numFrames;
static struct EntityBlueprint entityBlueprint[] =
{
	// Dummy Object
	TYPE_STATIC, 0, 0, 0, 0, AI_NONE, 0, 0, 0, 0, 0, 0, 0, 0,
	// Player
	TYPE_CREATURE, 100, 10, 1, 1, AI_NONE, 16, 16, 1, 0, 0, 4, 0, 3,
	// Zombie
	TYPE_CREATURE, 25, 25, 1, 1, AI_CHASE_PLAYER, 16, 16, 1, 0, 16, 4, 0, 3
};*/