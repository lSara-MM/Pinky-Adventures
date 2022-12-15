#include "Player.h"
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


Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	
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
	deathAnim.PushBack({ 0, 0, 0, 0 });


	deathAnim.speed = 0.1f;
	deathAnim.loop = false;

	attackAnim.PushBack({ 8, 107, 17, 28 });
	attackAnim.PushBack({ 38, 108, 20, 27 });
	attackAnim.PushBack({ 73, 108, 18, 27 });
	attackAnim.PushBack({ 104, 107, 17, 28 });

	attackAnim.speed = 0.1f;
	attackAnim.loop = false;


	active = true;
}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();

	texturePath = parameters.attribute("texturepath").as_string();
	fxCoin = parameters.attribute("audiopathCoin").as_string();
	fxGem = parameters.attribute("audiopathGem").as_string();

	speed = parameters.attribute("velocity").as_int();
	width = parameters.attribute("width").as_int();
	height = parameters.attribute("height").as_int();
	jumpPath = parameters.attribute("audiopathJump").as_string();
	landPath = parameters.attribute("audiopathLand").as_string();
	deathPath = parameters.attribute("audiopathDeath").as_string();
	secretPath = parameters.attribute("audiopathSecret").as_string();

	jump = 2;
	grav = GRAVITY_Y;
	contador = 0; //temps salta player
	
	score = 0;
	return true;
}

bool Player::Start() {

	texture = app->tex->Load(texturePath);

	pickCoinFxId = app->audio->LoadFx(fxCoin);
	pickGemFxId = app->audio->LoadFx(fxGem);
	
	pbody = app->physics->CreateRectangle(position.x + width / 2, position.y + height / 2, width, height, bodyType::DYNAMIC);
	pbody->body->SetFixedRotation(true);
	
	pbody->listener = this; 

	pbody->ctype = ColliderType::PLAYER;
	
	fxJump = app->audio->LoadFx(jumpPath);
	fxLand = app->audio->LoadFx(landPath);
	fxDeath = app->audio->LoadFx(deathPath);
	fxSecret = app->audio->LoadFx(secretPath);

	ded = false;
	ani = true;
	attack = false;
	
	return true;
}

bool Player::Update()
{
	currentAnimation = &idleAnim;

	b2Vec2 vel = b2Vec2(0, grav); 

	if (app->input->godMode == false) {

		pbody->body->SetGravityScale(0.0);
	}

	if (app->input->godMode == true) {

		pbody->body->SetGravityScale(-60.0);
	}

	if (app->input->godMode == true && app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		vel = b2Vec2(0, -speed);
	}

	if (app->input->godMode == true && app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		vel = b2Vec2(0, 15);
	}

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN  && jump > 0 && ded == false) {
		
		currentAnimation = &jumpAnim;
		jump--;
		contador = 20;
		app->audio->PlayFx(fxJump);		
	}
	
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && ded == false) {
		flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
		vel = b2Vec2(-speed, grav);
		
		currentAnimation = &forwardAnim;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && ded == false) {
		flipType = SDL_RendererFlip::SDL_FLIP_NONE;
		vel = b2Vec2(speed, grav);
	
		currentAnimation = &forwardAnim;
	}

	
	else if (ded == true) {
		currentAnimation = &deathAnim;
	}

	if (contador != 0) {
		grav = -GRAVITY_Y;
		currentAnimation = &jumpAnim;
		contador--;
	}

	if (contador == 0) {
		grav = GRAVITY_Y;
	}


	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - width/2;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - height/2;

	currentAnimation->Update();

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, flipType);


	if (app->input->godMode || app->physics->collisions)
		app->render->DrawLine(position.x + pbody->width / 2, position.y + pbody->height / 2,
			position.x + METERS_TO_PIXELS(vel.x / (pbody->width / 2))+ pbody->width / 2,
			position.y + METERS_TO_PIXELS(vel.y / (pbody->height / 2))+ pbody->width / 2, 
			0, 0, 255);
	return true;
}

bool Player::CleanUp()
{
	app->tex->UnLoad(texture);
	pbody->body->GetWorld()->DestroyBody(pbody->body);

	return true;
}


void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

 	switch (physB->ctype)
	{
		case ColliderType::COIN:
			LOG("Collision COIN");
			{
				app->audio->PlayFx(pickCoinFxId);

				score += 10;
				ListItem<Coin*>* i = app->scene->listCoins.start;

				for (i; i != NULL; i = i->next)
				{
					if (i->data->ID == physB->id)
					{
 						i->data->isPicked = false;
						break;
					}
				}
			}
 			break;

		case ColliderType::GEM:
			LOG("Collision GEM");
			score += 100;
			app->audio->PlayFx(pickGemFxId);
			app->scene->gem->isPicked = false;
			app->scene->end = true;
			break;

		case ColliderType::PLATFORM:
			//LOG("Collision PLATFORM");
			if (jump == 0) {
				jump += 2;
			}
			app->audio->PlayFx(fxLand);
			break;

		case ColliderType::SPIKE:
			LOG("Collision SPIKE");
			if (app->input->godMode == false) {
				ded = true;
				app->audio->PlayFx(fxDeath);
			}
			break;

		case ColliderType::FALL:
			LOG("Collision FALL");
			if (app->input->godMode == false) {
				ded = true;
				app->audio->PlayFx(fxDeath);
			}
			break;

		case ColliderType::CHANGE:
			LOG("Collision CHANGE");
			if (app->scene->secret == false) {
				app->scene->secret = true;
				app->audio->PlayFx(fxSecret);
			}
			
			break;

		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
	}
	
}
