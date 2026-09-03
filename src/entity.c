#include "entity.h"
#include "level.h"
#include "vfx.h"
#include "ai.h"

static Entity entities[MAX_ENTITIES] = { 0 };//static Entity *entityPool[64] = { 0 };
static unsigned short entityCount = 0; //tracks unique ids
static unsigned short activeCount = 0;
Entity* gPlayer = NULL;
extern struct EntityBlueprint entityDatabase[ENT_COUNT];
static float levelChangeTimer = 0.0f;

void EntityManagerInit() {
	entityCount = 0;
	gPlayer = NULL;
	SDL_memset(entities, 0, activeCount);
	//spawn player here?
}

Entity* EntitySpawn(int x, int y, unsigned int eid) {
	struct EntityBlueprint* spawnData = &entityDatabase[eid];

	if (activeCount < MAX_ENTITIES) {
		Entity* ent = &entities[activeCount];

		// Wipe instance data block
		SDL_memset(ent, 0, sizeof(Entity));

		// Binds spawn properties
		ent->data = spawnData;
		ent->eType = spawnData->type;
		ent->pos.x = x;
		ent->pos.y = y;
		ent->health = spawnData->maxHealth;
		ent->frame = spawnData->frameStart;
		ent->id = ++entityCount;
		ent->eId = eid;
		ent->animTimer = 0.14f;
		ent->active = true;
		ent->playingAnim = true;
		ent->right = ent->pos.x + ent->data->width;
		ent->bottom = ent->pos.y + ent->data->height;
		activeCount++;
		return ent;
	}
	return nullptr;
}

void EntityKill(int id){
	if (id < activeCount && activeCount > 0) {
		for (int i = 0; i < activeCount; i++) {
			if (entities[i].id == id) {
				entities[i].active = false;
				return;
			}
		}
	}
}

void EntityKillIndex(int index) {
	if (activeCount == 0) return;
	activeCount--;
	entities[index] = entities[activeCount];
	entities[activeCount].active = false;
}

void EntityCleanup(void) {
	for (int i = activeCount - 1; i > 0; i--) {
		if (!entities[i].active) {
			// Swap with the last active entity
			activeCount--;
			entities[i] = entities[activeCount];
			entities[activeCount].active = false;
		}
	}
}

void EntityClearAll() {
	//Don't clear player, so start at 1
	for (int i = 1; i < activeCount; i++) {
		entities[i].active = false;
	}
	activeCount = 1;
}

Entity* EntityGetPool(void) {
	return &entities;
}

unsigned int EntityGetActiveCount() {
	return activeCount;
}

Entity* EntityGetById(int id) {
	for (int i = 0; i < activeCount; i++) {
		if (entities[i].id == id)
			return &entities[i];
	}
	return NULL;
}

void EntityMove(Entity *e, Vec2 v){
	e->pos.x += v.x;
	e->pos.y += v.y;
}

void EntitySetPos(Entity *e, int px, int py){
	e->pos.x = px;
	e->pos.y = py;
}

void EntityAnimate(Entity *e){
	if (!e->playingAnim)
		return;
	e->animTimer -= (float)ClockGetDeltaTime();
	int lastFrame = (e->data->frameStart + e->data->numFrames) - 1;

	if (e->frame < e->data->frameStart || e->frame > lastFrame){
		e->frame = e->data->frameStart;
		e->animTimer = 0.14;
	}
	if (e->animTimer < 0){
		e->animTimer += 0.14; // Add animation rate to entity data
		if (e->frame < lastFrame)
			e->frame += 1;
		else
			e->frame = e->data->frameStart;
	}
}

void EntityUpdateAll(float dt){
	if (levelChangeTimer > 0)
		levelChangeTimer -= dt;

	for (char i = activeCount-1; i >= 0; i--){
		Entity* e = &entities[i];
		
		switch (e->eType) {
		case TYPE_CREATURE:
			EntityAnimate(e);
			switch (e->data->ai) {
			case AI_CHASE_PLAYER: AIChaseMelee(e, dt);

			}
			//knockback creatures when hurt
			if (e->hurtFrames > 0) {
				e->hurtFrames--;
				
				switch (e->knockbackDir) {
				case UP:    e->vel.y = -2; break;// speed * deltaTime; break;
				case DOWN:  e->vel.y = 2; break;
				case LEFT:  e->vel.x = -2; break;
				case RIGHT: e->vel.x = 2; break;
				}
			}
			//move
			EntityMoveWithCollision(e, e->vel);
			//apply friction
			e->vel.x = 0;
			e->vel.y = 0;
			break;
		case TYPE_PICKUP: break;
		}

		//Get rid of inactive
		if (!e->active) EntityKillIndex(i);
	}
}

void EntityDrawAll() {
	Entity *e = 0;
	SDL_Point center = { 8, 8 };
	for (char i = activeCount - 1; i >= 0; i--) {
		e = &entities[i];
		if (e->hurtFrames > 0) {
			if (e->hurtFrames % 3 == 0) continue;
		}
		ImageDrawTileExt(e->pos.x, e->pos.y, TEX_ATLAS, e->frame, e->direction, &center, 0x00000000);
	}
}

void EntityHandleAllCollisions(void){
	Entity* player = gPlayer;
	//Check collision for entities 
	for (char i = activeCount - 1; i >= 0; i--) {
		Entity* a = &entities[i];
		for (int j = i - 1; j >= 0; j--) {
			Entity* b = &entities[j];

			//check for collision
			if (Vec2CheckRadiusOverlap(a->pos, 8, b->pos, 8)) {
				if (a->eType == TYPE_PICKUP && b == gPlayer) {
					AudioPlaySound(SND_PICKUP);
					if (b->health < b->data->maxHealth)
						b->health += 15;
					EntityKillIndex(i);
					continue;
				}
				else if (a->eType == TYPE_SYSTEM && b == gPlayer) {
					if (levelChangeTimer > 0)
						continue;
					if (a->eId == ENT_STAIRS_DOWN) {
						AudioPlaySound(SND_STEP);
						EntityClearAll();
						LevelLoad("levels/1.bin");
						LevelSelectRoom(LevelGetRoomId());
						levelChangeTimer = 1.4f;
					}
					else if (a->eId == ENT_STAIRS_UP) {
						AudioPlaySound(SND_STEP);
						EntityClearAll();
						LevelLoad("levels/0.bin");
						LevelSelectRoom(LevelGetRoomId());
						levelChangeTimer = 1.4f;
					}
				}
			}
		}
	}
}

void EntityMoveWithCollision(Entity* e, Vec2 vel) {
	int width = e->data->width;
	int height = e->data->height;
	char flags = (TILE_FLAG_LOW | TILE_FLAG_MID | TILE_FLAG_HIGH);

	//X-axis
	for (int i = 0; i < abs(vel.x); i++) {
		int step_x = sign(vel.x);
		int next_x = step_x + (int)e->pos.x;
		int check_x = (step_x > 0) ? next_x + width : next_x;
		char ct = LevelGetTileFlags(check_x, e->pos.y);
		char cb = LevelGetTileFlags(check_x, e->bottom);

		//Only move while there's no solid tile
		if (!(ct & flags) && !(cb & flags)) {
			e->pos.x += step_x;
			e->right = e->pos.x + width;
		}
		else {
			//Nudge player around tiles by one pixel
			if (!(ct & flags) && (cb & flags)) {
				e->pos.y -= 1;
				e->bottom = (int)e->pos.y + height;
				break;
			}
			else if ((ct & flags) && !(cb & flags)) {
				e->pos.y += 1;
				e->bottom = (int)e->pos.y + height;
				break;
			}
			break;
		}
	}

	// Y-axis
	for (int i = 0; i < abs(vel.y); i++) {
		int step_y = sign(vel.y);
		int next_y = step_y + (int)e->pos.y;
		int check_y = (step_y > 0) ? next_y + height : next_y;
		char cl = LevelGetTileFlags(e->pos.x, check_y);
		char cr = LevelGetTileFlags(e->right, check_y);
		//Only move while there's no solid tile
		if (!(cl & flags) && !(cr & flags)) {
			e->pos.y += step_y;
			e->bottom = e->pos.y + height;
		}
		else {
			//Nudge player around tiles by one pixel
			if (!(cl & flags) && (cr & flags)) {
				e->pos.x -= 1;
				e->right = (int)e->pos.x + width;
				break;
			}
			else if ((cl & flags) && !(cr & flags)) {
				e->pos.x += 1;
				e->right = (int)e->pos.x + width;
				break;
			}
			break;
		}
	}
}