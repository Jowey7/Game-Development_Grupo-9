#include "GameOverScene.h"
#include "src/Engine.h"
#include "src/Render.h"
#include "src/Window.h"


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
    // Ejemplo:
    // Engine::GetInstance().render->DrawString("Game Over", 200, 200, {255,0,0,255});

    return true;
}

bool GameOverScene::PostUpdate()
{
    // Aquí puedes manejar input para reiniciar o salir si lo deseas
    return true;
}

bool GameOverScene::CleanUp() { return true; }
