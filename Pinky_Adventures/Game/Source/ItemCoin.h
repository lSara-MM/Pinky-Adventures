#ifndef __ITEMCOIN_H__
#define __ITEMCOIN_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

#include "Animation.h"
#include "List.h"

struct SDL_Texture;

class Coin : public Entity
{
public:

	Coin();
	virtual ~Coin();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void SpawnCoins();
	void CreateCoins(int gid_, int x_, int y_);
public:

	bool isPicked;
	int ID;
private:

	SDL_Texture* texture;
	const char* texturePath;

	Animation* currentAnimCoin;
	Animation coinAnim;

	PhysBody* pbody;
	List<PhysBody*> listCoins;
};

#endif // __ITEMCOIN_H__