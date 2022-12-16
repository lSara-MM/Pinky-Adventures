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

#include "FadeToBlack.h"
#include "Map.h"
#include "EntityManager.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("enemy");

	idleAnim.PushBack({ 0, 3, 16, 11 });
	idleAnim.PushBack({ 0, 20, 16, 7 });
	idleAnim.PushBack({ 0, 35, 16, 10 });
	idleAnim.PushBack({ 0, 49, 16, 13 });

	idleAnim.speed = 0.1f;

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

	texturePathFlyingEnemy = parameters.attribute("texturepath_E").as_string();

	speed = parameters.attribute("velocity_E").as_int();
	width = parameters.attribute("width_E").as_int();
	height = parameters.attribute("height_E").as_int();
	jumpPath = parameters.attribute("audiopathJump_E").as_string();
	landPath = parameters.attribute("audiopathLand_E").as_string();
	deathPath = parameters.attribute("audiopathDeath_E").as_string();

	detectionDistance = parameters.attribute("detectionDistance").as_int();

	pos_Origin = position;
	grav = GRAVITY_Y;
	return true;
}

bool Enemy::Start() {
	
	textureFlyingEnemy = app->tex->Load(texturePathFlyingEnemy);

	pbody = app->physics->CreateRectangle(position.x + width / 2, position.y + height / 2, width, height, bodyType::DYNAMIC);
	pbody->body->SetGravityScale(0);
	pbody->body->SetFixedRotation(true);
	pbody->ctype = ColliderType::ENEMY;

	fxJump = app->audio->LoadFx(jumpPath);
	fxLand = app->audio->LoadFx(landPath);
	
	ID = app->scene->enemyIDset++;
	origin = true;
	return true;
}

bool Enemy::Update()
{
	currentAnimation = &idleAnim;
	b2Vec2 vel = b2Vec2(0, 0);

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//intent de pathfinding

	pos_Enemy = app->map->WorldToMap(position.x, position.y);
	pos_Player = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);


	LOG("distance %d", pos_Player.DistanceTo(pos_Enemy));
	if (pos_Player.DistanceTo(pos_Enemy) <= detectionDistance)
	{
		state = eState::CHASE;
		LOG("PLAYER DETECTED");
	}
	else 
	{ 
		state = eState::IDLE;
		LOG("LOST PLAYER'S TRACK");
	}

	State(pos_Player, pos_Enemy, vel);

	//if (chase) {
	//	app->pathfinding->CreatePath(pos_Enemy, pos_Player);
	//	const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
	//	if (path->At(0) != NULL) {
	//
	//		iPoint pos = app->map->MapToWorld(path->At(0)->x, path->At(0)->y);
	//
	//		if (pos.x < pos_Enemy.x) {
	//
	//			flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	//			vel = b2Vec2(-speed, 0);
	//
	//		}
	//
	//		if (pos.x > pos_Enemy.x) {
	//
	//			flipType = SDL_RendererFlip::SDL_FLIP_NONE;
	//			vel = b2Vec2(speed, 0);
	//		}
	//
	//		if (pos.y < pos_Enemy.y) {
	//
	//			vel = b2Vec2(0, -speed);
	//
	//		}
	//
	//		if (pos.y > pos_Enemy.y) {
	//
	//			vel = b2Vec2(0, speed);
	//
	//		}
	//	}
	//	
	//
	//	//DEBUG
	//	for (uint i = 0; i < path->Count(); ++i)
	//	{
	//		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
	//		TileSet * tileset = app->map->GetTilesetFromTileId(enGID);
	//		SDL_Rect r = tileset->GetTileRect(enGID);
	//		app->render->DrawTexture(tileset->texture, pos.x, pos.y, &r);
	//
	//	}
	//}
	//
	//if (idle) {
	//	app->pathfinding->ClearLastPath();
	//}
	// 
		

	//Update enemy position in pixels
	pbody->body->SetLinearVelocity(vel);

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - width / 2;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - height / 2;

	currentAnimation->Update();

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(textureFlyingEnemy, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, flipType);
	
	return true;
}

bool Enemy::CleanUp()
{
	
	pbody->body->GetWorld()->DestroyBody(pbody->body);

	return true;
}

void Enemy::State(iPoint posPlayer, iPoint posEnemy, b2Vec2 &vel)
{
	/*if (posPlayer.DistanceTo(posEnemy) <= detectionDistance) {
		idle = false;
		chase = true;
	}
	else {
		idle = true;
		chase = false;
	}*/

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

		app->pathfinding->CreatePath(pos_Enemy, pos_Player);
		path = app->pathfinding->GetLastPath();

		if (path->At(1) != NULL)
		{
			 posPath = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);

			if (pos_Enemy.x < posPath.x) {
				flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
				vel = b2Vec2(speed, 0);
			}

			if (pos_Enemy.x > posPath.x) {
				flipType = SDL_RendererFlip::SDL_FLIP_NONE;
				vel = b2Vec2(-speed, 0);
			}

			if (pos_Enemy.y < posPath.y) { vel = b2Vec2(0, speed); }
			if (pos_Enemy.y > posPath.y) { vel = b2Vec2(0, -speed); }

		}
		break;

	case eState::DEAD:
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
