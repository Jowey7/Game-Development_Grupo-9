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
	item->position = Vector2D(200, 350);

	return true;
}

bool Scene::Start()
{
	// Activamos el módulo del mapa al iniciar la escena
	Engine::GetInstance().map->active = true;
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");
	Engine::GetInstance().map->Load("Assets/Background/", "Swamp_Map.tmx");

	// Cargamos todas las capas del fondo en orden
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer1.png"));
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer2.png"));
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer3.png"));
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer4.png"));
	backgroundTextures.push_back(Engine::GetInstance().textures->Load("Assets/Background/B_Layer5.png"));

	return true;
}

bool Scene::PreUpdate()
{
	// --- AJUSTES DE CAPAS ---
	// Velocidades de parallax
	float parallaxSpeeds[] = { 0.1f, 0.25f, 0.4f, 0.6f, 0.8f };
	// Desplazamientos verticales (positivo = más abajo)
	int yOffsets[] = { -200, -200, -200, 40, -15 }; // <--- ¡MODIFICA ESTOS VALORES!
	// Factores de escala (1.0 = tamaño original, 1.2 = 20% más grande, 0.8 = 20% más pequeño)
	float scaleFactors[] = { 2.0f, 1.0f, 1.0f, 1.0f, 1.0f }; // <--- ¡MODIFICA ESTOS VALORES!


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

			// Aplicamos el factor de escala
			int scaledBgW = (int)(originalBgW * scaleFactors[i]);
			int scaledBgH = (int)(originalBgH * scaleFactors[i]);

			int mapHeightPixels = map->mapData.height * map->mapData.tileHeight;
			int backgroundY = mapHeightPixels - scaledBgH; // Usamos la altura escalada para la posición

			if (render->camera.y > 0) {
				backgroundY += (int)(render->camera.y * (1.0f - parallaxSpeeds[i]));
			}

			backgroundY += yOffsets[i];

			int mapWidthPixels = map->mapData.width * map->mapData.tileWidth;
			// Aseguramos que el mosaico no tenga huecos si la imagen es muy pequeña
			int numTiles = (scaledBgW > 0) ? (mapWidthPixels / scaledBgW) + 2 : 1;

			for (int j = 0; j < numTiles; ++j)
			{
				// Pasamos el nuevo tamaño a la función de dibujado
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

	return ret;
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

	return true;
}