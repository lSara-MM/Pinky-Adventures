#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"

#include "LoseScene.h"
#include "IntroScene.h"
#include "Scene.h"
#include "FadeToBlack.h"

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
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();
	
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->iScene, 5);

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->scene, 5);

	if (musLose == false)
	{
		app->audio->PauseMusic();
		musLose = true;
		app->audio->PlayMusic(musicLosePath, 0);
	}

	app->render->camera.x = 0;
	app->render->camera.y = 0;
	app->render->DrawTexture(loseTexture, 0, 0);

	// retry
	if (retry == true)
		app->render->DrawTexture(button1, 32, 53);
	else
		app->render->DrawTexture(button2, 344, 53);

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && retry == true)
	{
		app->audio->PauseMusic();
		app->fade->FadingToBlack(this, (Module*)app->scene, 45);
	}
	// non retry
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && retry == false)
	{
		app->audio->PauseMusic();
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
	}

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN ||
		app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
		retry = !retry;

	return true;
}

// Called each loop iteration
bool LoseScene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	//app->render->DrawTexture(LoseTexture, -65, 0);
	
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

	return true;
}
