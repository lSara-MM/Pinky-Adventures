#include "ItemCoin.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

#include "EntityManager.h"

Coin::Coin() : Entity(EntityType::COIN)
{
	name.Create("item");

	coinAnim.PushBack({ 0, 0, 16, 16 });
	coinAnim.PushBack({ 16, 0, 16, 16 });
	coinAnim.PushBack({ 32, 0, 16, 16 });
	coinAnim.PushBack({ 48, 0, 16, 16 });

	coinAnim.speed = 0.1f;


}

Coin::~Coin() {}

bool Coin::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Coin::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body

	pbody = app->physics->CreateCircleSensor(position.x + 8, position.y + 8, 8, bodyType::STATIC);
	pbody->ctype = ColliderType::COIN;
	pbody->body->SetFixedRotation(true);
	isPicked = true;
	return true;
}

bool Coin::Update()
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  

	//position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	//position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	if (isPicked == false)
	{
		active = false;
		pbody->body->SetActive(false);
		return true;
	}

	currentAnimCoin = &coinAnim;
	currentAnimCoin->Update();
	SDL_Rect rect = currentAnimCoin->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);

	
	return true;
}

bool Coin::CleanUp()
{
	isPicked = false;
	app->tex->UnLoad(texture);
	return true;
}