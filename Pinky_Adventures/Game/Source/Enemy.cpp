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

	state = eState::IDLE;//Com s'hauria de posar això al xml?
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

	originPos = position;
	grav = GRAVITY_Y;
	return true;
}

bool Enemy::Start() {
	
	textureEnemy = app->tex->Load(texturePathEnemy);
	ID = app->scene->enemyIDset++;

	pbody = app->physics->CreateRectangle(position.x + width / 2, position.y + height / 2, width, height, bodyType::DYNAMIC, ID);
	
	if (type == eType::FLYING) { pbody->body->SetGravityScale(0); }

	pbody->body->SetFixedRotation(true);
	pbody->ctype = ColliderType::ENEMY;

	pbody->listener = this;

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

	pos_Enemy = app->map->WorldToMap(position.x, position.y);
	pos_Player = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);
	pos_Origin = app->map->WorldToMap(originPos.x, originPos.y);

	LOG("distance %d", pos_Player.DistanceTo(pos_Enemy));

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

			LOG("position %d origin %d", position.x, pos_Origin.x);
			vel = b2Vec2(0, grav);

			/*if (position.x + dist > pos_Origin.x) {   d = true; }
			else if (position.x + dist < pos_Origin.x && d == true) { vel = b2Vec2(speed, grav); }*/

			//(app->pathfinding->IsWalkable(iPoint(pos_Enemy.x + 1, pos_Enemy.y))) ? speed : speed = -speed;
			
			/*if (position.x >= pos_Origin.x || position.x + 50 < pos_Origin.x)
			{
				flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
				vel = b2Vec2(-speed, grav);
			}

			if (position.x < pos_Origin.x)
			{
				flipType = SDL_RendererFlip::SDL_FLIP_NONE;
				vel = b2Vec2(speed, grav);
			}*/

			break;

		case eType::FLYING:
			currentAnimation = &idleFlyingEnemyAnim;
			vel = b2Vec2(0, 0);
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

		State(pos_Player, pos_Enemy, vel);
		break;

	case eState::DEAD:
		//active = false;
		flipType = SDL_RendererFlip::SDL_FLIP_VERTICAL;//no queda bé amb la snake

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
		//(pos_Enemy != pos_Origin) ? State(pos_Origin, pos_Enemy, vel) : state = eState ::IDLE;	// perque no vol tornar al seu punt d'origen :/
		break;

	default:
		break;
	}


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

			else if (posPath_0.y < posPath.y || posPath_0.y > posPath.y)
			{
				pbody->body->ApplyForceToCenter(b2Vec2(0, -100), 0);
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
	if (path != nullptr)
	{
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			TileSet* tileset = app->map->GetTilesetFromTileId(enGID);
			SDL_Rect r = tileset->GetTileRect(enGID);
			app->render->DrawTexture(tileset->texture, pos.x, pos.y, &r);
		}
		
		SDL_Rect rect = { pos_Enemy.x - detectionDistance / 2, pos_Enemy.y - detectionDistance / 2, METERS_TO_PIXELS(detectionDistance),  METERS_TO_PIXELS(detectionDistance) };
		app->render->DrawRectangle(rect, 255, 0, 100, 255, false);
	}
	
}


void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	ListItem<Enemy*>* e;

	switch (physB->ctype)
	{

		case ColliderType::PLATFORM:
			LOG("Collision PLATFORM");
		
			break;
	
		case ColliderType::SPIKE:
			LOG("Collision ENEMY SPIKE");
		
			pbody->body->ApplyForceToCenter(b2Vec2(0, -50), 0);
			
			break;

		case ColliderType::FALL:
			LOG("Collision ENEMY SPIKE");

			e = app->scene->listEnemies.start;

			for (e; e != NULL; e = e->next)
			{
				if (e->data->ID == physA->id)
				{
					e->data->state = eState::DEAD;
					break;
				}
			}

			app->audio->PlayFx(app->scene->enemy->fxDeath_Enemy);

			e->data->pbody->body->SetGravityScale(15);
			
			break;

	case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
	}
}