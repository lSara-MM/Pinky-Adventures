#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "List.h"

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
	Animation attackAnim;

	int jump, speed, width, height;
	bool ded;
	bool ani;
	bool attackState;

	float grav;
	SDL_RendererFlip flipType;
	int contador;
	PhysBody* pbody;
	uint fxDeath;
	int score;

	float dtP;


	int attackCooldown;
	int contadorCooldown;
	


private:
	SDL_Texture* texture;
	const char* texturePath;
	const char* jumpPath;
	const char* landPath;
	const char* deathPath;
	const char* secretPath;

	int playerForce;//quan lluny envia enemic al donar-li

	uint fxJump;
	uint fxLand;
	
	uint fxSecret;

	uint pickHealthFxId;
	uint pickCoinFxId;
	uint pickGemFxId;
	uint goPortal;
	const char* fxCoin;	
	const char* fxGem;
	const char* fxHealth;
	const char* fxPortal;


	uint fxAttack;
	const char* attackPath;

	const char* audiopathCd;
	uint attackCd;

	bool tp;


};

#endif // __PLAYER_H__