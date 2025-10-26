#include "GameOverScene.h"
#include "Engine.h"
#include "Render.h"
#include "Window.h"
#include "Input.h"
#include "Scene.h"
#include "Textures.h"
#include "Log.h"

GameOverScene::GameOverScene() : Module()
{
    name = "gameover";
}

GameOverScene::~GameOverScene() {}

bool GameOverScene::Awake() { return true; }
bool GameOverScene::Start()
{ 
    gameOverText = Engine::GetInstance().textures->Load("Assets/Background/Game Over_P1.png");

    if (gameOverText == nullptr)
    {
        LOG("Failed to load Game Over texture");
        return false;
    }
    return true;

}


bool GameOverScene::Update(float dt)
{
    Engine::GetInstance().render->SetBackgroundColor({ 0, 0, 0, 255 });

    int screenW = Engine::GetInstance().window->width;
    int screenH = Engine::GetInstance().window->height;

    if (gameOverText) {
        int texW = 0, texH = 0;
        Engine::GetInstance().textures->GetSize(gameOverText, texW, texH);

        // Set desired size - try using a small scale factor instead
        float scaleFactor = 0.5f; // This will make it 30% of original size
        int desiredW = (int)(texW * scaleFactor);
        int desiredH = (int)(texH * scaleFactor);

        // Center the texture
        int x = (screenW - desiredW) / 2;
        int y = (screenH - desiredH) / 2;

        Engine::GetInstance().render->DrawTexture(
            gameOverText,
            x, y,                // x, y (centered)
            nullptr,             // srcRect (draw whole texture)
            0.0f,                // speed (set to 0 to ignore camera)
            0.0,                 // angle
            0, 0,                // pivotX, pivotY (use defaults)
            desiredW, desiredH,  // width, height (scaled down)
            SDL_FLIP_NONE        // no flip
        );
    }

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
