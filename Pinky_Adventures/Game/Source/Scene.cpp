#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"

#include "Physics.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;


	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{

		Coin* item = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);
		item->parameters = itemNode;
		listCoins.Add(item);
	}

	gem = (Gem*)app->entityManager->CreateEntity(EntityType::GEM);
	gem->parameters = config.child("item2");
	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");


	//audioPath = config.child("lvl1").attribute("audioPath").as_string();

	back1Path = config.attribute("background1").as_string();
	back2Path = config.attribute("background2").as_string();
	back3Path = config.attribute("background3").as_string();
	losePath = config.attribute("losebackground").as_string();
	but1Path = config.attribute("losebutton1").as_string();
	but2Path = config.attribute("losebutton2").as_string();

	// music
	musicPathBg = config.attribute("music").as_string();
	musicLosePath = config.attribute("musicLose").as_string();

	originList = listCoins.start;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	app->physics->Enable();
	app->entityManager->Start();

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	posx1 = 0;
	posx2 = 0;
	posx3 = 0;

	contadorT = 0;

	// L03: DONE: Load map
	app->map->Load();
	secret = false;
	
	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());
	
	// cammera 
	maxCameraPosLeft = 0;
	maxCameraPosRigth = app->map->mapData.width * app->map->mapData.tileWidth * app->win->GetScale();

	bgColor = { 0, 0, app->win->GetWidth() * app->win->GetScale() + 800,  app->win->GetHeight() };

	if (player->active == false) {
		player->Enable();
	}

	BACK1 = app->tex->Load(back1Path);
	BACK2 = app->tex->Load(back2Path);
	BACK3 = app->tex->Load(back3Path);
	loseTexture = app->tex->Load(losePath);
	button1 = app->tex->Load(but1Path);
	button2 = app->tex->Load(but2Path);

	//app->audio->PlayMusic(audioPath, 0);

	app->audio->PlayMusic(musicPathBg, 0); //nose que pasa que de repent ha dixat de funcionar despues del fade to black 2
	//passava amb el tetris, posar a 0 el fade ho arregla
	
	retry = true;
	musLose = false;
	mute = false;
	end = false;
	app->input->godMode = true;	// TO CHANGE WHEN RELEASE
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	Debug();
	//app->render->DrawRectangle(bgColor, 88, 141, 190);

	// parallax
	int maxR = -player->position.x * app->win->GetScale() + 300;
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
	{
		posx1 = maxR*0.2f;
	}
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
	{
		posx2 = maxR*0.2f; //ajustar velocitat com es prefereixi, o posar-les diferents entre els backgrounds
	}
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
	{
		posx3 = -maxR*0.2f;
	}
	app->render->DrawTexture(BACK1, posx1, -380, &bgColor,1.0f,NULL, NULL, NULL);
	app->render->DrawTexture(BACK3, posx3, -380, &bgColor, 1.0f, NULL, NULL, NULL);
	app->render->DrawTexture(BACK2, posx2, -380, &bgColor, 1.0f, NULL, NULL, NULL);

	

	if (secret == false) {
		app->map->Draw();
	}
	
	else if (secret == true) {
		app->map->DrawSecret();
	}

	app->entityManager->Update(dt);	


	if (player->ded == true) {
		contadorT++;
	}

	if (player->ded == true && (player->ani == false || contadorT == 80)) 
	{
		Lose();
	}

	if (player->position.x > 624 && player->position.x < 895 && player->position.y > 224)
	{
		secret = true;
	}

	
	//app->map->DrawPlatformCollider();

	
	if (end == true) {
		app->audio->PauseMusic();
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;
	
	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->render->camera.x = 0;
	app->render->camera.y = 0;

	app->audio->PauseMusic();
	player->Disable();


	app->render->camera.x = 0;
	app->render->camera.y = 0;

	app->audio->PauseMusic();
	app->tex->UnLoad(BACK1);
	app->tex->UnLoad(BACK2);
	app->tex->UnLoad(BACK3);

	app->tex->UnLoad(loseTexture);
	app->tex->UnLoad(button1);
	app->tex->UnLoad(button2);
	
	// Reset items
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = app->entityManager->entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;
		pEntity->active = true;
	}
	listCoins.start = originList;


	app->physics->Disable();
	//app->map->CleanUp();per algun motiu no pilla algo del tileset i peta
	app->map->UnloadCollisions();

	return true;
}

void Scene::Debug()
{
	int a = 3 * app->win->GetScale();

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		app->fade->FadingToBlack(this, (Module*)app->scene, 0);
	}


	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	// Instant win
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
		end = true;
	// Instant lose
	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {
		player->ded = true;
		app->audio->PlayFx(player->fxDeath);
	}

	// GodMode
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) //canvi valors per tal que siguin iguals al que demana
		app->input->godMode = !app->input->godMode;

	// Show collisions
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		app->physics->collisions = !app->physics->collisions;


	// Mute / unmute
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
		mute = !mute;



	if (app->input->godMode == true)
	{
		// Free camera
		if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			app->render->camera.y += a;

		if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			app->render->camera.y -= a;

		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			app->render->camera.x += a;

		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			app->render->camera.x -= a;

		// Reset camera
		if (app->input->GetKey(SDL_SCANCODE_C) == KEY_REPEAT)
		{
			int maxR = -player->position.x * app->win->GetScale() + 300;
			app->render->camera.x = maxR;
		}

		// Borrar al final
		if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		{
			app->audio->PauseMusic();
			app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
		}

	}

	(mute) ? app->audio->PauseMusic() : app->audio->ResumeMusic();

}

bool Scene::Lose()
{
	if (musLose == false)
	{
		app->audio->PauseMusic();
		musLose = true;
		app->audio->PlayMusic(musicLosePath, 0);
	}

	app->render->camera.x = 0;
	app->render->camera.y = 0;
	player->Disable();
	app->render->DrawTexture(loseTexture, 0, 0);

	// retry
	if (retry == true)
	{
		app->render->DrawTexture(button1, 32, 53);
	}
	else
	{
		app->render->DrawTexture(button2, 344, 53);
	}
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && retry == true)
	{
		app->audio->PauseMusic();
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
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