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
	RUN,
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

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void Respawn();

private:
	void SetState(PlayerState state);

public:
	PlayerState currentState;

	// Texturas del jugador
	SDL_Texture* idleTexture = nullptr;
	SDL_Texture* runTexture = nullptr;
	SDL_Texture* jumpTexture = nullptr;
	SDL_Texture* currentTexture = nullptr;
	SDL_Texture* HP = nullptr;

	// --- Variables para el efecto de salto ---
	SDL_Texture* jumpEffectTexture = nullptr;
	Animation jumpEffectAnim;
	bool showJumpEffect = false;
	Vector2D jumpEffectPosition;

	// Animaciones del jugador
	Animation idleAnim;
	Animation runAnim;
	Animation jumpAnim;
	Animation* currentAnimation = nullptr;
	bool flip = false;

	// Físicas y estado
	PhysBody* pbody;
	float jumpForce = 2.1f;
	bool isJumping = false;
	int lives = 3;
	Vector2D initialPosition;

	// Fx de audio
	int pickCoinFxId;

private:
	float respawnCooldown = 0.0f;
};