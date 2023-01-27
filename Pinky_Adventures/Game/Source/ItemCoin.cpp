#include "ItemCoin.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

#include "Map.h"
#include "EntityManager.h"

Coin::Coin() : Entity(EntityType::COIN)
{
	name.Create("item");

	coinAnim.PushBack({ 0, 0, 16, 16 });
	coinAnim.PushBack({ 16, 0, 16, 16 });
	coinAnim.PushBack({ 32, 0, 16, 16 });
	coinAnim.PushBack({ 48, 0, 16, 16 });

	coinAnim.speed = 0.1f;
}

Coin::~Coin() {}

bool Coin::Awake() {
	return true;
}

bool Coin::Start() {
	return true;
}

bool Coin::Update(float dt)
{
	if (!isAlive)
	{
		active = false;
		pbody->body->SetActive(false);
		return true;
	}

	currentAnimCoin = &coinAnim;
	currentAnimCoin->Update();
	SDL_Rect rect = currentAnimCoin->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);
	return true;
}

bool Coin::CleanUp()
{
	app->tex->UnLoad(texture);
	pbody->body->GetWorld()->DestroyBody(pbody->body);

	return true;
}

void Coin::SpawnCoins() {

	ListItem<MapLayer*>* mapLayerItem;
	mapLayerItem = app->map->mapData.maplayers.start;

	/*ListItem<PhysBody*>* bodyItem;
	bodyItem = listCoins.start;*/

	while (mapLayerItem != NULL) {

		if (mapLayerItem->data->name == "coins") {

			for (int x = 0; x < mapLayerItem->data->width; x++)
			{
				for (int y = 0; y < mapLayerItem->data->height; y++)
				{
					int gid = mapLayerItem->data->Get(x, y);

					if (gid == 248)
					{
						CreateCoins(gid, x, y);
					}
				}
			}
		}
		mapLayerItem = mapLayerItem->next;
	}
}

void Coin::CreateCoins(int gid_, int x_, int y_) {

	Coin* item = (Coin*)app->entityManager->CreateEntity(EntityType::COIN);

	TileSet* tileset = app->map->GetTilesetFromTileId(gid_);
	SDL_Rect r = tileset->GetTileRect(gid_);
	iPoint pos = app->map->MapToWorld(x_, y_);

	//PhysBody* collider;
	item->ID = app->scene->coinIDset++;

	item->position = pos;

	item->pbody = app->physics->CreateCircleSensor(pos.x + r.w / 2, pos.y + r.h / 2, 8, bodyType::STATIC, item->ID);
	item->pbody->ctype = ColliderType::COIN;
	item->pbody->body->SetFixedRotation(true);
	item->active = true;
	item->isAlive = true;
	
	item->texture = tileset->texture;
	app->scene->listCoins.Add(item);
}

