#include "GameOverScene.h"
#include "Engine.h"
#include "Render.h"
#include "Window.h"

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

    // Dibuja el texto "Game Over" si tienes función de texto
    // Engine::GetInstance().render->DrawString("Game Over", centerX, centerY, {255,0,0,255});

    return true;
}

bool GameOverScene::PostUpdate()
{
    // Puedes manejar input para reiniciar o salir aquí
    return true;
}

bool GameOverScene::CleanUp() { return true; }
