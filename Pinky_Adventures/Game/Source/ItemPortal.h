#ifndef __ITEMPORTAL_H__
#define __ITEMPORTAL_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

#include "Animation.h"

struct SDL_Texture;

class Portal : public Entity
{
public:

	Portal();
	virtual ~Portal();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();
public:

	bool isPicked;


private:

	SDL_Texture* texture;
	const char* texturePath;

	int width, height;
	Animation* currentAnimPort;
	Animation PortAnim;
	SDL_RendererFlip flipType;
	
	PhysBody* pbody;

};

#endif // __ITEMPORTAL_H__