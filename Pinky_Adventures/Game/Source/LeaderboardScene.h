#ifndef __LEADERBOARD_SCENE_H__
#define __LEADERBOARD_SCENE_H__

#include "Module.h"
#include "Animation.h"
#include "Textures.h"

#include "App.h"

class LeaderboardScene : public Module
{
public:

	// Constructors & Destructors
	LeaderboardScene();
	
	// Destructor
	virtual ~LeaderboardScene();

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

	void ranks();
	void bubbleSort(int array[], int size);

	//Load / Save methods
	bool LoadState(pugi::xml_node&);
	bool SaveState(pugi::xml_node&);

public:
	// Textures
	int leaderboard[10];
	int currentScore;

	pugi::xml_node leadSaveNode;
	pugi::xml_node leadLoadNode;

private:
	
	int prevScore[2];
	SDL_Rect bgColor;
};

#endif //__LEADERBOARD_SCENE_H__