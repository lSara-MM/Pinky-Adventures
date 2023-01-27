#include "ItemHealth.h"
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

Health::Health() : Entity(EntityType::COIN)
{
	name.Create("health");

	healthAnim.PushBack({ 1, 1, 16, 16 });
	healthAnim.PushBack({ 19, 1, 16, 16 });
	healthAnim.PushBack({ 37, 1, 16, 16 });
	healthAnim.PushBack({ 55, 1, 16, 16 });
	healthAnim.PushBack({ 73, 1, 16, 16 });
	healthAnim.PushBack({ 91, 1, 16, 16 });
	healthAnim.PushBack({ 109, 1, 16, 16 });
	healthAnim.PushBack({ 127, 1, 16, 16 });

	healthAnim.speed = 0.1f;
}

Health::~Health() {}

bool Health::Awake() {
	return true;
}

bool Health::Start() {
	return true;
}

bool Health::Update(float dt)
{
	if (!isAlive)
	{
		active = false;
		pbody->body->SetActive(false);
		return true;
	}

	currentAnimHealth = &healthAnim;
	if (!app->scene->pause)
	{
		currentAnimHealth->Update();
	}
	SDL_Rect rect = currentAnimHealth->GetCurrentFrame();
	app->render->DrawTexture(texture, position.x, position.y, &rect);
	return true;
}

bool Health::CleanUp()
{
	app->tex->UnLoad(texture);
	pbody->body->GetWorld()->DestroyBody(pbody->body);

	return true;
}

void Health::SpawnHealth() {

	ListItem<MapLayer*>* mapLayerItem;
	mapLayerItem = app->map->mapData.maplayers.start;

	/*ListItem<PhysBody*>* bodyItem;
	bodyItem = listCoins.start;*/

	while (mapLayerItem != NULL) {

		if (mapLayerItem->data->name == "health") {

			for (int x = 0; x < mapLayerItem->data->width; x++)
			{
				for (int y = 0; y < mapLayerItem->data->height; y++)
				{
					int gid = mapLayerItem->data->Get(x, y);

					if (gid == 252)
					{
						CreateHealth(gid, x, y);
					}
				}
			}
		}
		mapLayerItem = mapLayerItem->next;
	}
}

void Health::CreateHealth(int gid_, int x_, int y_) {

	Health* item = (Health*)app->entityManager->CreateEntity(EntityType::HEALTH);

	TileSet* tileset = app->map->GetTilesetFromTileId(gid_);
	SDL_Rect r = tileset->GetTileRect(gid_);
	iPoint pos = app->map->MapToWorld(x_, y_);

	//PhysBody* collider;
	item->ID = app->scene->coinIDset++;

	item->position = pos;

	item->pbody = app->physics->CreateCircleSensor(pos.x + r.w / 2, pos.y + r.h / 2, 8, bodyType::STATIC, item->ID);
	item->pbody->ctype = ColliderType::HEALTH;
	item->pbody->body->SetFixedRotation(true);
	item->active = true;
	item->isAlive = true;
	
	item->texture = tileset->texture;
	app->scene->listHealth.Add(item);
}

