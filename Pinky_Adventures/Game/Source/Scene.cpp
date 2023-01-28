#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"

#include "IntroScene.h"
#include "Scene.h"
#include "LeaderboardScene.h"
#include "EntityManager.h"
#include "Map.h"

#include "Pathfinding.h"
#include "Physics.h"
#include "FadeToBlack.h"
#include "ItemCoin.h"
#include "ItemSave.h"
#include "ItemHealth.h"

#include "Defs.h"
#include "Log.h"

#include <iostream>
using namespace std;
#include <sstream>

Scene::Scene() : Module()
{
	name.Create("scene");

	coinAnim.PushBack({ 0, 0, 16, 16 });
	coinAnim.PushBack({ 16, 0, 16, 16 });
	coinAnim.PushBack({ 32, 0, 16, 16 });
	coinAnim.PushBack({ 48, 0, 16, 16 });

	coinAnim.speed = 0.1f;


	heartAnim.PushBack({ 1, 1, 16, 16 });
	heartAnim.PushBack({ 19, 1, 16, 16 });
	heartAnim.PushBack({ 37, 1, 16, 16 });
	heartAnim.PushBack({ 55, 1, 16, 16 });
	heartAnim.PushBack({ 73, 1, 16, 16 });
	heartAnim.PushBack({ 91, 1, 16, 16 });
	heartAnim.PushBack({ 109, 1, 16, 16 });
	heartAnim.PushBack({ 127, 1, 16, 16 });

	heartAnim.speed = 0.1f;
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
	
	coinPath = config.attribute("coinspath").as_string();
	heartPath = config.attribute("heartspath").as_string();

	sceneNode = config;
	return ret;
}

bool Scene::Start()
{
	timerLvl1.Start();
	timeLeft = 60.0;
	timeElapsed = 0.0;

	// IMPORTANT, DO NOT SET ID's TO 0;
	coinIDset = 1;
	enemyIDset = 1;
	healthIDset = 1;


	//pause menu
	pause = false;

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
	coin_tex = app->tex->Load(coinPath); 
	heart_tex = app->tex->Load(heartPath);


	app->audio->PlayMusic(musicPathBg, 0);
	
	mute = false;
	end = false;
	drawPaths = false;
	frcap = true;
	freeCam = false;
	app->input->godMode = false;

	if (app->iScene->loaded)
	{
		app->LoadGameRequest();
	}

	tempo.Start();
	
	return true;
}

bool Scene::PreUpdate()
{
	return true;
}

bool Scene::Update(float dt)
{
	// Move camera with player
	float speed = dt / 1000;
	
	int maxR = -player->position.x * app->win->GetScale() + 300;
	if (!freeCam)
	{
		if (-maxR < maxCameraPosRigth - app->render->camera.w && -maxR > maxCameraPosLeft)
		{
			app->render->camera.x = ceil(maxR);
		}
		else if(maxR>=-3200){
			app->render->camera.x = 0;
		}
	}

	// Background parallax
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft) {
		//posx1 = maxR * speed;
	}
		
	else if (maxR >= -3200) {
		posx1 = 0;
	}

	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft) {
		//posx2 = maxR * speed;

	}
	else if (maxR >= -3200) {
		posx2 = 0;
	}
	if (-maxR < app->scene->maxCameraPosRigth - app->render->camera.w && -maxR > app->scene->maxCameraPosLeft) {

		//posx3 = maxR*speed;

	}
	else if (maxR >= -3200) {
		posx3 = 0;
	}

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

	if (player->lives > 1 && player->ded == true && contadorT == 80)
	{
		if (!save->isPicked)
		{
			player->pbody->body->SetTransform({ PIXEL_TO_METERS(120),PIXEL_TO_METERS(260) }, 0);
		}
		
		else if (save->isPicked)
		{
			player->pbody->body->SetTransform({ PIXEL_TO_METERS(save->position.x),PIXEL_TO_METERS(save->position.y) }, 0);
		}
		
		player->currentAnimation->current_frame = 0;
		player->ded = false;
		contadorT = 0;
		player->lives--;	
	}

	else if (player->ded == true && contadorT == 80)
	{
		app->fade->FadingToBlack(this, (Module*)app->loseScene, 45);
		if (app->leadScene->leaderboard[9] < player->score) { app->leadScene->leaderboard[9] = player->score; }
		app->leadScene->currentScore = player->score;
	}

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

		app->render->DrawTexture(attackIcon, -app->render->camera.x * 0.5, 0, &attackCajaCd, 1.0f, NULL, NULL, NULL);
	}
	
	return true;
}

bool Scene::PostUpdate()
{
	bool ret = true;

	int fontsize = 10;

	// render score
	string s_score = std::to_string(player->score);
	const char* ch_score = s_score.c_str();

	app->render->TextDraw("Score:", 50, 3, fontsize, { 0, 0, 0 });
	app->render->TextDraw(ch_score, 120, 3, fontsize, { 0, 0, 0 });
	

	// coins
	string s_coins = std::to_string(player->coins);
	const char* ch_coins = s_coins.c_str();

	currentAnimCoin = &coinAnim;
	currentAnimCoin->Update();
	SDL_Rect rect = currentAnimCoin->GetCurrentFrame();
	app->render->DrawTexture(coin_tex, -app->render->camera.x * 0.5 + 50, 17, &rect);
	app->render->TextDraw("x", 75, 20, fontsize, { 0, 0, 0 });
	app->render->TextDraw(ch_coins, 95, 20, fontsize, { 0, 0, 0 });


	// heart
	string s_hearts = std::to_string(player->lives);
	const char* ch_hearts = s_hearts.c_str();

	currentAnimHeart = &heartAnim;
	currentAnimHeart->Update();
	SDL_Rect rect2 = currentAnimHeart->GetCurrentFrame();
	app->render->DrawTexture(heart_tex, -app->render->camera.x * 0.5 + 50, 40, &rect2);
	app->render->TextDraw("x", 75, 43, fontsize, { 0, 0, 0 });
	app->render->TextDraw(ch_hearts, 95, 43, fontsize, { 0, 0, 0 });


	// time
	
	string s_TIME = std::to_string(app->secondsSinceStartupTempo);
	const char* ch_TIME = s_TIME.c_str();
	app->render->TextDraw("Time:", 415, 20, fontsize, { 0, 0, 0 });
	app->render->TextDraw(ch_TIME, 480, 20, fontsize, { 0, 0, 0 });


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
	listHealth.Clear();

	app->tex->UnLoad(BACK1);
	app->tex->UnLoad(BACK2);
	app->tex->UnLoad(BACK3);
	app->tex->UnLoad(attackIcon);

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

	if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		if (app->leadScene->leaderboard[9] < player->score) { app->leadScene->leaderboard[9] = player->score; }
		app->leadScene->currentScore = player->score;

		player->Disable();

		app->fade->FadingToBlack(this, (Module*)app->leadScene, 0);
	}

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
		//player->ded = true;
		//app->audio->PlayFx(player->fxDeath);
		app->fade->FadingToBlack(this, (Module*)app->loseScene, 0);
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

	// Enable/Disable Frcap
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		frcap = !frcap;
		LOG("frame rate: %d", app->physics->frameRate);
	}
	
	
	//pause menu
	if (app->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
	{
		pause = !pause;
		x = !x;
		LOG("PAUSE");
	}
	if (x)
	{
		app->iScene->OpenSettings();
	}
	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		pause = !pause;
		LOG("PAUSE");
	}
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
			app->render->camera.y = 0;
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

	portal = (Portal*)app->entityManager->CreateEntity(EntityType::PORTAL);
	portal->parameters = sceneNode.child("portal");
	portal->Awake();


	save =(Save*)app->entityManager->CreateEntity(EntityType::SAVE);
	save->parameters = sceneNode.child("save");
	save->Awake();


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
	health->SpawnHealth();

	return true;
}