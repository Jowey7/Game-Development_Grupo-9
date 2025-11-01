#pragma once

#include "Entity.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Animation.h"

struct SDL_Texture;

// Enum para los estados del jugador
enum class PlayerState
{
	IDLE,
	WALK,
	SPRINT,
	JUMP
};

class Player : public Entity
{
public:
	Player();
	virtual ~Player();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB) override; // <-- "override" AÑADIDO
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override; // <-- AÑADIDO
	void Respawn();

private:
	void SetState(PlayerState state);

public:
	PlayerState currentState;

	// Texturas
	SDL_Texture* idleTexture = nullptr;
	SDL_Texture* walkTexture = nullptr;
	SDL_Texture* sprintTexture = nullptr;
	SDL_Texture* jumpTexture = nullptr;
	SDL_Texture* currentTexture = nullptr;
	SDL_Texture* HP = nullptr;
	SDL_Texture* jumpEffectTexture = nullptr;

	// Animaciones
	Animation idleAnim;
	Animation walkAnim;
	Animation sprintAnim;
	Animation jumpAnim;
	Animation jumpEffectAnim;
	Animation* currentAnimation = nullptr;
	bool showJumpEffect = false;
	Vector2D jumpEffectPosition;
	bool flip = false;

	// Físicas y estado
	PhysBody* pbody;
	bool isOverlappingOneWayPlatform = false; // <-- AÑADIDO
	PhysBody* currentOneWayPlatform = nullptr; // <-- AÑADIDO
	float walkSpeed = 3.5f;
	float sprintSpeed = 5.0f;
	float jumpForce = 2.1f;
	float airSpeedLimit = 0.0f;
	bool isJumping = false;
	int lives = 3;
	Vector2D initialPosition;

	// Fx de audio
	int pickCoinFxId;

private:
	float respawnCooldown = 0.0f;
};