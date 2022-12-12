#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "List.h"

#include "Animation.h"
#include "ItemCoin.h"
#include "ItemGem.h"

struct SDL_Texture;


class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void State(iPoint posPlayer, iPoint posEnemy);

public:

	// The pointer to the current player animation
	// It will be switched depending on the player's movement direction
	Animation* currentAnimation = nullptr;

	// A set of animations
	Animation idleAnim;
	Animation forwardAnim;
	Animation jumpAnim;
	Animation deathAnim;

	SDL_RendererFlip flipType;


	int jump, speed, width, height;

	float grav;
	bool idle;
	bool chase;

	PhysBody* pbody;

private:

	SDL_Texture* textureWalkingEnemy;
	const char* texturePathWalkingEnemy;

	SDL_Texture* textureFlyingEnemy;
	const char* texturePathFlyingEnemy;

	Animation* currentAnimWalkingEnemy;
	Animation* currentAnimFlyingEnemy;
	Animation WalkingEnemyAnim;
	Animation FlyingEnemyAnim;


	

	SDL_Texture* texture;
	const char* texturePath;
	const char* jumpPath;
	const char* landPath;
	const char* deathPath;

	uint fxJump;
	uint fxLand;

};

#endif // __ITEMGEM_H__