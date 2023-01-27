#include "ItemPortal.h"
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

Portal::Portal() : Entity(EntityType::PORTAL)
{
	name.Create("portal");

	PortAnim.PushBack({ 12, 14, 29, 43 });
	PortAnim.PushBack({ 87, 16, 19, 41 });
	PortAnim.PushBack({ 150, 14, 25, 43 });
	PortAnim.PushBack({ 210, 16, 30, 41 });

	PortAnim.PushBack({ 275, 15, 25, 42 });
	PortAnim.PushBack({ 333, 16, 28, 44 });
	PortAnim.PushBack({ 405, 17, 17, 40 });
	PortAnim.PushBack({ 467, 16, 19, 41 });

	PortAnim.speed = 0.1f;

	active = true;
}

Portal::~Portal() {}

bool Portal::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	width = parameters.attribute("width").as_int();
	height = parameters.attribute("height").as_int();

	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Portal::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	pbody = app->physics->CreateRectangleSensor(position.x + width, position.y + height / 2, width, height, bodyType::STATIC);
	pbody->ctype = ColliderType::PORTAL;
	pbody->body->SetFixedRotation(true);
	isPicked = true;
	active = true;
	return true;
}

bool Portal::Update(float dt)
{
	if (isPicked == false)
	{
		active = false;
		pbody->body->SetActive(false);
		return true;
	}

	currentAnimPort = &PortAnim;
	if (!app->scene->pause)
	{
		currentAnimPort->Update();
	}
	flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	SDL_Rect rect = currentAnimPort->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect,1.0f,NULL,NULL,NULL, flipType);
	
	return true;
}

bool Portal::CleanUp()
{
	isPicked = false;
	app->tex->UnLoad(texture);
	pbody->body->GetWorld()->DestroyBody(pbody->body);

	return true;
}
