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

#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiSliderBar.h"


#include "Settings.h"

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

	// Settings
	bool OnGuiMouseClickEvent(GuiControl* control);

public:

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
	
	bool pause;

	Timer tempo;

	bool frcap;

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
	
	pugi::xml_node sceneNode;

	Timer timerLvl1;
	float timeLeft;
	float timeElapsed;



	// UI
	const char* coinPath;
	SDL_Texture* coin_tex;

	Animation* currentAnimCoin;
	Animation coinAnim;

	const char* heartPath;
	SDL_Texture* heart_tex;

	Animation* currentAnimHeart;
	Animation heartAnim;


	// Settings
	Settings options;
	Settings* pSettings = &options;
};

#endif // __SCENE_H__