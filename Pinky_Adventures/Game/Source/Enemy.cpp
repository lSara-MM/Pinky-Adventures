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

	grav = GRAVITY_Y;
	return true;
}

bool Enemy::Start() {
	
	texture = app->tex->Load(texturePath);
	//enemyPath = app->tex->Load();
	pbody = app->physics->CreateRectangle(position.x + width / 2, position.y + height / 2, width, height, bodyType::DYNAMIC);
	pbody->body->SetFixedRotation(true);

	pbody->ctype = ColliderType::ENEMY;

	fxJump = app->audio->LoadFx(jumpPath);
	fxLand = app->audio->LoadFx(landPath);

	idle = true;
	chase = false;
	
	return true;
}

bool Enemy::Update()
{
	currentAnimation = &idleAnim;
	b2Vec2 vel = b2Vec2(0, grav);

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//intent de pathfinding

	pos_Enemy = app->map->WorldToMap(METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - width / 2, METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - height / 2);
	pos_Player = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);

	State(pos_Player, pos_Enemy);

	if (chase) {

		app->pathfinding->CreatePath(pos_Enemy, pos_Player);
		const DynArray<iPoint>* path = app->pathfinding->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			TileSet * tileset = app->map->GetTilesetFromTileId(enGID);
			SDL_Rect r = tileset->GetTileRect(enGID);
			app->render->DrawTexture(tileset->texture, pos.x, pos.y, &r);

			if (pos.x < pos_Enemy.x) {
				flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
				vel = b2Vec2(-speed, GRAVITY_Y);

				currentAnimation = &forwardAnim;
			}

			if (pos.x > pos_Enemy.x) {

				flipType = SDL_RendererFlip::SDL_FLIP_NONE;
				vel = b2Vec2(speed, GRAVITY_Y);
				currentAnimation = &forwardAnim;

			}

			/*if (pos.y >= position_E.y) {

				currentAnimation = &jumpAnim;

				pbody->body->ApplyForceToCenter(b2Vec2{ 0,100 }, 1);

			}*/
		}

	}


	if (idle) {

		currentAnimation = &idleAnim;
		app->pathfinding->ClearLastPath();

	}

	//Update enemy position in pixels
	
	pbody->body->SetLinearVelocity(vel);

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - width / 2;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - height / 2;

	currentAnimation->Update();

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, flipType);
	
	return true;
}

bool Enemy::CleanUp()
{
	
	pbody->body->GetWorld()->DestroyBody(pbody->body);

	return true;
}

void Enemy::State(iPoint posPlayer, iPoint posEnemy)
{
	if (posPlayer.DistanceTo(posEnemy) <= 50) {

		idle = false;
		chase = true;
	}
	else {
		idle = true;
		chase = false;
	}

}
