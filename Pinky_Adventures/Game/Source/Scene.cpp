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

	// music
	musicBg = config.attribute("music").as_string();

	originList = listCoins.start;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	posx1 = 0;
	posx2 = 0;
	posx3 = 0;

	contadorT = 0;

	app->entityManager->Start();

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
	
	//app->audio->PlayMusic(audioPath, 0);

	secret = false;

	//player->Start();

	app->audio->PlayMusic(musicBg,0); //nose que pasa que de repent ha dixat de funcionar despues del fade to black 2
	//passava amb el tetris, posar a 0 el fade ho arregla

	mute = false;

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
	int a = 3 * app->win->GetScale();
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	app->input->godMode = true;	// TO CHANGE WHEN RELEASE
	// GodMode
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		app->input->godMode = !app->input->godMode;

	// Show collisions
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		app->physics->collisions = !app->physics->collisions;

	// Instant win
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		player->ded = true;

	// Instant lose
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		player->ded = true;

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
			app->fade->FadingToBlack(this, (Module*)app->iScene, 90);

	}
	
	(mute) ? app->audio->PauseMusic() : app->audio->ResumeMusic();

	//app->render->DrawRectangle(bgColor, 88, 141, 190);

	int maxR = -player->position.x * app->win->GetScale() + 300;
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
	{
		posx1 = maxR*0.2f;
	}
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
	{
		posx2 = maxR*0.2f;//ajustar velocitat com es prefereixi, o posar-les diferents entre els backgrounds
	}
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
	{
		posx3 = -maxR*0.2f;
	}
	app->render->DrawTexture(BACK1, posx1, -380, &bgColor,1.0f,NULL, NULL, NULL);

	app->render->DrawTexture(BACK3, posx3, -380, &bgColor, 1.0f, NULL, NULL, NULL);

	app->render->DrawTexture(BACK2, posx2, -380, &bgColor, 1.0f, NULL, NULL, NULL);

	


	//if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && player->ded == false) {
	//
	//	posx1 -= 5;
	//	posx2 -= 5;
	//	posx3 += 5;
	//}

	//if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && player->ded == false) {
	//	posx1 += 5;
	//	posx2 += 5;
	//	posx3 -= 5;
	//}


	if (secret == false) {
		app->map->Draw();
	}
	
	else if (secret == true) {
		app->map->DrawSecret();
	}

	
	app->entityManager->Update(dt);	// millor que posar-ho individual
	//player->Update();


	if (app->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) {
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
		app->audio->PauseMusic();
	}
	


	if (player->ded == true) {
		contadorT++;
	}

	if (player->ded == true && contadorT == 80) {
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
	}

	if (player->position.x > 624 && player->position.x < 895 && player->position.y > 224)
	{
		secret = true;
	
	}
	
	//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more
	// Draw map
	

	app->map->DrawPlatformCollider();

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
