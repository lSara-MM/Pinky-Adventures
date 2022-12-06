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
	bool InitEntities();

public:

	Player* player;
	Gem* gem;
	Coin* coins;

	List<Coin*> listCoins;
	
	int maxCameraPosLeft;
	int maxCameraPosRigth;
	bool secret;

	SDL_Rect bgColor;
	int posx1, posx2, posx3;
	bool end;

	pugi::xml_node sceneNode;
private:
	int contadorT;
	
	SDL_Texture* BACK1;
	SDL_Texture* BACK2;
	SDL_Texture* BACK3;

	bool mute;
	const char* musicPathBg;

	const char* back1Path;
	const char* back2Path;
	const char* back3Path;
	
	PhysBody* ghostCollider;
};

#endif // __SCENE_H__