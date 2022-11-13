#ifndef __LOGOSCENE_H__
#define __LOGOSCENE_H__

#include "Module.h"
#include "Player.h"


struct SDL_Texture;

class LogoScene : public Module
{
public:

	LogoScene();

	// Destructor
	virtual ~LogoScene();

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

	const char* musicLogo;
	const char* imagePath;

	SDL_Texture* logoTexture;
	const char* logoPath;

	SDL_Texture* p2sTexture;
	int v_start = 0;


};

#endif // __LOGOSCENE_H__