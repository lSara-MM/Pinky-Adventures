#include "ItemGem.h"
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

Gem::Gem() : Entity(EntityType::GEM)
{
	name.Create("item2");

	GemAnim.PushBack({ 0, 0, 32, 32 });
	GemAnim.PushBack({ 32, 0, 32, 32 });
	GemAnim.PushBack({ 64, 0, 32, 32 });
	GemAnim.PushBack({ 96, 0, 32, 32 });

	GemAnim.PushBack({ 0, 0, 32, 32 });
	GemAnim.PushBack({ 0, 0, 32, 32 });
	GemAnim.PushBack({ 0, 0, 32, 32 });
	GemAnim.PushBack({ 0, 0, 32, 32 });

	GemAnim.speed = 0.1f;
}

Gem::~Gem() {}

bool Gem::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Gem::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body

	pbody = app->physics->CreateCircleSensor(position.x + 16, position.y + 16, 10, bodyType::STATIC);
	pbody->ctype = ColliderType::GEM;
	pbody->body->SetFixedRotation(true);
	isPicked = true;
	active = true;
	return true;
}

bool Gem::Update()
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

	currentAnimGem = &GemAnim;
	currentAnimGem->Update();
	SDL_Rect rect = currentAnimGem->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);

	
	return true;
}

bool Gem::CleanUp()
{
	isPicked = false;
	app->tex->UnLoad(texture);
	return true;
}
