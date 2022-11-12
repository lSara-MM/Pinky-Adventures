#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "ItemCoin.h"
#include "ItemGem.h"

#include "App.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

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

	void Debug();
	bool Lose();

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;
	Gem* gem;
	List<Coin*> listCoins;
	//ListItem<Item*>* coins;
	int contadorT;
	int maxCameraPosLeft;
	int maxCameraPosRigth;

	bool secret; //mapa secreto activo o no
	//active = false;

	SDL_Rect bgColor;
	int posx1, posx2, posx3;
	bool end;
private:
	ListItem<Coin*>* originList;

	SDL_Texture* loseTexture;
	SDL_Texture* button1;
	SDL_Texture* button2;
	SDL_Texture* BACK1;
	SDL_Texture* BACK2;
	SDL_Texture* BACK3;

	bool mute;
	const char* musicPathBg;
	const char* musicLosePath;


	const char* audioLosePath;
	const char* back1Path;
	const char* back2Path;
	const char* back3Path;
	const char* losePath;
	const char* but1Path;
	const char* but2Path;

	bool retry;

	bool musLose;
};

#endif // __SCENE_H__