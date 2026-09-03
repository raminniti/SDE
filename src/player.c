#include "player.h"
#include "engine.h"
#include "data.h"
#include "projectile.h"

PlayerProgress gPlayerProgress = { 0 };
static float attackCoolDown = 0.0f;
static int mana = 0;

void PlayerInit(int gridX, int gridY) {
    gPlayerProgress.keys = 0;
    gPlayerProgress.hasSpeedBoots = false;
    gPlayerProgress.maxHealthBonus = 0;

    gPlayer = EntitySpawn(gridX * TILE_SIZE, gridY * TILE_SIZE, ENT_PLAYER);
}

void PlayerUpdate(float dt) {
    if (gPlayer == NULL) return;
    if (gPlayer->health<1 || gPlayer->hurtFrames > 0) return;

    //Vec2 dir = { 0 };

    //TODO: make it so most recent direction overrides last using a queue
    if (InputIsKeyHeld(SDLK_a)){
        gPlayer->vel.x = -1;
        gPlayer->direction = 90;
        gPlayer->facingDir = LEFT;
        //EntityAnimate(gPlayer);
    }
    else if (InputIsKeyHeld(SDLK_d)){
        gPlayer->vel.x = 1;
        gPlayer->direction = 270;
        gPlayer->facingDir = RIGHT;
        //EntityAnimate(gPlayer);
    }
    else if (InputIsKeyHeld(SDLK_w)){
        gPlayer->vel.y = -1;
        gPlayer->direction = 180;
        gPlayer->facingDir = UP;
        //EntityAnimate(gPlayer);
    }
    else if (InputIsKeyHeld(SDLK_s)){
        gPlayer->vel.y = 1;
        gPlayer->direction = 0;
        gPlayer->facingDir = DOWN;
        //EntityAnimate(gPlayer);
    }

    //save last direction for attacks
    if((gPlayer->vel.x != 0 || gPlayer->vel.y != 0))// && !InputIsKeyHeld(SDLK_SPACE))
        gPlayer->lastDir = gPlayer->vel;

    //Moved to Entity update for all creatures
    //EntityMoveWithCollision(gPlayer, gPlayer->vel);

    if (attackCoolDown > 0){
        attackCoolDown -= dt;
    }

    if (InputIsKeyHeld(SDLK_SPACE) && attackCoolDown <= 0) {
        Projectile * p = ProjectileSpawn( Vec2Add(gPlayer->pos, Vec2Scale(gPlayer->lastDir, 16)), Vec2Scale(gPlayer->lastDir, 2), PROJ_BULLET, FACTION_PLAYER);
        p->direction = gPlayer->facingDir;
        AudioPlaySound(SND_SHOOT);
        attackCoolDown = 0.32f;
    }
}

