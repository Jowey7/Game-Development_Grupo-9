#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"

EntityManager::EntityManager() : Module()
{
	name = "entitymanager";
}

// Destructor
EntityManager::~EntityManager()
{
}

// Called before render is available
bool EntityManager::Awake()
{
	LOG("Loading Entity Manager");
	bool ret = true;
	return ret;
}

bool EntityManager::Start() {

	bool ret = true;

	//Iterates over the entities and calls Start
	for (const auto& entity : entities)
	{
		if (entity->active == false) continue;
		if (!entity->Start())
		{
			ret = false;
			break;
		}
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	LOG("Cleaning up Entity Manager");
	for (const auto& entity : entities)
	{
		entity->CleanUp();
	}

	entities.clear();

	return true;
}

std::shared_ptr<Entity> EntityManager::CreateEntity(EntityType type)
{
	std::shared_ptr<Entity> entity;

	switch (type)
	{
	case EntityType::PLAYER:
		entity = std::make_shared<Player>();
		break;
	case EntityType::ITEM:
		entity = std::make_shared<Item>();
		break;
	default:
		break;
	}

	if (entity) {
		entities.push_back(entity);
		entity->Awake();
		entity->Start(); // <-- CORRECCIÓN APLICADA AQUÍ
	}

	return entity;
}

void EntityManager::DestroyEntity(std::shared_ptr<Entity> entity)
{
	if (entity != nullptr)
	{
		entity->CleanUp();
		entities.remove(entity);
	}
}

void EntityManager::AddEntity(std::shared_ptr<Entity> entity)
{
	if (entity != nullptr) entities.push_back(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	for (const auto& entity : entities)
	{
		if (entity->active == false) continue;
		if (!entity->Update(dt))
		{
			ret = false;
			break;
		}
	}
	return ret;
}