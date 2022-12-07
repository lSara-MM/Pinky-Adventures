#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

#include "Animation.h"

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
public:

	bool isPicked;


private:

	SDL_Texture* textureWalkingEnemy;
	const char* texturePathWalkingEnemy;

	SDL_Texture* textureFlyingEnemy;
	const char* texturePathFlyingEnemy;

	Animation* currentAnimWalkingEnemy;
	Animation* currentAnimFlyingEnemy;
	Animation WalkingEnemyAnim;
	Animation FlyingEnemyAnim;

	bool idle;
	bool chase;

	PhysBody* pbody;

};

#endif // __ITEMGEM_H__