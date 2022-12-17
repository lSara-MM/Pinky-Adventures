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
#include "Pathfinding.h"

#include "Scene.h"
#include "Map.h"
#include "EntityManager.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("enemy");

	idleFlyingEnemyAnim.PushBack({ 0, 3, 16, 11 });
	idleFlyingEnemyAnim.PushBack({ 0, 20, 16, 7 });
	idleFlyingEnemyAnim.PushBack({ 0, 35, 16, 10 });
	idleFlyingEnemyAnim.PushBack({ 0, 49, 16, 13 });

	idleFlyingEnemyAnim.speed = 0.1f;

	deathAnim.PushBack({ 0, 20, 16, 7 });


	ForwardWalkingEnemyAnim.PushBack({ 128, 0, 16, 16 });
	ForwardWalkingEnemyAnim.PushBack({ 144, 0, 16, 16 });
	ForwardWalkingEnemyAnim.PushBack({ 160, 0, 16, 16 });
	ForwardWalkingEnemyAnim.PushBack({ 176, 0, 16, 16 });
	ForwardWalkingEnemyAnim.PushBack({ 192, 0, 16, 16 });
	ForwardWalkingEnemyAnim.PushBack({ 208, 0, 16, 16 });
	ForwardWalkingEnemyAnim.PushBack({ 224, 0, 16, 16 });
	ForwardWalkingEnemyAnim.PushBack({ 240, 0, 16, 16 });

	ForwardWalkingEnemyAnim.speed = 0.1f;

	idleWalkingEnemyAnim.PushBack({ 128, 16, 16, 16 });
	idleWalkingEnemyAnim.PushBack({ 144, 16, 16, 16 });
	idleWalkingEnemyAnim.PushBack({ 160, 16, 16, 16 });
	idleWalkingEnemyAnim.PushBack({ 176, 16, 16, 16 });
	idleWalkingEnemyAnim.PushBack({ 192, 16, 16, 16 });
	idleWalkingEnemyAnim.PushBack({ 208, 16, 16, 16 });
	idleWalkingEnemyAnim.PushBack({ 224, 16, 16, 16 });
	idleWalkingEnemyAnim.PushBack({ 240, 16, 16, 16 });

	idleWalkingEnemyAnim.speed = 0.1f;

	state = eState::IDLE;
	active = true;
}

Enemy::~Enemy() {}

bool Enemy::Awake() {

	position.x = parameters.attribute("x_E").as_int();
	position.y = parameters.attribute("y_E").as_int();
	
	SString typeConfig = parameters.attribute("type_E").as_string();
	if (typeConfig == "basic") { type = eType::BASIC; }
	else if (typeConfig == "flying") { type = eType::FLYING; }
	else { type = eType::UNKNOWN; }

	texturePathEnemy = parameters.attribute("texturepath_E").as_string();

	speed = parameters.attribute("velocity_E").as_int();
	width = parameters.attribute("width_E").as_int();
	height = parameters.attribute("height_E").as_int();
	jumpPath = parameters.attribute("audiopathJump_E").as_string();
	landPath = parameters.attribute("audiopathLand_E").as_string();
	deathPath = parameters.attribute("audiopathDeath_E").as_string();

	detectionDistance = parameters.attribute("detectionDistance").as_int();

	isFlying = parameters.attribute("fly").as_bool();

	pos_Origin = position;
	grav = GRAVITY_Y;
	return true;
}

bool Enemy::Start() {
	
	textureEnemy = app->tex->Load(texturePathEnemy);
	ID = app->scene->enemyIDset++;

	pbody = app->physics->CreateRectangle(position.x + width / 2, position.y + height / 2, width, height, bodyType::DYNAMIC, ID);
	
	if (isFlying) {
		pbody->body->SetGravityScale(0);
	}
	

	pbody->body->SetFixedRotation(true);
	pbody->ctype = ColliderType::ENEMY;

	headSensor = app->physics->CreateRectangleSensor((position.x + width / 2), (position.y + height / 2) - 7, 3, 3, bodyType::KINEMATIC, ID);
	headSensor->body->SetFixedRotation(true);
	headSensor->ctype = ColliderType::ENEMY_WP;

	fxJump = app->audio->LoadFx(jumpPath);
	fxLand = app->audio->LoadFx(landPath);
	fxDeath_Enemy = app->audio->LoadFx(deathPath);
	
	origin = true;

	return true;
}

bool Enemy::Update()
{
	b2Vec2 vel = b2Vec2(0, 0);

	if (isFlying) {

		currentAnimation = &idleFlyingEnemyAnim;
		
	}

	else if(!isFlying){
		currentAnimation = &idleWalkingEnemyAnim;
		vel = b2Vec2(0, grav);
	}


	//Set the velocity of the pbody of the player
	//pbody->body->SetLinearVelocity(vel);

	//intent de pathfinding

	pos_Enemy = app->map->WorldToMap(position.x, position.y);
	pos_Player = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);


	LOG("distance %d", pos_Player.DistanceTo(pos_Enemy));
	if (state != eState::DEAD && pos_Player.DistanceTo(pos_Enemy) <= detectionDistance)
	{
		state = eState::CHASE;
		LOG("PLAYER DETECTED");
	}
	else if (state != eState::DEAD)
	{ 
		state = eState::IDLE;
		LOG("LOST PLAYER'S TRACK");
	}

	State(pos_Player, pos_Enemy, vel);

	//Update enemy position in pixels
	pbody->body->SetLinearVelocity(vel);

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - width / 2;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - height / 2;

	//headSensor->body->SetLinearVelocity(vel);
	headSensor->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x) + 0.17f, PIXEL_TO_METERS(position.y)), 0);

	currentAnimation->Update();

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(textureEnemy, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, flipType);
	
	return true;
}

bool Enemy::CleanUp()
{
	app->tex->UnLoad(textureEnemy);
	pbody->body->GetWorld()->DestroyBody(pbody->body);
	headSensor->body->GetWorld()->DestroyBody(headSensor->body);
	return true;
}

void Enemy::State(iPoint posPlayer, iPoint posEnemy, b2Vec2 &vel)
{
	const DynArray<iPoint>* path = nullptr;

	switch (state)
	{
	case eState::IDLE:
		switch (type)
		{
		case eType::BASIC:
			break;
		case eType::FLYING:
			pbody->body->SetLinearVelocity(b2Vec2(0, 0));
			break;
		case eType::UNKNOWN:
			break;
		default:
			break;
		}
		break;

	case eState::CHASE:

		/*app->pathfinding->CreatePath(pos_Enemy, pos_Player);
		path = app->pathfinding->GetLastPath();

		if (path->At(0) != NULL)
		{
			iPoint pos = app->map->MapToWorld(path->At(0)->x, path->At(0)->y);

			if (pos_Enemy.x < pos_Player.x) {
				flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
				vel = b2Vec2(speed, 0);
			}

			if (pos_Enemy.x > pos_Player.x) {
				flipType = SDL_RendererFlip::SDL_FLIP_NONE;
				vel = b2Vec2(-speed, 0);
			}

			if (pos_Enemy.y < pos_Player.y) { vel = b2Vec2(0, speed); }
			if (pos_Enemy.y > pos_Player.y) { vel = b2Vec2(0, -speed); }
		}*/

		app->pathfinding->CreatePath(pos_Enemy, pos_Player);
		path = app->pathfinding->GetLastPath();

		if (path->At(1) != NULL)
		{
			 posPath = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);
			 posPath_0 = app->map->MapToWorld(path->At(0)->x, path->At(0)->y);

			 if (isFlying) {

				 if (posPath_0.x < posPath.x) {

					 flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
					 vel = b2Vec2(speed, 0);
				 }
				 else if (posPath_0.x > posPath.x) {

					 flipType = SDL_RendererFlip::SDL_FLIP_NONE;
					 vel = b2Vec2(-speed, 0);
				 }
				 else if (posPath_0.y < posPath.y)
				 {
					 vel = b2Vec2(0, speed);
				 }
				 else if (posPath_0.y > posPath.y) {
					 vel = b2Vec2(0, -speed);
				 }
			}

			 else if(!isFlying) {

				 if (posPath_0.x < posPath.x) {

					 flipType = SDL_RendererFlip::SDL_FLIP_NONE;
					 currentAnimation = &ForwardWalkingEnemyAnim;
					
					 vel = b2Vec2(speed, grav);
				 }
				 else if (posPath_0.x > posPath.x) {

					 currentAnimation = &ForwardWalkingEnemyAnim;
					 flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
				
					 vel = b2Vec2(-speed, grav);
				 }
				
			 }
			
		}
		break;

	case eState::DEAD:
		//active = false;
		flipType = SDL_RendererFlip::SDL_FLIP_VERTICAL;
		currentAnimation = &deathAnim;
		//pbody->body->SetActive(false);	// si no esta comentat lo bicho se queda mort a l'aire :/
		headSensor->body->SetActive(false);
		break;

	default:
		break;
	}

	//DEBUG
	if (path != nullptr)
	{
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			TileSet* tileset = app->map->GetTilesetFromTileId(enGID);
			SDL_Rect r = tileset->GetTileRect(enGID);
			app->render->DrawTexture(tileset->texture, pos.x, pos.y, &r);
		}

		iPoint en = app->map->MapToWorld(pos_Enemy.x, pos_Enemy.y);
		iPoint pl = app->map->MapToWorld(pos_Player.x, pos_Player.y);

		app->render->DrawLine(en.x + pbody->width / 2, en.y + pbody->height / 2,
			pl.x + METERS_TO_PIXELS(vel.x / (pbody->width / 2)) + pbody->width / 2,
			pl.y + METERS_TO_PIXELS(vel.y / (pbody->height / 2)) + pbody->width / 2,
			0, 255, 255);
	}
	
}
