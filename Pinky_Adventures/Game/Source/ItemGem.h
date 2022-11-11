#ifndef __ITEMGEM_H__
#define __ITEMGEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

#include "Animation.h"

struct SDL_Texture;

class Gem : public Entity
{
public:

	Gem();
	virtual ~Gem();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();
public:

	bool isPicked;


private:

	SDL_Texture* texture;
	const char* texturePath;

	Animation* currentAnimGem;
	Animation GemAnim;

	//DONE 4: Add a physics to an item
	PhysBody* pbody;

	//List<PhysBody*> listItems;
};

#endif // __ITEMGEM_H__