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

	SDL_Texture* BACK1;
	SDL_Texture* BACK2;
	SDL_Texture* BACK3;

	int posx1, posx2, posx3;

	const char* audioPath;
	const char* back1Path;
	const char* back2Path;
	const char* back3Path;

private:
	SDL_Texture* img;
	bool mute;
	const char* musicBg;
	ListItem<Coin*>* originList;
};

#endif // __SCENE_H__