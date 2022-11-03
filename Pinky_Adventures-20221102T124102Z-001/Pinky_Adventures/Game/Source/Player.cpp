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

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	// idle animation (arcade sprite sheet)
	idleAnim.PushBack({ 8, 39, 17, 28 });
	idleAnim.PushBack({ 40, 39, 17, 28 });
	idleAnim.PushBack({ 71, 38, 18, 29 });
	idleAnim.PushBack({ 102, 38, 20, 29 });
	idleAnim.speed = 0.15f;

	// walk forward animation (arcade sprite sheet)
	forwardAnim.PushBack({ 8, 142, 17, 27 });
	forwardAnim.PushBack({ 40, 141, 17, 28 });
	forwardAnim.PushBack({ 72, 141, 17, 28 });
	forwardAnim.PushBack({ 103, 142, 18, 27 });
	forwardAnim.PushBack({ 136, 141, 17, 28 });
	forwardAnim.PushBack({ 168, 141, 17, 28 });
	forwardAnim.speed = 0.1f;

	// TODO 4: Make ryu walk backwards with the correct animations
	jumpAnim.PushBack({ 8, 73, 17, 28 });
	jumpAnim.PushBack({ 38, 75, 20, 26 });
	jumpAnim.PushBack({ 104, 71, 17, 30 });
	jumpAnim.PushBack({ 135, 69, 18, 29 });
	jumpAnim.PushBack({ 166, 71, 20, 29 });
	jumpAnim.PushBack({ 197, 74, 22, 27 });
	jumpAnim.PushBack({ 231, 75, 18, 26 });

	jumpAnim.speed = 0.1f;
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	fxPath = parameters.attribute("audiopath").as_string();
	width = 20;
	height = 30;
	jump = 1;
	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateRectangle(position.x + width/2, position.y +height/2, width, height, bodyType::DYNAMIC);
	pbody->body->SetFixedRotation(true);
	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this; 

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	pickCoinFxId = app->audio->LoadFx(fxPath);

	return true;
}

bool Player::Update()
{
	currentAnimation = &idleAnim;
	// L07 DONE 5: Add physics to the player - updated player position using physics

	int speed = 5; 
	b2Vec2 vel = b2Vec2(0, GRAVITY_Y); 

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture

	
	if ((app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) && jump == 1) {
		
		currentAnimation = &jumpAnim;
		jump = 0;
		pbody->body->ApplyForce(b2Vec2(0, -800), pbody->body->GetWorldCenter(), true);
	}
	
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		flipType = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
		vel = b2Vec2(-speed, GRAVITY_Y);
		currentAnimation = &forwardAnim;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		flipType = SDL_RendererFlip::SDL_FLIP_NONE;
		vel = b2Vec2(speed, GRAVITY_Y);
		currentAnimation = &forwardAnim;
	}

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - width/2;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - height/2;

	currentAnimation->Update();

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, flipType);


	return true;
}

bool Player::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	switch (physB->ctype)
	{
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			app->audio->PlayFx(pickCoinFxId);
			break;
		case ColliderType::PLATFORM:
			LOG("Collision PLATFORM");
			if (jump == 0) {
				jump += 1;
			}
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
	}
	
}
