#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"

#include "Pathfinding.h"
#include "Physics.h"
#include "FadeToBlack.h"
#include "ItemCoin.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

Scene::~Scene()
{}

bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	/*for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{

		Coin* item = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);
		item->parameters = itemNode;
		listCoins.Add(item);
	}*/

	back1Path = config.attribute("background1").as_string();
	back2Path = config.attribute("background2").as_string();
	back3Path = config.attribute("background3").as_string();
	
	musicPathBg = config.attribute("music").as_string();

	return ret;
}

bool Scene::Start()
{
	app->physics->Enable();
	app->render->camera.x = 0;
	app->render->camera.y = 0;

	posx1 = 0;
	posx2 = 0;
	posx3 = 0;

	contadorT = 0;
	
	// Load map
	bool retLoad = app->map->Load();

	//walkability nom�s de walking enemy per ara
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	InitEntities();
	app->entityManager->Enable();


	secret = false;
	ghostCollider = app->physics->CreateRectangle(890, 240, 10, 16 * app->win->GetScale(), bodyType::STATIC);

	
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());
	
	// camera 
	maxCameraPosLeft = 0;
	maxCameraPosRigth = app->map->mapData.width * app->map->mapData.tileWidth * app->win->GetScale();

	bgColor = { 0, 0, app->win->GetWidth() * app->win->GetScale() + 800,  app->win->GetHeight() };

	
	// Background
	BACK1 = app->tex->Load(back1Path);
	BACK2 = app->tex->Load(back2Path);
	BACK3 = app->tex->Load(back3Path);

	app->audio->PlayMusic(musicPathBg, 0);
	
	mute = false;
	end = false;
	return true;
}

bool Scene::PreUpdate()
{
	return true;
}

bool Scene::Update(float dt)
{
	Debug();
	
	// Background parallax
	int maxR = -player->position.x * app->win->GetScale() + 300;
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
		posx1 = maxR*0.2f;
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
		posx2 = maxR*0.2f; 
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
		posx3 = -maxR*0.2f;

	app->render->DrawTexture(BACK1, posx1, -380, &bgColor,1.0f,NULL, NULL, NULL);
	app->render->DrawTexture(BACK3, posx3, -380, &bgColor, 1.0f, NULL, NULL, NULL);
	app->render->DrawTexture(BACK2, posx2, -380, &bgColor, 1.0f, NULL, NULL, NULL);

	if (secret == false) {
		app->map->Draw();
		ghostCollider->body->SetActive(true);
	}
	
	else if (secret == true) {
		app->map->DrawSecret();
		ghostCollider->body->SetActive(false);
	}

	//app->entityManager->Update(dt);	

	// Win/Lose logic
	if (player->ded == true)
		contadorT++;

	if (player->ded == true && (player->ani == false || contadorT == 80)) 
		app->fade->FadingToBlack(this, (Module*)app->loseScene, 45);

	if (player->position.x > 624 && player->position.x < 895 && player->position.y > 224)
		secret = true;

	if (end == true) {
		//app->audio->PauseMusic();
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);
	}


	//intent de pathfinding

	iPoint position_P;
	position_P.x = player->pbody->body->GetTransform().p.x;
	position_P.y = player->pbody->body->GetTransform().p.y;


	
	iPoint position_E;
	position_E.x = enemy->pbody->body->GetTransform().p.x;
	position_E.y = enemy->pbody->body->GetTransform().p.y;


	enemy->State(position_P, position_E);

	
	if (enemy->chase) {
		
		app->pathfinding->CreatePath(position_E, position_P);

		const DynArray<iPoint>* path = app->pathfinding->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);

			//enemy->pbody->body->SetTransform(b2Vec2{ PIXEL_TO_METERS(pos.x),PIXEL_TO_METERS(pos.y) }, 0); faria tps

			if (pos.x < position_E.x) {
				enemy->flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
				//enemy->vel = b2Vec2(-enemy->speed, GRAVITY_Y);

				enemy->currentAnimation = &enemy->forwardAnim;
			}

			if (pos.x > position_E.x) {

				//enemy->vel = b2Vec2(enemy->speed, GRAVITY_Y);
				enemy->currentAnimation = &enemy->forwardAnim;

			}

			if (pos.y > position_E.y) {

				enemy->currentAnimation = &enemy->jumpAnim;

				enemy->pbody->body->ApplyForceToCenter(b2Vec2{ 0,10 }, 1);

			}
		}

	}


	if (enemy->idle) {

		enemy->currentAnimation = &enemy->idleAnim;
		app->pathfinding->ClearLastPath();

	}



	return true;
}

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
	gem->Disable();
	listCoins.Clear();

	app->tex->UnLoad(BACK1);
	app->tex->UnLoad(BACK2);
	app->tex->UnLoad(BACK3);
	
	app->entityManager->Disable();
	app->physics->Disable();
	app->map->CleanUp();

	return true;
}

void Scene::Debug()
{
	int a = 3 * app->win->GetScale();

	// Start again level
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->scene, 0);


	// Load / Save - keys F5 (save) / F6 (load)
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
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		app->input->godMode = !app->input->godMode;

	// Show collisions
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		app->physics->collisions = !app->physics->collisions;

	// Mute / unmute
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
		mute = !mute;

	// God mode functions
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
	}

	(mute) ? app->audio->PauseMusic() : app->audio->ResumeMusic();
}

bool Scene::InitEntities()
{
	gem = (Gem*)app->entityManager->CreateEntity(EntityType::GEM);
	gem->parameters = sceneNode.child("item2");
	gem->Awake();

	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = sceneNode.child("player");
	player->Awake();

	

	enemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY);
	enemy->parameters = sceneNode.child("enemy");
	enemy->Awake();

	coins->SpawnCoins();

	return true;
}