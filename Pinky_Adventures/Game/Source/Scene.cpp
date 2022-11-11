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

		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
		listCoins.Add(item);
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	contadorT = 0;
	//app->physics->Enable();
	//app->physics->Start();
	
	//app->entityManager->Enable();//activar aixo fa que apareixin colliders extra del player, però player va com x2 speed

	app->entityManager->Start();
	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	// L03: DONE: Load map
	app->map->Load();

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

	bgColor = { 0, 0, app->win->GetWidth() * app->win->GetScale() + 100,  app->win->GetHeight() };

	if (player->active == false) {
		player->Enable();
	}
	
	secret = false;
	//player->Start();
	

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
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += 1;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= 1;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += 1;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= 1;


	app->render->DrawRectangle(bgColor, 88, 141, 190);//orden importa



	if (secret == false) {
		app->map->Draw();
	}
	
	else if (secret == true) {
		app->map->DrawSecret();
	}

	
	app->entityManager->Update(dt);	// millor que posar-ho individual
	//player->Update();

	if (app->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
		app->fade->FadingToBlack(this, (Module*)app->iScene, 90);

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
	
	player->Disable();

	app->render->camera.x = 0;
	app->render->camera.y = 0;


	app->physics->Disable();
	//app->map->CleanUp();per algun motiu no pilla algo del tileset i peta
	app->map->UnloadCollisions();

	return true;
}
