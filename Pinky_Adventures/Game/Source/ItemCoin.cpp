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
	ID = parameters.attribute("id").as_int();

	return true;
}

bool Coin::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateCircleSensor(position.x + 8, position.y + 8, 8, bodyType::STATIC, ID);
	pbody->ctype = ColliderType::COIN;
	pbody->body->SetFixedRotation(true);
	isPicked = true;
	active = true;
	return true;
}

bool Coin::Update()
{

	if (isPicked == false)
	{
		active = false;
		pbody->body->SetActive(false);
		return true;
	}

	if ((ID <= 9 || ID >= 25) || (ID > 9 && ID < 25 && app->scene->secret == true))
	{
		currentAnimCoin = &coinAnim;
		currentAnimCoin->Update();
		SDL_Rect rect = currentAnimCoin->GetCurrentFrame();
		app->render->DrawTexture(texture, position.x, position.y, &rect);
	}
	return true;
}

bool Coin::CleanUp()
{
	isPicked = false;
	app->tex->UnLoad(texture);
	return true;
}
