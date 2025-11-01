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
#include "Physics.h"
#include "Item.h" // <-- Asegúrate de que este include existe

Scene::Scene() : Module()
{
	name = "scene";
}

Scene::~Scene() {}

bool Scene::Awake()
{
	LOG("Loading Scene");
	return true;
}

bool Scene::Start()
{
	// --- Creación de Entidades ---
	// Se crea el jugador y los ítems aquí para asegurar que aparezcan cada vez que la escena se inicie.
	player = std::dynamic_pointer_cast<Player>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));

	std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	item->position = Vector2D(200, 350);
	// ---------------------------

	Engine::GetInstance().map->active = true;
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");
	Engine::GetInstance().map->Load("Assets/Background/", "Swamp_Map.tmx");

	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer1.png"));
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer2.png"));
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer3.png"));
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer4.png"));
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer5.png"));

	// --- CARGAR TEXTURA DEBUG ---
	// NOTA: Asegúrate de que "Debug Info.png" esté en esta ruta
	debugInfoTexture = Engine::GetInstance().textures->Load("Assets/Background/Debug Info.png");
	if (debugInfoTexture == nullptr)
	{
		LOG("¡Error al cargar la textura Debug Info.png!");
	}
	showDebugInfo = false; // Asegurarse de que empieza oculto

	Map* map = Engine::GetInstance().map.get();
	Physics* physics = Engine::GetInstance().physics.get();

	int mapWidth = map->mapData.width * map->mapData.tileWidth;
	int mapHeight = map->mapData.height * map->mapData.tileHeight;

	PhysBody* leftWall = physics->CreateRectangle(-10, mapHeight / 2, 20, mapHeight, STATIC);
	leftWall->ctype = ColliderType::PLATFORM;

	PhysBody* rightWall = physics->CreateRectangle(mapWidth + 10, mapHeight / 2, 20, mapHeight, STATIC);
	rightWall->ctype = ColliderType::PLATFORM;

	return true;
}

bool Scene::PreUpdate()
{
	float parallaxSpeeds[] = { 0.1f, 0.25f, 0.4f, 0.6f, 0.8f };
	int yOffsets[] = { -200, -200, -200, 40, -15 };
	float scaleFactors[] = { 2.0f, 1.3f, 1.3f, 1.0f, 1.0f };

	for (size_t i = 0; i < backgroundTextures.size(); ++i)
	{
		SDL_Texture* texture = backgroundTextures[i];
		if (texture != nullptr)
		{
			Render* render = Engine::GetInstance().render.get();
			Map* map = Engine::GetInstance().map.get();
			Textures* textures = Engine::GetInstance().textures.get();

			int originalBgW, originalBgH;
			textures->GetSize(texture, originalBgW, originalBgH);

			int scaledBgW = (int)(originalBgW * scaleFactors[i]);
			int scaledBgH = (int)(originalBgH * scaleFactors[i]);

			int mapHeightPixels = map->mapData.height * map->mapData.tileHeight;
			int backgroundY = mapHeightPixels - scaledBgH;

			if (render->camera.y > 0) {
				backgroundY += (int)(render->camera.y * (1.0f - parallaxSpeeds[i]));
			}

			backgroundY += yOffsets[i];

			int mapWidthPixels = map->mapData.width * map->mapData.tileWidth;
			int numTiles = (scaledBgW > 0) ? (mapWidthPixels / scaledBgW) + 2 : 1;

			for (int j = 0; j < numTiles; ++j)
			{
				render->DrawTexture(texture, j * scaledBgW, backgroundY, NULL, parallaxSpeeds[i], 0, INT_MAX, INT_MAX, scaledBgW, scaledBgH);
			}
		}
	}
	return true;
}

bool Scene::Update(float dt) { return true; }

bool Scene::PostUpdate()
{
	bool ret = true;

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	// --- LÓGICA DE DEBUG TOGGLE ---
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		showDebugInfo = !showDebugInfo;
	}

	// F9: Mostrar/Ocultar Colliders
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		Engine::GetInstance().physics->ToggleDebug();
	}

	// F10: Activar/Desactivar Modo Dios
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (player != nullptr)
		{
			player->ToggleGodMode();
		}
	}

	// F11: Activar/Desactivar Límite de FPS (30 FPS)
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		// <-- MODIFICADO AQUÍ -->
		Engine::GetInstance().ToggleFrameRateCap();
	}
	// <-- FIN DE LA MODIFICACIÓN -->


	// --- RENDERIZADO DE DEBUG ---
	if (showDebugInfo && debugInfoTexture != nullptr)
	{
		int originalW, originalH;
		Engine::GetInstance().textures->GetSize(debugInfoTexture, originalW, originalH);

		int scaledW = (int)(originalW * 0.5f); // 50% del tamaño original
		int scaledH = (int)(originalH * 0.5f); // 50% del tamaño original

		// Dibujamos la textura en (200, 0) de la pantalla.
		// Usamos speed = 0.0f para que no se mueva con la cámara.
		Engine::GetInstance().render->DrawTexture(debugInfoTexture, 330, 0, NULL, 0.0f, 0, INT_MAX, INT_MAX, scaledW, scaledH);
	}


	if (player != nullptr)
	{
		Render* render = Engine::GetInstance().render.get();
		Window* window = Engine::GetInstance().window.get();
		Map* map = Engine::GetInstance().map.get();

		render->camera.x = -player->position.getX() * window->GetScale() + window->width / 2;
		if (render->camera.x > 0)
		{
			render->camera.x = 0;
		}
		int mapWidthInPixels = map->mapData.width * map->mapData.tileWidth * window->GetScale();
		if (render->camera.x < -(mapWidthInPixels - window->width))
		{
			render->camera.x = -(mapWidthInPixels - window->width);
		}

		int mapHeightInPixels = map->mapData.height * map->mapData.tileHeight * window->GetScale();

		if (mapHeightInPixels < window->height)
		{
			render->camera.y = -(mapHeightInPixels - window->height);
		}
		else
		{
			render->camera.y = -player->position.getY() * window->GetScale() + window->height / 2;
			if (render->camera.y > 0)
			{
				render->camera.y = 0;
			}
			if (render->camera.y < -(mapHeightInPixels - window->height))
			{
				render->camera.y = -(mapHeightInPixels - window->height);
			}
		}
	}

	return true;
}

bool Scene::CleanUp()
{
	LOG("Freeing scene");
	player.reset();
	Engine::GetInstance().entityManager->CleanUp();
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().map->active = false;

	for (auto& texture : backgroundTextures)
	{
		Engine::GetInstance().textures->UnLoad(texture);
	}
	backgroundTextures.clear();

	// --- LIMPIAR TEXTURA DEBUG ---
	if (debugInfoTexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(debugInfoTexture);
		debugInfoTexture = nullptr;
	}

	return true;
}