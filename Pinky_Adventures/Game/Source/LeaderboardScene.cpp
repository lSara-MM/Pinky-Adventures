#include "LeaderboardScene.h"
#include "EntityManager.h"
#include "Player.h"
#include "App.h"
#include "Window.h"
#include "Textures.h"
#include "Render.h"
#include "Scene.h"
#include "Physics.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

#include <iostream>
using namespace std;
#include <sstream>

#include <stdlib.h>     /* srand, rand */
#include <time.h>

#include "Animation.h"

LeaderboardScene::LeaderboardScene() : Module()
{
	animLurkingCat.PushBack({ 0, 0, 256, 192 });
	animLurkingCat.PushBack({ 0, 0, 256, 192 });
	animLurkingCat.PushBack({ 0, 0, 256, 192 });

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			animLurkingCat.PushBack({ 256 * j, 192 * i, 256, 192 });
			if (i == 3 && j == 3) { break; }
		}
	}
	for (int i = 7; i > 0; i--)
	{
		animLurkingCat.PushBack({ 256 * i, 0, 256, 192 });
	}
	animLurkingCat.speed = 0.2f;
}

LeaderboardScene::~LeaderboardScene()
{
	// You should do some memory cleaning here, if required
}

bool LeaderboardScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// Set camera position
	app->render->camera.x = app->render->camera.y = 0;

	// Load textures
	texLurkingCat = app->tex->Load("pinball/ss_LurkingCat.png");
	bgColor = { 0, 0, app->win->GetWidth() * app->win->GetScale(), app->win->GetHeight() * app->win->GetScale() };

	srand(time(NULL));
	startTime = SDL_GetTicks();
	return ret;
}

bool LeaderboardScene::CleanUp()
{
	prevScore[0] = prevScore[1];
	animLurkingCat.Reset();
	return true;
}

bool LeaderboardScene::Update(float dt)
{
	dTime = SDL_GetTicks() - startTime;
	randNum = rand() % 3000 + 10000;
	
	app->render->DrawRectangle(bgColor, 162, 209, 255);

	// Animation
	//LOG("%d", randNum);
	if (dTime < 4000 || animLurkingCat.HasFinished() == true)
	{
		animLurkingCat.Update();
		//app->render->DrawTexture(texLurkingCat, 368, 300, &(animLurkingCat.GetCurrentFrame()), 1.f, 1.f, -90, INT_MAX, INT_MAX, SDL_renderFlip::SDL_FLIP_HORIZONTAL);

	}
	if (dTime > randNum)
	{
		startTime = SDL_GetTicks();
	}

	// Change screens
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
	}
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		app->fade->FadingToBlack(this, (Module*)app->scene, 0);
	}
	if (app->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		//startTime = SDL_GetTicks();
		LOG("startTime: %d\ndTime: %d", startTime, dTime);
	}
	
	ranks();

	// render text
	//app->render->TextDraw(220, 30, titleFont, "HIGH", 0.7f);
	//app->render->TextDraw(170, 80, titleFont, "SCORES", 0.7f);
	for (int i = 0; i < 10; i++)
	{
		/*	string s_num = std::to_string(i+1);
			const char* ch_num = s_num.c_str();
			(i < 9) ? app->render->TextDraw(85, 164 + 50 * i, titleFont, ch_num, 0.5f)
				: app->render->TextDraw(53, 164 + 50 * i, titleFont, ch_num, 0.5f);
			app->render->TextDraw(117, 164 + 50 * i, titleFont, ".", 0.5f);


			string s_score = std::to_string(leaderboard[i]);
			const char* ch_score = s_score.c_str();
			if (leaderboard[i] == currentScore)
			{
				SDL_Rect rect = { 147, 160 + 50 * i, 32 * strlen(ch_score), 40 };
				app->render->DrawRectangle(rect, 200, 0, 255, 75);
				app->render->TextDraw(32 * strlen(ch_score) + 170, 167 + 50 * i, subtitleFont, "Current", 0.75f);
			}

			app->render->TextDraw(150, 164 + 50 * i, subtitleFont, ch_score);
		}

		app->render->TextDraw(150, 680, titleFont, "PREVIOUS SCORE", 0.3f);
		string s_Pnum = std::to_string(prevScore[0]);
		const char* ch_Pnum = s_Pnum.c_str();
		app->render->TextDraw(200, 720, subtitleFont, ch_Pnum);*/

	}

	// Keep playing
	return true;
}

void LeaderboardScene::ranks()
{
	bubbleSort(leaderboard, 10);
	prevScore[1] = currentScore;
}

void LeaderboardScene::bubbleSort(int array[], int size)
{
	// loop to access each array element
	for (int step = 0; step < size - 1; ++step) {

		// loop to compare array elements
		for (int i = 0; i < size - step - 1; ++i) {

			// compare two adjacent elements
			// change > to < to sort in descending order
			if (array[i] < array[i + 1]) {

				// swapping occurs if elements
				// are not in the intended order
				int temp = array[i];
				array[i] = array[i + 1];
				array[i + 1] = temp;
			}
		}
	}
}