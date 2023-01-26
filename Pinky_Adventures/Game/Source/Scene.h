#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "ItemCoin.h"
#include "ItemGem.h"
#include "ItemPortal.h"
#include "ItemSave.h"
#include "ItemHealth.h"

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

	int lives;

	Player* player;
	Gem* gem;
	Coin* coins;
	Enemy* enemy;
	Portal* portal;
	Save* save;
	Health* health;

	List<Coin*> listCoins;
	int coinIDset;

	List<Health*> listHealth;
	int healthIDset;

	List<Enemy*> listEnemies;
	int enemyIDset;
	
	int maxCameraPosLeft;
	int maxCameraPosRigth;
	bool secret;

	SDL_Rect bgColor;
	SDL_Rect attackCajaNoCd;
	SDL_Rect attackCajaCd;

	int posx1, posx2, posx3;
	bool end;
	bool freeCam;

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

	const char* attackIconPath;
	SDL_Texture* attackIcon;
	
	PhysBody* ghostCollider;

	bool drawPaths;
	bool frame30;
};

#endif // __SCENE_H__