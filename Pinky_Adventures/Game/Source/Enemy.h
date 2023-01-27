#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "List.h"

#include "Animation.h"
#include "ItemCoin.h"
#include "ItemGem.h"
#include "Physics.h"

struct SDL_Texture;

enum class eState
{
	IDLE,
	CHASE,
	DEAD,
	RETURN
};

enum class eType
{
	BASIC,	// Only walk (in case in future some enemies can attack or do special moves
	FLYING, 
	UNKNOWN
};

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void CreatePath(iPoint posPlayer, iPoint posEnemy, b2Vec2 &vel);

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:
	// The pointer to the current player animation
	// It will be switched depending on the player's movement direction
	Animation* currentAnimation = nullptr;

	// A set of animations
	Animation idleFlyingEnemyAnim;
	Animation idleWalkingEnemyAnim;
	Animation ForwardWalkingEnemyAnim;
	Animation deathFlyEnemyAnim;

	Animation deathWalkEnemyAnim;

	SDL_RendererFlip flipType;

	int speed, width, height;
	eState state;
	eType type;
	int ID;

	float grav;
	PhysBody* pbody, *headSensor;

	iPoint pos_Player;
	iPoint pos_Enemy;
	iPoint pos_Origin;	// make the enemy return to the spawn point
	iPoint originPos;
	bool origin;

	int detectionDistance;

	uint fxDeath_Enemy;

	float dtE;

private:

	SDL_Texture* textureWalkingEnemy;
	const char* texturePathWalkingEnemy;

	SDL_Texture* textureEnemy;
	const char* texturePathEnemy;

	Animation* currentAnimWalkingEnemy;
	Animation* currentAnimFlyingEnemy;
	Animation WalkingEnemyAnim;
	Animation FlyingEnemyAnim;

	int enGID = 244;	// red square

	const char* deathPath;

	uint fxJump;
	uint fxLand;


	iPoint posPath;
	iPoint posPath_0;
};

#endif // __ITEMGEM_H__