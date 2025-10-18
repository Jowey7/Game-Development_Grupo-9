#include "Engine.h"
#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "Physics.h"
#include "Log.h"
#include "GameOverScene.h"
#include <sstream>
#include <iomanip>

Engine::Engine() {
	// ... (el constructor se mantiene igual)
	LOG("Constructor Engine::Engine");
	frames = 0;
	startupTime.Start();
	lastSecFrameTime.Start();
	frameTime.Start();

	window = std::make_shared<Window>();
	input = std::make_shared<Input>();
	render = std::make_shared<Render>();
	textures = std::make_shared<Textures>();
	audio = std::make_shared<Audio>();
	physics = std::make_shared<Physics>();
	scene = std::make_shared<Scene>();
	entityManager = std::make_shared<EntityManager>();
	map = std::make_shared<Map>();

	AddModule(window);
	AddModule(input);
	AddModule(textures);
	AddModule(audio);
	AddModule(physics);
	AddModule(map);
	AddModule(entityManager);
	AddModule(scene);
	AddModule(render);

	currentScene = scene;
}

Engine& Engine::GetInstance() {
	static Engine instance;
	return instance;
}

void Engine::AddModule(std::shared_ptr<Module> module) {
	module->Init();
	moduleList.push_back(module);
}

bool Engine::Awake() {
	// ... (la función Awake se mantiene igual)
	LOG("Engine::Awake");
	LoadConfig();
	gameTitle = configFile.child("config").child("engine").child("title").child_value();
	targetFrameRate = configFile.child("config").child("engine").child("targetFrameRate").attribute("value").as_int();

	for (const auto& module : moduleList) {
		module->LoadParameters(configFile.child("config").child(module->name.c_str()));
		if (!module->Awake()) return false;
	}
	return true;
}

bool Engine::Start() {
	// ... (la función Start se mantiene igual)
	LOG("Engine::Start");
	for (const auto& module : moduleList) {
		if (module->active && !module->Start()) return false;
	}
	return true;
}

bool Engine::Update() {
	bool ret = true;
	PrepareUpdate();
	if (input->GetWindowEvent(WE_QUIT)) ret = false;
	if (ret) ret = PreUpdate();
	if (ret) ret = DoUpdate();
	if (ret) ret = PostUpdate();
	FinishUpdate();

	// Realiza el cambio de escena de forma segura al final del fotograma
	if (sceneToLoad != nullptr) {
		if (currentScene) {
			currentScene->CleanUp();
			currentScene->active = false;
		}

		currentScene = sceneToLoad;
		sceneToLoad = nullptr; // Limpiamos la solicitud

		// Inicializamos la nueva escena
		bool found = false;
		for (auto& module : moduleList) {
			if (module == currentScene) {
				found = true;
				module->Init(); // Reactivamos si ya existía
				break;
			}
		}

		if (!found) {
			auto it = moduleList.end();
			--it; // Apunta a render
			moduleList.insert(it, currentScene);
			currentScene->Init();
		}

		currentScene->Start();
	}

	return ret;
}

// ... (CleanUp y otras funciones se mantienen igual)
// ... (Omito el resto por brevedad, no tienen cambios)

bool Engine::CleanUp() {
	LOG("Engine::CleanUp");
	for (auto it = moduleList.rbegin(); it != moduleList.rend(); ++it) {
		if (!(*it)->CleanUp()) return false;
	}
	return true;
}

void Engine::PrepareUpdate() {
	frameTime.Start();
}

void Engine::FinishUpdate() {
	double currentDt = frameTime.ReadMs();
	float maxFrameDuration = 1000.0f / targetFrameRate;
	if (targetFrameRate > 0 && currentDt < maxFrameDuration) {
		SDL_Delay((Uint32)(maxFrameDuration - currentDt));
	}

	frameCount++;
	secondsSinceStartup = startupTime.ReadSec();
	dt = (float)frameTime.ReadMs();
	lastSecFrameCount++;

	if (lastSecFrameTime.ReadMs() > 1000) {
		lastSecFrameTime.Start();
		framesPerSecond = lastSecFrameCount;
		lastSecFrameCount = 0;
		averageFps = (float)frameCount / secondsSinceStartup;
	}

	std::stringstream ss;
	ss << gameTitle << " | Av.FPS: " << std::fixed << std::setprecision(2) << averageFps
		<< " | Last sec frames: " << framesPerSecond
		<< " | Last dt: " << dt;
	window->SetTitle(ss.str().c_str());
}

bool Engine::PreUpdate() {
	render->Clear(); // <--- SE LLAMA A CLEAR() AQUÍ
	for (const auto& module : moduleList) {
		if (module->active && !module->PreUpdate()) return false;
	}
	return true;
}

bool Engine::DoUpdate() {
	for (const auto& module : moduleList) {
		if (module->active && !module->Update(dt)) return false;
	}
	return true;
}

bool Engine::PostUpdate() {
	for (const auto& module : moduleList) {
		if (module->active && !module->PostUpdate()) return false;
	}
	return true;
}

bool Engine::LoadConfig() {
	pugi::xml_parse_result result = configFile.load_file("config.xml");
	if (!result) {
		LOG("Error loading config.xml: %s", result.description());
		return false;
	}
	return true;
}

// Implementación de la nueva función para solicitar el cambio de escena
void Engine::RequestSceneChange(std::shared_ptr<Module> newScene) {
	sceneToLoad = newScene;
}