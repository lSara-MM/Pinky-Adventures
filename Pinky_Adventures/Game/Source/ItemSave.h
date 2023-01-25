#ifndef __ITEMSAVE_H__
#define __ITEMSAVE_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

#include "Animation.h"

struct SDL_Texture;

class Save : public Entity
{
public:

	Save();
	virtual ~Save();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();
public:

	bool isPicked;
	uint fxSave;


private:

	SDL_Texture* texture;
	const char* texturePath;
	const char* fxPath;
	

	int width, height;
	Animation* currentAnimSave;
	Animation SaveIdleAnim;
	Animation SaveCheckAnim;
	SDL_RendererFlip flipType;
	
	PhysBody* pbody;

};

#endif // __ITEMSAVE_H__