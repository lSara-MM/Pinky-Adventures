#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

#include "Animation.h"
#include "ItemCoin.h"
#include "ItemGem.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
	void OnCollision(PhysBody* physA, PhysBody* physB);


public:
	// The pointer to the current player animation
	// It will be switched depending on the player's movement direction
	Animation* currentAnimation = nullptr;

	// A set of animations
	Animation idleAnim;
	Animation forwardAnim;
	Animation jumpAnim;
	Animation hurtAnim;
	Animation deathAnim;

	int jump, speed, width, height;
	bool ded;
	bool ani;
	float grav;
	SDL_RendererFlip flipType;
	int contador;
	//bool colliding;
	PhysBody* pbody;
	
	int score;

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;
	const char* coinPath;
	const char* jumpPath;
	const char* landPath;
	const char* deathPath;
	const char* secretPath;

	
	uint fxJump;
	uint fxLand;
	uint fxDeath;
	uint fxSecret;

	// L07 DONE 5: Add physics to the player - declare a Physics body
	//PhysBody* pbody;

	PhysBody* camerabody;

	uint pickCoinFxId;
	uint pickGemFxId;
	const char* fxCoin;	
	const char* fxGem;
};

#endif // __PLAYER_H__