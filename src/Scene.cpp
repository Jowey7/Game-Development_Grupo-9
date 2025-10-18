#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"

Scene::Scene() : Module()
{
	name = "scene";
}

Scene::~Scene() {}

bool Scene::Awake()
{
	LOG("Loading Scene");
	player = std::dynamic_pointer_cast<Player>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));

	std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	item->position = Vector2D(200, 672);

	return true;
}

bool Scene::Start()
{
	// Activamos el módulo del mapa al iniciar la escena
	Engine::GetInstance().map->active = true;
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	return true;
}

bool Scene::PreUpdate() { return true; }
bool Scene::Update(float dt) { return true; }

bool Scene::PostUpdate()
{
	bool ret = true;

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if (player != nullptr)
	{
		Render* render = Engine::GetInstance().render.get();
		Window* window = Engine::GetInstance().window.get();

		render->camera.x = -player->position.getX() * window->GetScale() + window->width / 2;

		if (render->camera.x > 0)
		{
			render->camera.x = 0;
		}

		int mapWidthInPixels = Engine::GetInstance().map->mapData.width * Engine::GetInstance().map->mapData.tileWidth * window->GetScale();
		if (render->camera.x < -(mapWidthInPixels - window->width))
		{
			render->camera.x = -(mapWidthInPixels - window->width);
		}
	}

	return ret;
}

bool Scene::CleanUp()
{
	LOG("Freeing scene");
	player.reset();
	Engine::GetInstance().entityManager->CleanUp();
	Engine::GetInstance().map->CleanUp();
	// SOLUCIÓN GAME OVER: Desactivamos el mapa al limpiar la escena
	Engine::GetInstance().map->active = false;
	return true;
}