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

// Destructor
Scene::~Scene()
{
}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	player = std::dynamic_pointer_cast<Player>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));

	std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	item->position = Vector2D(200, 672);

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	// SOLUCIÓN: Lógica para que la cámara siga al jugador
	if (player != nullptr)
	{
		Render* render = Engine::GetInstance().render.get();
		Window* window = Engine::GetInstance().window.get();

		// Centramos la cámara en la posición X del jugador
		render->camera.x = -player->position.getX() * window->GetScale() + window->width / 2;

		// Limitamos el movimiento de la cámara para que no se salga del mapa
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

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	Engine::GetInstance().entityManager->CleanUp();
	return true;
}