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
	attackAnim.PushBack({ 39, 109, 17, 26 });
	attackAnim.PushBack({ 71, 108, 26, 27 });
	attackAnim.PushBack({ 104, 109, 18, 26 });
	attackAnim.PushBack({ 137, 108, 22, 27 });
	attackAnim.PushBack({ 167, 109, 17, 26 });

	attackAnim.speed = 0.1f;
	attackAnim.loop = false;

	active = true;
}

Player::~Player() {

}

bool Player::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();

	score = parameters.attribute("score").as_int();
	coins = parameters.attribute("coins").as_int();
	lives = parameters.attribute("lives").as_int();


	texturePath = parameters.attribute("texturepath").as_string();
	fxCoin = parameters.attribute("audiopathCoin").as_string();
	fxGem = parameters.attribute("audiopathGem").as_string(); 
	fxHealth = parameters.attribute("audiopathHealth").as_string();
	fxPortal = parameters.attribute("audiopathPortal").as_string();

	speed = parameters.attribute("velocity").as_int();
	width = parameters.attribute("width").as_int();
	height = parameters.attribute("height").as_int();
	jumpPath = parameters.attribute("audiopathJump").as_string();
	landPath = parameters.attribute("audiopathLand").as_string();
	deathPath = parameters.attribute("audiopathDeath").as_string();
	secretPath = parameters.attribute("audiopathSecret").as_string();

	attackPath = parameters.attribute("audiopathAttack").as_string();

	attackCooldown = parameters.attribute("attackCooldown").as_int();

	playerForce = parameters.attribute("playerForce").as_int();

	audiopathCd = parameters.attribute("audiopathCd").as_string();

	grav = parameters.attribute("FUYMGravity").as_int();;

	jump = 2;
	

	contador = 0; //temps salta player

	pickCoinFxId = app->audio->LoadFx(fxCoin);
	pickGemFxId = app->audio->LoadFx(fxGem);
	pickHealthFxId = app->audio->LoadFx(fxHealth);
	goPortal = app->audio->LoadFx(fxPortal);

	fxJump = app->audio->LoadFx(jumpPath);
	fxLand = app->audio->LoadFx(landPath);
	fxDeath = app->audio->LoadFx(deathPath);
	fxSecret = app->audio->LoadFx(secretPath);

	fxAttack = app->audio->LoadFx(attackPath);

	attackCd = app->audio->LoadFx(audiopathCd);
	return true;
}

bool Player::Start() {

	texture = app->tex->Load(texturePath);
	
	pbody = app->physics->CreateRectangle(position.x + width / 2, position.y + height / 2, width, height, bodyType::DYNAMIC);
	pbody->body->SetFixedRotation(true);
	
	pbody->listener = this; 

	pbody->ctype = ColliderType::PLAYER;

	contadorCooldown = attackCooldown;

	ded = false;
	ani = true;
	attackState = false;
	tp = false;
	
	return true;
}

bool Player::Update(float dt)
{
	if (app->scene->pause)
	{
		dtP = 0;
	}
	else if(!app->scene->pause)
	{
		dtP = dt / 1000;
	}
	

	currentAnimation = &idleAnim;

	b2Vec2 vel = b2Vec2(0, grav); 

	if (app->input->godMode == false) {

		pbody->body->SetGravityScale(0.0);
	}

	if (app->input->godMode == true) {

		pbody->body->SetGravityScale(-750* dtP);
	}

	if (app->input->godMode == true && app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && !app->scene->pause)
	{
		vel = b2Vec2(0, -125);
		app->scene->freeCam = false;
	}

	if (app->input->godMode == true && app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && !app->scene->pause)
	{
		vel = b2Vec2(0, 500);
		app->scene->freeCam = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN  && jump > 0 && ded == false && !app->input->godMode 
		&& !app->scene->pause)
	{
		
		pbody->body->ApplyForceToCenter(b2Vec2(50, -playerForce), 0);
		currentAnimation = &jumpAnim;
		jump--;
		contador = 20;
		app->audio->PlayFx(fxJump);		
		app->scene->freeCam = false;
	}
	
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && ded == false && !app->scene->pause)
	{
		if (!attackState) {
			flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
		}
		
		vel = b2Vec2(-speed, grav);
		
		currentAnimation = &forwardAnim;
		app->scene->freeCam = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && ded == false && !app->scene->pause)
	{
		if(!attackState){
			flipType = SDL_RendererFlip::SDL_FLIP_NONE;
		}

		vel = b2Vec2(speed, grav);
	
		currentAnimation = &forwardAnim;
		app->scene->freeCam = false;
	}
	
	if (contador != 0) {
		currentAnimation = &jumpAnim;
		contador--;
	}

	if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN && ded == false && attackState == false && contadorCooldown == attackCooldown
		&& !app->scene->pause)
	{
		attackAnim.loop = true;

		b2PolygonShape box;

		if (flipType == SDL_RendererFlip::SDL_FLIP_NONE) {

			box.SetAsBox(PIXEL_TO_METERS(15) * 0.5f, PIXEL_TO_METERS(15) * 0.5f, b2Vec2(PIXEL_TO_METERS(25) * 0.5f, 0), 0);
		}

		else if (flipType == SDL_RendererFlip::SDL_FLIP_HORIZONTAL) {

			box.SetAsBox(PIXEL_TO_METERS(15) * 0.5f, PIXEL_TO_METERS(15) * 0.5f, b2Vec2(-PIXEL_TO_METERS(25) * 0.5f, 0), 0);
		}

		b2FixtureDef fixtureAttack;
		fixtureAttack.shape = &box;
		fixtureAttack.density = 1.0f;
		fixtureAttack.isSensor = true;

		pbody->body->CreateFixture(&fixtureAttack);

		attackState = true;
		contadorCooldown = 0;
	}

	if (attackState == true && ded == false){

		currentAnimation = &attackAnim;

		if (currentAnimation->current_frame > 5.8f) {
			for (b2Fixture* f = pbody->body->GetFixtureList(); f; f = f->GetNext()) {

				if (f->IsSensor()) {
					pbody->body->DestroyFixture(f);
				}
			}
			currentAnimation->current_frame = 0;
			attackState = false;
			currentAnimation = &idleAnim;
		}
	}

	if (contadorCooldown != attackCooldown) {//check if attack is on cooldown

		contadorCooldown++;
	}

	if (contadorCooldown == attackCooldown-1) {
		app->audio->PlayFx(attackCd);
	}

	if (ded == true) {
		currentAnimation = &deathAnim;
	}

	vel = b2Vec2(vel.x*dtP, vel.y*dtP);
	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - width/2;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - height/2;

	if (tp)
	{
		pbody->body->SetTransform({ PIXEL_TO_METERS(120),PIXEL_TO_METERS(260) }, 0);
		tp = false;
	}
	
	if (!app->scene->pause)
	{
		currentAnimation->Update();
	}
	

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

	ListItem<Coin*>* i;
	ListItem<Enemy*>* e;
	ListItem<Health*>* h;

	switch (physB->ctype)
	{
		case ColliderType::COIN:
			LOG("Collision COIN");

			app->audio->PlayFx(pickCoinFxId);

			score += 10;
			i = app->scene->listCoins.start;

			for (i; i != NULL; i = i->next)
			{
				if (i->data->ID == physB->id)
				{
					i->data->isAlive = false;
					coins++;
					//app->scene->listCoins.Del(i);
					break;
				}
			}
			break;

		case ColliderType::ENEMY_WP:
			LOG("Collision ENEMY WEAKPOINT");
			score += 100;
			e = app->scene->listEnemies.start;

			for (e; e != NULL; e = e->next)
			{
				if (e->data->ID == physB->id)
				{
					e->data->state = eState::DEAD;
					e->data->pbody->body->SetGravityScale(15);
					//app->scene->listEnemies.Del(e);
					break;
				}
			}
			app->audio->PlayFx(app->scene->enemy->fxDeath_Enemy);
			break;

		case ColliderType::ENEMY:
			LOG("Collision ENEMY");

			if (attackState == true || app->input->godMode) {

				e = app->scene->listEnemies.start;

				for (e; e != NULL; e = e->next)
				{
					if (e->data->ID == physB->id)
					{
						e->data->state = eState::DEAD;

						if (flipType == SDL_RendererFlip::SDL_FLIP_HORIZONTAL) {

							e->data->pbody->body->ApplyForceToCenter(b2Vec2(-playerForce, 0), 0);
						}

						if (flipType == SDL_RendererFlip::SDL_FLIP_NONE) {

							e->data->pbody->body->ApplyForceToCenter(b2Vec2(playerForce, 0), 0);
						}

						e->data->pbody->body->SetGravityScale(750*dtP);
						//app->scene->listEnemies.Del(e);
						break;
					}
				}

				app->audio->PlayFx(fxAttack);

				app->audio->PlayFx(app->scene->enemy->fxDeath_Enemy);
			}

			if (app->input->godMode == false && attackState == false) {
				
				e = app->scene->listEnemies.start;

				for (e; e != NULL; e = e->next)
				{
					if (e->data->ID == physB->id)
					{
						if (e->data->state != eState::DEAD)
						{
							ded = true;
							app->audio->PlayFx(fxDeath);
						}
						break;
					}
				}
			}
			break;

		case ColliderType::GEM:
			LOG("Collision GEM");
			score += 300;
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

		case ColliderType::PORTAL:

			LOG("Collision PORTAL");
			app->audio->PlayFx(goPortal);
			tp = true;
			break;

		case ColliderType::SAVE:
			score += 150;
			LOG("Collision SAVE");
			app->audio->PlayFx(app->scene->save->fxSave);
			app->scene->save->isPicked = true;

			break;

		case ColliderType::HEALTH:
			LOG("Collision HEALTH");
			score += 50;
			app->audio->PlayFx(pickHealthFxId);

			lives++;

			h = app->scene->listHealth.start;

			for (h; h != NULL; h = h->next)
			{
				if (h->data->ID == physB->id)
				{
					h->data->isAlive = false;
			
					break;
				}
			}
			break;


		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
	}
}
