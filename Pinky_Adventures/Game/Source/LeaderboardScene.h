#ifndef __LEADERBOARD_SCENE_H__
#define __LEADERBOARD_SCENE_H__

#include "Module.h"
#include "Animation.h"
#include "Textures.h"

class LeaderboardScene : public Module
{
public:

	// Constructors & Destructors
	LeaderboardScene();
	~LeaderboardScene();

	// Main module steps
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void ranks();
	void bubbleSort(int array[], int size);

public:
	// Textures
	Animation animLurkingCat;
	SDL_Texture* texLurkingCat;

	int leaderboard[10];
	int currentScore;

private:
	int titleFont;
	int subtitleFont;

	// Ranks
	Uint32 startTime = 0;
	uint dTime;
	int randNum;
	
	int prevScore[2];
	SDL_Rect bgColor;
};

#endif //__LEADERBOARD_SCENE_H__