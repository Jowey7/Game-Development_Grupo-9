#include "GameOverScene.h"
#include "Engine.h"
#include "Render.h"
#include "Window.h"
#include "Input.h"
#include "Scene.h"


GameOverScene::GameOverScene() : Module()
{
    name = "gameover";
}

GameOverScene::~GameOverScene() {}

bool GameOverScene::Awake() { return true; }
bool GameOverScene::Start() { return true; }

bool GameOverScene::Update(float dt)
{
    // Fondo negro
    Engine::GetInstance().render->SetBackgroundColor({ 0, 0, 0, 255 });

    // Si tienes función para dibujar texto, dibuja "Game Over" aquí.

    return true;
}

bool GameOverScene::PostUpdate()
{
    // Press ESC to close the game
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) return false;

    //Press R to restart the game
    if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
    {
        Engine::GetInstance().RequestSceneChange(std::make_shared<Scene>());
    }

    return true;
}

bool GameOverScene::CleanUp() { return true; }
