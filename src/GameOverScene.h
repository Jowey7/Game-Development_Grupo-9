#pragma once
#include "Module.h"
#include <SDL3/SDL_render.h>

class GameOverScene : public Module
{
public:
    GameOverScene();
    virtual ~GameOverScene();

    bool Awake() override;
    bool Start() override;
    bool Update(float dt) override;
    bool PostUpdate() override;
    bool CleanUp() override;

    SDL_Texture* gameOverText = nullptr;
};

