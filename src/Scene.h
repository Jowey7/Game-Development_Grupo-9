#pragma once

#include "Module.h"
#include "Player.h"
#include <vector>

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();
	virtual ~Scene();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

private:
	std::shared_ptr<Player> player;
	std::vector<SDL_Texture*> backgroundTextures;
};