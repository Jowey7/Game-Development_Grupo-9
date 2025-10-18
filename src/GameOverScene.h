#pragma once
#include "Module.h"

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
};

