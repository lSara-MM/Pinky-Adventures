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
	attackIconPath = config.attribute("textureAttackPath").as_string();
	
	musicPathBg = config.attribute("music").as_string();
	return ret;
}

bool Scene::Start()
{
	// IMPORTANT, DO NOT SET ID's TO 0;
	coinIDset = 1;
	enemyIDset = 1;


	app->physics->Enable();
	app->render->camera.x = 0;
	app->render->camera.y = 0;

	posx1 = 0;
	posx2 = 0;
	posx3 = 0;

	contadorT = 0;
	
	// Load map
	bool retLoad = app->map->Load();

	// Create walkability map
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

	attackCajaNoCd = { 0, 0, 32,  32 };

	attackCajaCd = { 128, 0, 32,  32 };

	// Background
	BACK1 = app->tex->Load(back1Path);
	BACK2 = app->tex->Load(back2Path);
	BACK3 = app->tex->Load(back3Path);

	attackIcon = app->tex->Load(attackIconPath);

	app->audio->PlayMusic(musicPathBg, 0);
	
	mute = false;
	end = false;
	drawPaths = false;
	frame30 = false;
	freeCam = false;
	return true;
}

bool Scene::PreUpdate()
{
	return true;
}

bool Scene::Update(float dt)
{
	// Move camera with player
	float speed = 1 * dt;
	player->dt = dt;
	int maxR = -player->position.x * app->win->GetScale() + 300;
	if (!freeCam)
	{
		if (-maxR < maxCameraPosRigth - app->render->camera.w && -maxR > maxCameraPosLeft)
		{
			app->render->camera.x = ceil(maxR);
		}
	}

	// Background parallax
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
		posx1 = maxR * 0.2f;
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
		posx2 = maxR * 0.2f;
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft)
		posx3 = -maxR * 0.2f;

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

	Debug();

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

	if (player->contadorCooldown==player->attackCooldown) {

		app->render->DrawTexture(attackIcon, -app->render->camera.x*0.5, 0, &attackCajaNoCd, 1.0f, NULL, NULL, NULL);

	}

	if (player->contadorCooldown != player->attackCooldown) {

		app->render->DrawTexture(attackIcon, -app->render->camera.x*0.5, 0, &attackCajaCd, 1.0f, NULL, NULL, NULL);

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
	listEnemies.Clear();

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

	// Show collisions
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		app->physics->collisions = !app->physics->collisions;
		drawPaths = !drawPaths;
	}

	// GodMode
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) 
	{ 
		app->input->godMode = !app->input->godMode; 
		app->physics->collisions = !app->physics->collisions;
		drawPaths = !drawPaths;
	}

	// Enable/Disable FPS cap to 30
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		frame30 = !frame30;
		LOG("frame rate: %d", app->physics->frameRate);
	}
	(frame30) ? app->physics->frameRate = 60.0f : app->physics->frameRate = 30.0f;

	// Mute / unmute
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
		mute = !mute;

	// God mode functions
	if (app->input->godMode)
	{
		// Free camera
		if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			app->render->camera.y += a;
			freeCam = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			app->render->camera.y -= a;
			freeCam = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			app->render->camera.x += a;
			freeCam = true;
		}
		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			app->render->camera.x -= a;
			freeCam = true;
		}

		// Reset camera
		if (app->input->GetKey(SDL_SCANCODE_C) == KEY_REPEAT)
		{
			int maxR = -player->position.x * app->win->GetScale() + 300;
			app->render->camera.x = maxR;
		}
	}

	if (drawPaths)
		app->map->DrawPaths();

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


	for (pugi::xml_node itemNode = sceneNode.child("enemy"); itemNode; itemNode = itemNode.next_sibling("enemy"))
	{
		enemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY);
		enemy->parameters = itemNode;
		enemy->Awake();
		listEnemies.Add(enemy);
	}

	coins->SpawnCoins();

	return true;
}