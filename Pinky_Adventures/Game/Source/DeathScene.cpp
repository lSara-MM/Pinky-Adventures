#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "DeathScene.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "Physics.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

DeathScene::DeathScene() : Module()
{
	name.Create("deathScene");
}

// Destructor
DeathScene::~DeathScene()
{}

// Called before render is available
bool DeathScene::Awake(pugi::xml_node& config)
{
	LOG("Loading DeathScene");
	bool ret = true;

	// iterate all objects in the IntroScene
	// Check https://pugixml.org/docs/quickstart.html#access
	/*for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Coin* item = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);
		item->parameters = itemNode;
	}*/

	deathPath = config.attribute("path").as_string();
	musicDeath = config.attribute("audioDeathPath").as_string();
	return ret;
}

// Called before the first frame
bool DeathScene::Start()
{

	Death = app->tex->Load(deathPath);
	//img = { 0, 0, app->win->GetWidth(),  app->win->GetHeight() };
	app->audio->PlayMusic(musicDeath,0);

	return true;
}

// Called each loop iteration
bool DeathScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool DeathScene::Update(float dt)
{
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	//int a = 3 * app->win->GetScale();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	/*if (app->input->godMode == true)
	{
		if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			app->render->camera.y += a;

		if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			app->render->camera.y -= a;

		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			app->render->camera.x += a;

		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			app->render->camera.x -= a;
	}*/

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);

	return true;
}

// Called each loop iteration
bool DeathScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	app->render->DrawTexture(Death, -260, -250);//no m'ha sortit reescalar-lo

	return ret;
}

// Called before quitting
bool DeathScene::CleanUp()
{
	LOG("Freeing IntroScene");
	app->audio->PauseMusic();
	app->tex->UnLoad(Death);

	return true;
}
