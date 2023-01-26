#include "ItemSave.h"
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

Save::Save() : Entity(EntityType::SAVE)
{
	name.Create("save");

	SaveIdleAnim.PushBack({ 4, 7, 21, 26 });
	SaveIdleAnim.PushBack({ 37, 6, 20, 27 });
	SaveIdleAnim.PushBack({ 68, 6, 21, 27 });
	SaveIdleAnim.PushBack({ 100, 7, 22, 26 });

	SaveIdleAnim.speed = 0.1f;


	SaveCheckAnim.PushBack({ 33, 39, 22, 28 });
	SaveCheckAnim.PushBack({ 33, 36, 25, 29 });
	SaveCheckAnim.PushBack({ 67, 39, 26, 27 });
	SaveCheckAnim.PushBack({ 99, 39, 29, 28 });
	SaveCheckAnim.PushBack({ 129, 39, 35, 28 });
	SaveCheckAnim.PushBack({ 170, 39, 25, 28 });
	SaveCheckAnim.PushBack({ 202, 39, 25, 28 });
	SaveCheckAnim.PushBack({ 0, 0, 0, 0 });

	SaveCheckAnim.speed = 0.1f;
	SaveCheckAnim.loop = false;
	
}

Save::~Save() {}

bool Save::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	width = parameters.attribute("width").as_int();
	height = parameters.attribute("height").as_int();
	fxPath = parameters.attribute("audiopath").as_string();

	fxSave = app->audio->LoadFx(fxPath);


	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Save::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	pbody = app->physics->CreateRectangleSensor(position.x + width / 2, position.y + height / 2, width, height, bodyType::STATIC);
	pbody->ctype = ColliderType::SAVE;
	pbody->body->SetFixedRotation(true);
	isPicked = false;
	
	return true;
}

bool Save::Update()
{
	if (isPicked == true)
	{
		
		pbody->body->SetActive(false);
		currentAnimSave = &SaveCheckAnim;
	
	}
	else {

		currentAnimSave = &SaveIdleAnim;

	}
	currentAnimSave->Update();
	flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	SDL_Rect rect = currentAnimSave->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect,1.0f,NULL,NULL,NULL, flipType);
	
	return true;
}

bool Save::CleanUp()
{
	isPicked = true;
	app->tex->UnLoad(texture);
	pbody->body->GetWorld()->DestroyBody(pbody->body);

	return true;
}
