#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Window.h"
#include "GameOverScene.h"
#include "Map.h"

const int PLAYER_RENDER_WIDTH = 32;
const int PLAYER_RENDER_HEIGHT = 32;


Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {}

bool Player::Awake() {
	position = Vector2D(60, 280);
	initialPosition = position;
	return true;
}

bool Player::Start() {
	// Carga de texturas
	idleTexture = Engine::GetInstance().textures->Load("Assets/Player1/p1_idle.png");
	walkTexture = Engine::GetInstance().textures->Load("Assets/Player1/p1_walk.png");
	sprintTexture = Engine::GetInstance().textures->Load("Assets/Player1/p1_run.png");
	jumpTexture = Engine::GetInstance().textures->Load("Assets/Player1/p1_jump.png");
	jumpEffectTexture = Engine::GetInstance().textures->Load("Assets/Player1/p1_jump_ef.png");
	HP = Engine::GetInstance().textures->Load("Assets/Objects/HP.png");

	// --- Configuración de Animaciones ---
	// IDLE (4 frames de 32x32)
	for (int i = 0; i < 4; ++i) idleAnim.frames.push_back({ i * 32, 0, 32, 32 });
	idleAnim.speed = 0.1f;
	idleAnim.loop = true;

	// WALK (6 frames de 32x32)
	for (int i = 0; i < 6; ++i) walkAnim.frames.push_back({ i * 32, 0, 32, 32 });
	walkAnim.speed = 0.2f;
	walkAnim.loop = true;

	// SPRINT (6 frames de 32x32)
	for (int i = 0; i < 6; ++i) sprintAnim.frames.push_back({ i * 32, 0, 32, 32 });
	sprintAnim.speed = 0.3f; // Un poco más rápida que caminar
	sprintAnim.loop = true;

	// JUMP (8 frames de 32x32)
	for (int i = 0; i < 8; ++i) jumpAnim.frames.push_back({ i * 32, 0, 32, 32 });
	jumpAnim.speed = 0.2f;
	jumpAnim.loop = false;

	// JUMP EFFECT (5 frames de 32x32)
	for (int i = 0; i < 5; ++i) jumpEffectAnim.frames.push_back({ i * 32, 0, 32, 32 });
	jumpEffectAnim.speed = 0.2f;
	jumpEffectAnim.loop = false;

	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), PLAYER_RENDER_WIDTH / 2, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	b2Body_SetFixedRotation(pbody->body, true);

	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	// Establecer estado inicial
	SetState(PlayerState::IDLE);
	isJumping = false;
	isGodMode = false; // <-- AÑADIDO (para asegurar)
	return true;
}

void Player::SetState(PlayerState state)
{
	if (currentState == state) return;

	currentState = state;
	switch (currentState)
	{
	case PlayerState::IDLE:
		currentAnimation = &idleAnim;
		currentTexture = idleTexture;
		break;
	case PlayerState::WALK:
		currentAnimation = &walkAnim;
		currentTexture = walkTexture;
		break;
	case PlayerState::SPRINT:
		currentAnimation = &sprintAnim;
		currentTexture = sprintTexture;
		break;
	case PlayerState::JUMP:
		currentAnimation = &jumpAnim;
		currentTexture = jumpTexture;
		jumpAnim.Reset();
		break;
	}
}

// --- FUNCIONES DE LA MÁQUINA DE ESTADOS (Implementación) ---

void Player::HandleIdle(float dt)
{
	desiredVelX = 0.0f;

	// Comprobar salto
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		airSpeedLimit = walkSpeed; // Saltar con velocidad de caminata si estamos quietos
		DoJump();
		return; // El estado ha cambiado, salimos
	}

	// Comprobar movimiento
	bool isSprinting = (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT);
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		SetState(isSprinting ? PlayerState::SPRINT : PlayerState::WALK);
	}
}

void Player::HandleWalk(float dt)
{
	bool isSprinting = (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT);

	// Comprobar salto
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		airSpeedLimit = walkSpeed;
		DoJump();
		return; // El estado ha cambiado, salimos
	}

	// Comprobar cambio de estado (Sprint o Parado)
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		desiredVelX = -walkSpeed;
		flip = true;
		if (isSprinting) SetState(PlayerState::SPRINT);
	}
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		desiredVelX = walkSpeed;
		flip = false;
		if (isSprinting) SetState(PlayerState::SPRINT);
	}
	else
	{
		// No hay input de movimiento, volvemos a IDLE
		SetState(PlayerState::IDLE);
	}
}

void Player::HandleSprint(float dt)
{
	bool isSprinting = (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT);

	// Comprobar salto
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		airSpeedLimit = sprintSpeed;
		DoJump();
		return; // El estado ha cambiado, salimos
	}

	// Comprobar cambio de estado (Walk o Parado)
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		desiredVelX = -sprintSpeed;
		flip = true;
		if (!isSprinting) SetState(PlayerState::WALK);
	}
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		desiredVelX = sprintSpeed;
		flip = false;
		if (!isSprinting) SetState(PlayerState::WALK);
	}
	else
	{
		// No hay input de movimiento, volvemos a IDLE
		SetState(PlayerState::IDLE);
	}
}

void Player::HandleJump(float dt)
{
	// 'isJumping' se pondrá a 'false' y el estado a 'IDLE' 
	// desde OnCollision o la lógica de plataformas áereas.

	// Manejar control en el aire
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		desiredVelX = -airSpeedLimit;
		flip = true;
	}
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		desiredVelX = airSpeedLimit;
		flip = false;
	}
	else
	{
		desiredVelX = 0.0f; // Sin control en el aire si no se pulsa nada
	}
}

void Player::DoJump()
{
	Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
	isJumping = true; // Necesario para la lógica de OnCollision
	SetState(PlayerState::JUMP);

	// Activar y posicionar el efecto de salto
	showJumpEffect = true;
	jumpEffectPosition.setX(position.getX() - (PLAYER_RENDER_WIDTH / 2));
	jumpEffectPosition.setY(position.getY() + (PLAYER_RENDER_HEIGHT / 2) - 32);
	jumpEffectAnim.Reset();
}

// <-- AÑADIR ESTA FUNCIÓN COMPLETA -->
void Player::ToggleGodMode()
{
	isGodMode = !isGodMode;
	if (isGodMode)
	{
		// Desactivar gravedad para el jugador
		b2Body_SetGravityScale(pbody->body, 0.0f);
	}
	else
	{
		// Reactivar gravedad para el jugador
		b2Body_SetGravityScale(pbody->body, 1.0f);
	}
}


// --- FUNCIÓN UPDATE PRINCIPAL (Refactorizada) ---

bool Player::Update(float dt)
{
	if (respawnCooldown > 0.0f) respawnCooldown -= dt / 1000.0f;

	Physics* physics = Engine::GetInstance().physics.get();

	// <-- MODIFICADO: Añadido chequeo de God Mode -->
	// --- 1. GESTIONAR LÓGICA DE ESTADO ---
	if (isGodMode)
	{
		// Lógica de vuelo libre (W/A/S/D)
		float godSpeed = 7.0f;
		desiredVelX = 0.0f;
		float desiredVelY = 0.0f;

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) desiredVelY = -godSpeed;
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) desiredVelY = godSpeed;
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) desiredVelX = -godSpeed;
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) desiredVelX = godSpeed;

		physics->SetLinearVelocity(pbody, desiredVelX, desiredVelY);
		SetState(PlayerState::IDLE); // Usar anim IDLE
	}
	else
	{
		// Lógica FSM Normal (si no estamos en Modo Dios)
		switch (currentState)
		{
		case PlayerState::IDLE:
			HandleIdle(dt);
			break;
		case PlayerState::WALK:
			HandleWalk(dt);
			break;
		case PlayerState::SPRINT:
			HandleSprint(dt);
			break;
		case PlayerState::JUMP:
			HandleJump(dt);
			break;
		}

		// --- 2. APLICAR FÍSICAS ---
		// 'desiredVelX' ha sido establecido por la función de estado
		physics->SetXVelocity(pbody, desiredVelX);
	}
	// <-- FIN DE LA MODIFICACIÓN DE GOD MODE -->


	// --- 3. ACTUALIZAR POSICIÓN Y RENDER ---
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// --- 4. LÓGICA DE PLATAFORMAS ATRAVESABLES ---
	// (Esto se queda aquí, ya que es una comprobación de física post-movimiento)
	float velY = physics->GetYVelocity(pbody);
	if (isOverlappingOneWayPlatform && velY > 0 && currentOneWayPlatform != nullptr)
	{
		int playerBottomY = y + (PLAYER_RENDER_WIDTH / 2); // Radio
		int platformX, platformY;
		currentOneWayPlatform->GetPosition(platformX, platformY);
		int platformTopY = platformY - (Engine::GetInstance().map->mapData.tileHeight / 2); // Semi-altura

		if (playerBottomY >= platformTopY && playerBottomY < (platformTopY + 10))
		{
			physics->SetYVelocity(pbody, 0.0f);
			int correctedY = platformTopY - (PLAYER_RENDER_WIDTH / 2);
			physics->SetPosition(pbody, (float)x, (float)correctedY);
			position.setY((float)correctedY);

			if (isJumping) {
				isJumping = false;
				SetState(PlayerState::IDLE);
			}
		}
	}

	// --- 5. RENDERIZAR EFECTO DE SALTO ---
	if (showJumpEffect)
	{
		SDL_Rect effectFrame = jumpEffectAnim.GetCurrentFrame();
		Engine::GetInstance().render->DrawTexture(jumpEffectTexture, (int)jumpEffectPosition.getX(), (int)jumpEffectPosition.getY(), &effectFrame, 1.0f, 0, INT_MAX, INT_MAX, 32, 32);
		if (jumpEffectAnim.HasFinished())
		{
			showJumpEffect = false;
		}
	}

	// --- 6. RENDERIZAR JUGADOR ---
	SDL_Rect currentFrameRect = currentAnimation->GetCurrentFrame();
	SDL_FlipMode flipFlag = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	int drawX = x - (PLAYER_RENDER_WIDTH / 2);
	int drawY = y - (PLAYER_RENDER_HEIGHT / 2);
	Engine::GetInstance().render->DrawTexture(currentTexture, drawX, drawY, &currentFrameRect, 1.0, 0.0, INT_MAX, INT_MAX, PLAYER_RENDER_WIDTH, PLAYER_RENDER_HEIGHT, flipFlag);

	// --- 7. COMPROBAR MUERTE / RESPAWN ---
	int mapHeight = Engine::GetInstance().map->mapData.height * Engine::GetInstance().map->mapData.tileHeight;

	// <-- MODIFICADO: Añadido !isGodMode -->
	if (position.getY() > mapHeight && respawnCooldown <= 0.0f && !isGodMode)
	{
		Respawn();
	}

	// --- 8. RENDERIZAR UI (VIDAS) ---
	int spacing = 5;
	int startX = 20;
	int startY = 20;
	int iconW = 0, iconH = 0;
	if (HP) {
		Engine::GetInstance().textures->GetSize(HP, iconW, iconH);
		for (int i = 0; i < lives; ++i)
		{
			int xPos = startX + i * (iconW + spacing);
			Engine::GetInstance().render->DrawTexture(HP, xPos, startY, NULL, 0.0f);
		}
	}

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	if (idleTexture) Engine::GetInstance().textures->UnLoad(idleTexture);
	if (walkTexture) Engine::GetInstance().textures->UnLoad(walkTexture);
	if (sprintTexture) Engine::GetInstance().textures->UnLoad(sprintTexture);
	if (jumpTexture) Engine::GetInstance().textures->UnLoad(jumpTexture);
	if (jumpEffectTexture) Engine::GetInstance().textures->UnLoad(jumpEffectTexture);
	if (HP) Engine::GetInstance().textures->UnLoad(HP);

	if (pbody != nullptr) {
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		if (isJumping) // Solo nos importa al aterrizar
		{
			isJumping = false;
			SetState(PlayerState::IDLE);
		}
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);
		physB->listener->Destroy();
		break;
	case ColliderType::ONE_WAY_PLATFORM:
		isOverlappingOneWayPlatform = true;
		currentOneWayPlatform = physB;
		break;
	case ColliderType::WATER:
		// <-- MODIFICADO: Añadido chequeo de God Mode -->
		if (isGodMode) break; // Si somos Dios, ignoramos el agua

		LOG("Collision WATER");
		if (respawnCooldown <= 0.0f) {
			Respawn();
		}
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	if (physB->ctype == ColliderType::ONE_WAY_PLATFORM)
	{
		isOverlappingOneWayPlatform = false;
		currentOneWayPlatform = nullptr;
	}
}

void Player::Respawn()
{
	lives--;
	if (lives > 0)
	{
		Engine::GetInstance().physics->SetPosition(pbody, initialPosition.getX(), initialPosition.getY());
		Engine::GetInstance().physics->SetLinearVelocity(pbody, 0.0f, 0.0f);
		isJumping = false;
		SetState(PlayerState::IDLE);
		respawnCooldown = 1.0f;
		LOG("Vidas restantes: %d", lives);
	}
	else
	{
		LOG("¡Game Over! El jugador se ha quedado sin vidas.");
		Engine::GetInstance().audio->StopMusic();
		Engine::GetInstance().RequestSceneChange(std::make_shared<GameOverScene>());
	}
}