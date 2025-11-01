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
	bool Update(float dt); // <-- Esta ser� ahora el "gestor" de estados
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;
	void Respawn();

	void ToggleGodMode(); // <-- A�ADIR ESTA L�NEA

private:
	void SetState(PlayerState state);

	// --- Funciones de la M�quina de Estados ---
	void HandleIdle(float dt);
	void HandleWalk(float dt);
	void HandleSprint(float dt);
	void HandleJump(float dt);
	void DoJump(); // Funci�n de ayuda para el salto

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

	// F�sicas y estado
	PhysBody* pbody;
	bool isOverlappingOneWayPlatform = false;
	PhysBody* currentOneWayPlatform = nullptr;
	float walkSpeed = 3.5f;
	float sprintSpeed = 5.0f;
	float jumpForce = 2.1f;
	float airSpeedLimit = 0.0f;
	bool isJumping = false; // A�n la usamos para la l�gica de colisi�n
	bool isGodMode = false; // <-- A�ADIR ESTA L�NEA
	int lives = 3;
	Vector2D initialPosition;

	// --- Variables miembro para la l�gica de estado ---
	float desiredVelX = 0.0f; // <-- Se convierte en variable miembro

	// Fx de audio
	int pickCoinFxId;

private:
	float respawnCooldown = 0.0f;
};