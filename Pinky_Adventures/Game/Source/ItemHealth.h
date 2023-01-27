#ifndef __ITEMHEALTH_H__
#define __ITEMHEALTH_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

#include "Animation.h"
#include "List.h"

struct SDL_Texture;

class Health : public Entity
{
public:

	Health();
	virtual ~Health();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SpawnHealth();
	void CreateHealth(int gid_, int x_, int y_);
public:

	bool isAlive;
	int ID;
	PhysBody* pbody;
private:

	SDL_Texture* texture;

	Animation* currentAnimHealth;
	Animation healthAnim;
};

#endif // __ITEMHEALTH_H__