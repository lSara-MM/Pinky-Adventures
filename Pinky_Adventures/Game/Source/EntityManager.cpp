#include "EntityManager.h"
#include "Player.h"
#include "ItemCoin.h"
#include "ItemGem.h"
#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();
	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	switch (type)
	{

	case EntityType::PLAYER:
		entity = new Player();
		break;

	case EntityType::ENEMY:
		entity = new Enemy();
		break;

	case EntityType::COIN:
		entity = new Coin();
		break;
	case EntityType::GEM:
		entity = new Gem();
		break;

	default: break;
	}

	// Created entities are added to the list
	AddEntity(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Update();
	}

	return ret;
}

bool EntityManager::LoadState(pugi::xml_node& data)
{
	float x = data.child("player").attribute("x").as_int();
	float y = data.child("player").attribute("y").as_int();

	app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) }, 0);

	ListItem<Enemy*>* e;
	e = app->scene->listEnemies.start;
	
	for (pugi::xml_node itemNode = data.child("enemy"); itemNode; itemNode = itemNode.next_sibling("enemy"))
	{
		float x = itemNode.attribute("x_E").as_int();
		float y = itemNode.attribute("y_E").as_int();
		SString loadedState = itemNode.attribute("state_E").as_string();

		e->data->pbody->body->SetTransform({ PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) }, 0);

		if (loadedState == "idle") { e->data->state = eState::IDLE; }
		else if (loadedState == "chase") { e->data->state = eState::CHASE; }
		else if (loadedState == "dead") { e->data->state = eState::DEAD; }
		else if (loadedState == "return") { e->data->state = eState::RETURN; }

		if (e->data->type == eType::FLYING)	{ e->data->pbody->body->SetGravityScale(0); }
		e = e->next;
	}
	return true;
}


bool EntityManager::SaveState(pugi::xml_node& data)
{
	pugi::xml_node player = data.append_child("player");

	player.append_attribute("x") = app->scene->player->position.x;
	player.append_attribute("y") = app->scene->player->position.y;

	ListItem<Enemy*>* e;
	for (e = app->scene->listEnemies.start; e != NULL; e = e->next)
	{
		pugi::xml_node enemy = data.append_child("enemy");

		enemy.append_attribute("x_E") = e->data->position.x;
		enemy.append_attribute("y_E") = e->data->position.y;

		switch (e->data->state)
		{
		case eState::IDLE:
			enemy.append_attribute("state_E") = "idle";
			break;
		case eState::CHASE:
			enemy.append_attribute("state_E") = "chase";
			break;
		case eState::DEAD:
			enemy.append_attribute("state_E") = "dead";
			break;
		case eState::RETURN:
			enemy.append_attribute("state_E") = "return";
			break;
		default:
			enemy.append_attribute("state_E") = "idle";
			break;
		}
	}

	return true;
}