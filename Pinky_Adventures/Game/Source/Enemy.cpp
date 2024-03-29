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

	deathFlyEnemyAnim.PushBack({ 0, 20, 16, 7 });

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

	deathWalkEnemyAnim.PushBack({ 128, 0, 16, 16 });

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
	deathPath = parameters.attribute("audiopathDeath_E").as_string();

	detectionDistance = parameters.attribute("detectionDistance").as_int();

	originPos = position;
	grav = GRAVITY_Y;
	return true;
}

bool Enemy::Start() {
	
	textureEnemy = app->tex->Load(texturePathEnemy);
	ID = app->scene->enemyIDset++;

	pbody = app->physics->CreateRectangle(position.x + width / 2, position.y + height / 2, width, height, bodyType::DYNAMIC, ID);
	
	if (type == eType::FLYING) { pbody->body->SetGravityScale(0); }
	/*else if (type == eType::BASIC) { pbody->body->SetGravityScale(15); }*/


	pbody->body->SetFixedRotation(true);
	pbody->ctype = ColliderType::ENEMY;

	pbody->listener = this;

	headSensor = app->physics->CreateRectangleSensor((position.x + width / 2), (position.y + height / 2) - 7, 3, 3, bodyType::KINEMATIC, ID);
	headSensor->body->SetFixedRotation(true);
	headSensor->ctype = ColliderType::ENEMY_WP;

	fxDeath_Enemy = app->audio->LoadFx(deathPath);
	
	origin = true;
	return true;
}

bool Enemy::Update(float dt)
{

	if (app->scene->pause)
	{
		dtE = 0;
	}
	else if(!app->scene->pause)
	{
		dtE = dt / 1000;
	}

	if (type == eType::BASIC) { pbody->body->SetGravityScale(750*dtE); }

	b2Vec2 vel = b2Vec2(0, 0);

	pos_Enemy = app->map->WorldToMap(position.x, position.y);
	pos_Player = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);
	pos_Origin = app->map->WorldToMap(originPos.x, originPos.y);

	iPoint idleWalk = {100, 0};
	iPoint pos_IdleWalk = app->map->WorldToMap(originPos.x + idleWalk.x, originPos.y + idleWalk.y);

	switch (state)
	{
	case eState::IDLE:
		if (pos_Player.DistanceTo(pos_Enemy) <= detectionDistance)
		{
			state = eState::CHASE;
			LOG("PLAYER DETECTED");
		}

		switch (type)
		{
		case eType::BASIC:
			currentAnimation = &idleWalkingEnemyAnim;
			//vel = b2Vec2(speed, grav);
			break;

		case eType::FLYING:
			currentAnimation = &idleFlyingEnemyAnim;

			if (pos_Enemy != pos_IdleWalk && origin) { CreatePath(pos_IdleWalk, pos_Enemy, vel); }
			else if (pos_Enemy != pos_Origin && !origin) { CreatePath(pos_Origin, pos_Enemy, vel); }
			else if (pos_Enemy == pos_IdleWalk) { origin = false; }
			else if (pos_Enemy == pos_Origin) { origin = true; }	
			break;

		case eType::UNKNOWN:
			break;

		default:
			break;
		}
		break;

	case eState::CHASE:
		if (pos_Player.DistanceTo(pos_Enemy) > detectionDistance)
		{
			state = eState::RETURN;
			LOG("LOST PLAYER'S TRACK");
			break;
		}

		CreatePath(pos_Player, pos_Enemy, vel);
		break;

	case eState::DEAD:
		//active = false;
		flipType = SDL_RendererFlip::SDL_FLIP_VERTICAL;

		switch (type)
		{
		case eType::BASIC:
			currentAnimation = &deathWalkEnemyAnim;
			break;
		case eType::FLYING:
			currentAnimation = &deathFlyEnemyAnim;
			break;
		case eType::UNKNOWN:
			break;
		default:
			break;
		}
		headSensor->body->SetActive(false);
		break;

	case eState::RETURN:
		//(pos_Enemy.x != pos_Origin.x) ? CreatePath(pos_Origin, pos_Enemy, vel) : state = eState ::IDLE;	// per motius X, l'enemy serp no arriba mai al punt d'origen :/
		if (type == eType::BASIC)
		{
			state = eState::IDLE; 
			currentAnimation = &ForwardWalkingEnemyAnim;
		}
		if (type == eType::FLYING)
		{
			(pos_Enemy.x != pos_Origin.x) ? CreatePath(pos_Origin, pos_Enemy, vel) : state = eState::IDLE; 
			currentAnimation = &idleFlyingEnemyAnim;
		}

		break;

	default:
		break;
	}


	//Update enemy position in pixels
	vel = b2Vec2(vel.x * dtE, vel.y * dtE);
	pbody->body->SetLinearVelocity(vel);

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - width / 2;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - height / 2;

	headSensor->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x) + 0.17f, PIXEL_TO_METERS(position.y)), 0);

	if (!app->scene->pause)
	{
		currentAnimation->Update();
	}
	
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

void Enemy::CreatePath(iPoint posPlayer, iPoint posEnemy, b2Vec2 &vel)
{
	const DynArray<iPoint>* path = nullptr;

	app->pathfinding->CreatePath(posEnemy, posPlayer);
	path = app->pathfinding->GetLastPath();

	if (path->At(1) != NULL)
	{
		posPath = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);
		posPath_0 = app->map->MapToWorld(path->At(0)->x, path->At(0)->y);

		switch (type)
		{
		case eType::BASIC:
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

			break;

		case eType::FLYING:
			currentAnimation = &idleFlyingEnemyAnim;

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
			break;

		case eType::UNKNOWN:
			break;

		default:
			break;
		}
	}

	//DEBUG
	if (app->input->godMode)
	{
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			TileSet* tileset = app->map->GetTilesetFromTileId(enGID);
			SDL_Rect r = tileset->GetTileRect(enGID);
			app->render->DrawTexture(tileset->texture, pos.x, pos.y, &r);
		}
		
		SDL_Rect rect = { pos_Enemy.x - detectionDistance / 2, pos_Enemy.y - detectionDistance / 2, METERS_TO_PIXELS(detectionDistance),  METERS_TO_PIXELS(detectionDistance) };
		//app->render->DrawRectangle(rect, 255, 0, 100, 255, false);
		//iPoint pos = ;
		iPoint a = app->map->MapToWorld(detectionDistance * app->win->GetScale(), detectionDistance * app->win->GetScale());
		app->render->DrawCircle((position.x + width)* app->win->GetScale(), (position.y + height) * app->win->GetScale(), a.x, 255, 100, 100);
	}
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	ListItem<Enemy*>* e;
	e = app->scene->listEnemies.start;
	if (e->data->state != eState::DEAD)
	{
		switch (physB->ctype)
		{
		case ColliderType::FALL:
			LOG("Collision ENEMY FALL");
			
			for (e; e != NULL; e = e->next)
			{
				if (e->data->ID == physA->id)
				{
					e->data->state = eState::DEAD;
					e->data->pbody->body->SetGravityScale(750 *dtE);
					break;
				}
			}

			app->audio->PlayFx(app->scene->enemy->fxDeath_Enemy);
			break;

		case ColliderType::ENEMY_LIMIT:
			LOG("Collision ENEMY LIMIT");
			//aix� fa que l'enemic torni a la vida si li faig atac que l'envia a col�lisionar amb el l�mit
			//for (e; e != NULL; e = e->next)
			//{
			//	if (e->data->ID == physA->id)
			//	{
			//		//e->data->speed  =- e->data->speed;
			//		e->data->state = eState::IDLE;
			//		break;
			//	}
			//}
			break;

		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		}
	}
}