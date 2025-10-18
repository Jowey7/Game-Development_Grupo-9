#pragma once

#include "Entity.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();

	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// LÍNEA CORREGIDA
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void Respawn();

public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	SDL_Texture* HP = NULL;

	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	PhysBody* pbody;
	float jumpForce = 2.5f;
	bool isJumping = false;

	int lives = 3;
	Vector2D initialPosition;

private:
	float respawnCooldown = 0.0f; // Temporizador para evitar bucles de muerte
};