#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"

#include "LeaderboardScene.h"
#include "LoseScene.h"
#include "IntroScene.h"
#include "Scene.h"
#include "FadeToBlack.h"
#include "GuiManager.h"


#include "Defs.h"
#include "Log.h"

LoseScene::LoseScene() : Module()
{
	name.Create("loseScene");
}

// Destructor
LoseScene::~LoseScene()
{}

// Called before render is available
bool LoseScene::Awake(pugi::xml_node& config)
{
	LOG("Loading LoseScene");
	bool ret = true;

	// iterate all objects in the LoseScene
	// Check https://pugixml.org/docs/quickstart.html#access
	musicLosePath = config.attribute("musicLose").as_string();

	losePath = config.attribute("background").as_string();
	but1Path = config.attribute("losebutton1").as_string();
	but2Path = config.attribute("losebutton2").as_string();

	return ret;
}

// Called before the first frame
bool LoseScene::Start()
{
	SString title("You lost");
	app->win->SetTitle(title.GetString());

	loseTexture = app->tex->Load(losePath);
	button1 = app->tex->Load(but1Path);
	button2 = app->tex->Load(but2Path);

	retry = true;
	musLose = false;
	

	// buttons
	for (int i = 0; buttons[i] != "\n"; i++)
	{
		listButtons.Add((GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, i + 1, buttons[i], { 30, 200 + 35 * i, 90, 27 }, 11, this, ButtonType::LARGE));
	}

	return true;
}

// Called each loop iteration
bool LoseScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool LoseScene::Update(float dt)
{
	//if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	//	app->LoadGameRequest();
	
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->scene, 5);

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->iScene, 5);

	if (musLose == false)
	{
		app->audio->PauseMusic();
		musLose = true;
		app->audio->PlayMusic(musicLosePath, 0);
	}

	app->render->camera.x = 0;
	app->render->camera.y = 0;
	app->render->DrawTexture(loseTexture, 0, 0);

	return true;
}

// Called each loop iteration
bool LoseScene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	app->guiManager->Draw();

	return ret;
}

// Called before quitting
bool LoseScene::CleanUp()
{
	LOG("Freeing LoseScene");
	app->audio->PauseMusic();

	app->tex->UnLoad(loseTexture);
	app->tex->UnLoad(button1);
	app->tex->UnLoad(button2);

	listButtons.Clear();
	app->guiManager->CleanUp();

	return true;
}

bool LoseScene::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Event by %d ", control->id);

	app->audio->PlayFx(control->fxControl);

	switch (control->id)
	{
	case 1:
		LOG("Button Retry click");
		app->scene->continueEnabled = false;
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		
		break;
	case 2:
		LOG("Button Give up click");
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
		break;
	case 3:
		LOG("Button Leaderboard click");
		app->fade->FadingToBlack(this, (Module*)app->leadScene, 90);
		break;
	}

	return true;
}
