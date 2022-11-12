#ifndef __DEATHSCENE_H__
#define __DEATHSCENE_H__

#include "Module.h"
#include "Player.h"
#include "ItemCoin.h"
#include "ItemGem.h"

struct SDL_Texture;

class DeathScene : public Module
{
public:

	DeathScene();

	// Destructor
	virtual ~DeathScene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;

private:
	
	SDL_Rect img;

	SDL_Texture* Death;

	const char* musicDeath;

	const char* deathPath;
};

#endif // __INTROSCENE_H__
