#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"


#include "Defs.h"
#include "Log.h"
#include "EntityManager.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");

	

	active = true;
}

Enemy::~Enemy() {}

bool Enemy::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	//texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Enemy::Start() {

	//initilize textures
	//texture = app->tex->Load(texturePath);

	return true;
}

bool Enemy::Update()
{
	
	return true;
}

bool Enemy::CleanUp()
{
	
	//pbody->body->GetWorld()->DestroyBody(pbody->body);

	return true;
}
