#ifndef DATA_H
#define DATA_H
#include <stdbool.h>
#include "image.h"
#include "audio.h"
#include "vector.h"

// Texture slot mappings for the Image module, prob remove for 1 2048 texture atlas
enum Direction{ UP=0, LEFT, DOWN, RIGHT};

typedef enum {
    TEX_ATLAS = 0,
    TEX_GUI,
    TEX_COUNT
} TilesheetID;

// Sound effect indices
typedef enum {
    SND_HIT = 0,
    SND_SHOOT,
    SND_DEATH,
    SND_HURT,
    SND_STEP,
    SND_PICKUP,
    SND_BREAK,
    SND_COUNT
} SoundID;

// Entity blueprint database indices
typedef enum {
    ENT_DUMMY = 0,
    ENT_PLAYER,
    ENT_ZOMBIE,
    ENT_SOUL,
    ENT_STAIRS_DOWN,
    ENT_STAIRS_UP,
    ENT_UPGRADE,
    ENT_COUNT
} EntityID;

// Core behavioral
typedef enum {
    TYPE_STATIC = 0, // No logic
    TYPE_CREATURE,   // Actively moves, takes damage, tracks health
    TYPE_SYSTEM,
    TYPE_PICKUP,
    TYPE_COUNT// TYPE_PROJECTILE, TYPE_EFFECT,TYPE_DECOR,
} EntityType;

// AI types
typedef enum {
    AI_NONE = 0,
    AI_CHASE_PLAYER,
    AI_RANDOM_WANDER
} AiType;

typedef enum {
    FACTION_ENEMY = 0,
    FACTION_PLAYER,
    FACTION_NEUTRAL
} Faction;

// Immutable Data Blueprint (The Flyweight Pattern)
struct EntityBlueprint{
    unsigned char type, //player, creature, projectile, decoration, etc... 
        maxHealth,
        damage,			// damage done on touch
        speed,			// movement speed
        attackSpeed,	// time between being able to attack
        ai,				// Ai this entity has, if any. 
        width,			//size for collision
        height,
        radius,
        touchDamage;
		//interactable,	// if this can be activated, not needed if interactable type?
	bool rotateSprite;	// Should the sprite rotate based on the entity direction? 
	unsigned short projectileId, //if the ai tries to spawn a projectile it'll use this 
		frameStart,		// base frame/idle frame
		frameAttack,	// one frame that'll show after attacking
		frameOffset,	// optional -> can offset all frames to be something else or fix position on tilesheet
		numFrames;      // number of frames to cycle through from idle to show walking or play an idle animation
    float animRate;     // Time interval between animation steps
    Faction faction;
};

void DatabaseLoadAssets(void);
#endif
