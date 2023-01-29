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
#include "GuiManager.h"

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
	name.Create("leaderboardScene");
}

LeaderboardScene::~LeaderboardScene()
{
	// You should do some memory cleaning here, if required
}

bool LeaderboardScene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;


	// NO FUNCIONEN BE ELS PUGIS ESTOS :')
	pugi::xml_document gameStateFile;
	pugi::xml_parse_result result = gameStateFile.load_file("save_game.xml");

	leadLoadNode = gameStateFile.child("save_state").child("leaderboardScene");

	LoadState(leadLoadNode);


	pugi::xml_document* saveDoc = new pugi::xml_document();
	pugi::xml_node saveStateNode = saveDoc->append_child("save_state");

	leadSaveNode = saveStateNode.append_child("leaderboardScene");

	return ret;
}

bool LeaderboardScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// Set camera position
	app->render->camera.x = app->render->camera.y = 0;

	// Load textures
	//texLurkingCat = app->tex->Load("pinball/ss_LurkingCat.png");
	bgColor = { 0, 0, app->win->GetWidth() * app->win->GetScale(), app->win->GetHeight() * app->win->GetScale() };

	// buttons

	listButtons.Add((GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, buttons[0], { 25, 35, 90, 27 }, 10, this, ButtonType::SMALL));

	return ret;
}

bool LeaderboardScene::CleanUp()
{
	prevScore[0] = prevScore[1];

	listButtons.Clear();
	app->guiManager->CleanUp();
	return true;
}

bool LeaderboardScene::PreUpdate()
{
	return true;
}

bool LeaderboardScene::Update(float dt)
{
	app->render->DrawRectangle(bgColor, 206, 167, 240);


	// Change screens
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		app->fade->FadingToBlack(this, (Module*)app->scene, 0);
	}
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		app->fade->FadingToBlack(this, (Module*)app->iScene, 0);
	}

	ranks();

	// render text
	app->render->TextDraw("Leaderboard", 130, 40, 24, { 125, 0, 138 });

	for (int i = 0; i < 10; i++)
	{
		string s_num = std::to_string(i + 1);
		const char* ch_num = s_num.c_str();
		int y = 100 + 25 * i;
		(i < 9) ? app->render->TextDraw(ch_num, 100, y, 12, { 0, 0, 0 })
			: app->render->TextDraw(ch_num, 88, y, 12, { 0, 0, 0 });

		app->render->TextDraw(".", 117, y, 12, { 0, 0, 0 });


		string s_score = std::to_string(leaderboard[i]);
		const char* ch_score = s_score.c_str();
		if (leaderboard[i] == currentScore)
		{
			SDL_Rect rect = { 127, 95 + 25 * i, strlen(ch_score) * 20, 20 };
			app->render->DrawRectangle(rect, 150, 0, 255, 75);
			app->render->TextDraw("Current score", 32 * strlen(ch_score) + 130, y, 10);
		}

		app->render->TextDraw(ch_score, 130, y, 12);
	}

	/*app->render->TextDraw(150, 680, titleFont, "PREVIOUS SCORE", 0.3f);
	string s_Pnum = std::to_string(prevScore[0]);
	const char* ch_Pnum = s_Pnum.c_str();
	app->render->TextDraw(200, 720, subtitleFont, ch_Pnum);*/


	// Keep playing
	return true;
}

bool LeaderboardScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	app->guiManager->Draw();

	return ret;
}

void LeaderboardScene::ranks()
{
	bubbleSort(leaderboard, 10);
	prevScore[1] = currentScore;

	SaveState(leadSaveNode);
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

bool LeaderboardScene::LoadState(pugi::xml_node& data)
{
	for (int i = 0;  i < 10; i++)
	{
		string s_num = std::to_string(i + 1);
		const char* ch_num = s_num.c_str();

		leaderboard[i] = data.child(ch_num).attribute("score").as_int();
	}

	return true;
}

bool LeaderboardScene::SaveState(pugi::xml_node& data)
{
	for (int i = 0; i < 10; i++)
	{		
		string s_num = std::to_string(i + 1);
		const char* ch_num = s_num.c_str();

		pugi::xml_node score = data.append_child("score");

		string s_score = std::to_string(leaderboard[i]);
		const char* ch_score = s_score.c_str();

		score.append_attribute("score") = ch_score;
	}
	return true;
}

bool LeaderboardScene::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Event by %d ", control->id);

	app->audio->PlayFx(control->fxControl);

	switch (control->id)
	{
	case 1:
		LOG("Button Close settings click");
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
		break;
	}

	return true;
}
