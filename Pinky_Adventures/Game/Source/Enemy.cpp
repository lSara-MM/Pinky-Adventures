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
#include "Window.h"

#include "FadeToBlack.h"
#include "Map.h"
#include "ItemCoin.h"
#include "ItemGem.h"
#include "EntityManager.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");


	idleAnim.PushBack({ 8, 39, 17, 28 });
	idleAnim.PushBack({ 40, 39, 17, 28 });
	idleAnim.PushBack({ 71, 38, 18, 29 });
	idleAnim.PushBack({ 102, 38, 20, 29 });

	idleAnim.speed = 0.1f;


	forwardAnim.PushBack({ 8, 142, 17, 27 });
	forwardAnim.PushBack({ 40, 141, 17, 28 });
	forwardAnim.PushBack({ 72, 141, 17, 28 });
	forwardAnim.PushBack({ 103, 142, 18, 27 });
	forwardAnim.PushBack({ 136, 141, 17, 28 });
	forwardAnim.PushBack({ 168, 141, 17, 28 });

	forwardAnim.speed = 0.1f;


	jumpAnim.PushBack({ 8, 73, 17, 28 });
	jumpAnim.PushBack({ 38, 75, 20, 26 });
	jumpAnim.PushBack({ 104, 71, 17, 30 });
	jumpAnim.PushBack({ 135, 69, 18, 29 });
	jumpAnim.PushBack({ 166, 71, 20, 29 });
	jumpAnim.PushBack({ 197, 74, 22, 27 });
	jumpAnim.PushBack({ 231, 75, 18, 26 });

	jumpAnim.speed = 0.07f;

	deathAnim.PushBack({ 4, 4, 21, 29 });
	deathAnim.PushBack({ 36, 2, 25, 28 });
	deathAnim.PushBack({ 65, 4, 25, 28 });
	deathAnim.PushBack({ 99, 4, 29, 29 });
	deathAnim.PushBack({ 129, 4, 35, 29 });
	deathAnim.PushBack({ 170, 4, 25, 29 });
	deathAnim.PushBack({ 202, 4, 25, 29 });


	deathAnim.speed = 0.1f;
	

	active = true;
}

Enemy::~Enemy() {}

bool Enemy::Awake() {

	position.x = parameters.attribute("x_E").as_int();
	position.y = parameters.attribute("y_E").as_int();

	texturePath = parameters.attribute("texturepath_E").as_string();

	speed = parameters.attribute("velocity_E").as_int();
	width = parameters.attribute("width_E").as_int();
	height = parameters.attribute("height_E").as_int();
	jumpPath = parameters.attribute("audiopathJump_E").as_string();
	landPath = parameters.attribute("audiopathLand_E").as_string();
	deathPath = parameters.attribute("audiopathDeath_E").as_string();

	return true;
}

bool Enemy::Start() {

	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateRectangle(position.x + width / 2, position.y + height / 2, width, height, bodyType::DYNAMIC);
	pbody->body->SetFixedRotation(true);

	pbody->listener = this;

	pbody->ctype = ColliderType::ENEMY;

	fxJump = app->audio->LoadFx(jumpPath);
	fxLand = app->audio->LoadFx(landPath);

	idle = true;

	chase = false;

	

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

void Enemy::State(iPoint posPlayer, iPoint posEnemy)
{

	if (posPlayer.DistanceTo(posEnemy) <= 20) {

		idle = false;
		chase = true;

	}

	else {

		idle = true;
		chase = false;
	}

}
