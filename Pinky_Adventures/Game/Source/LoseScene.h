#ifndef __LOSESCENE_H__
#define __LOSESCENE_H__

#include "Module.h"
#include "Player.h"


struct SDL_Texture;

class LoseScene : public Module
{
public:

	LoseScene();

	// Destructor
	virtual ~LoseScene();

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
	Player* player;
private:
	const char* musicLosePath;
	const char* imagePath;

	SDL_Texture* loseTexture;
	SDL_Texture* button1;
	SDL_Texture* button2;

	const char* losePath;
	const char* but1Path;
	const char* but2Path;

	SDL_Texture* p2sTexture;
	int v_start = 0;

	bool retry;
	bool musLose;
};

#endif // __LOSESCENE_H__