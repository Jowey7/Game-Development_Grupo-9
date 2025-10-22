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

	SetState(PlayerState::IDLE);

	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), PLAYER_RENDER_WIDTH / 2, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	b2Body_SetFixedRotation(pbody->body, true);

	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

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

bool Player::Update(float dt)
{
	if (respawnCooldown > 0.0f) respawnCooldown -= dt / 1000.0f;

	Physics* physics = Engine::GetInstance().physics.get();
	float desiredVelX = 0.0f;

	// --- LÓGICA DE MOVIMIENTO ---

	if (isJumping)
	{
		// 1. EL JUGADOR ESTÁ EN EL AIRE
		SetState(PlayerState::JUMP);

		// Lee el input horizontal, pero la velocidad está limitada por airSpeedLimit
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
	}
	else
	{
		// 2. EL JUGADOR ESTÁ EN EL SUELO
		bool isSprinting = (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || Engine::GetInstance().input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT);
		bool isMoving = false;

		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			desiredVelX = isSprinting ? -sprintSpeed : -walkSpeed;
			flip = true;
			isMoving = true;
		}
		else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			desiredVelX = isSprinting ? sprintSpeed : walkSpeed;
			flip = false;
			isMoving = true;
		}

		// Establecer estado en el suelo
		if (isMoving)
		{
			SetState(isSprinting ? PlayerState::SPRINT : PlayerState::WALK);
		}
		else
		{
			SetState(PlayerState::IDLE);
		}

		// Comprobar si salta
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			// Guarda la velocidad máxima que tendrá en el aire
			airSpeedLimit = isMoving ? (isSprinting ? sprintSpeed : walkSpeed) : walkSpeed;

			physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
			isJumping = true;
			SetState(PlayerState::JUMP);
			// Activa y posiciona el efecto de salto
			showJumpEffect = true;
			jumpEffectPosition.setX(position.getX() - (PLAYER_RENDER_WIDTH / 2));
			jumpEffectPosition.setY(position.getY() + (PLAYER_RENDER_HEIGHT / 2) - 32);
			jumpEffectAnim.Reset();
		}
	}

	physics->SetXVelocity(pbody, desiredVelX);

	// --- ACTUALIZACIÓN Y RENDERIZADO ---
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	if (showJumpEffect)
	{
		SDL_Rect effectFrame = jumpEffectAnim.GetCurrentFrame();
		Engine::GetInstance().render->DrawTexture(jumpEffectTexture, (int)jumpEffectPosition.getX(), (int)jumpEffectPosition.getY(), &effectFrame, 1.0f, 0, INT_MAX, INT_MAX, 32, 32);
		if (jumpEffectAnim.HasFinished())
		{
			showJumpEffect = false;
		}
	}

	SDL_Rect currentFrameRect = currentAnimation->GetCurrentFrame();
	SDL_FlipMode flipFlag = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	int drawX = x - (PLAYER_RENDER_WIDTH / 2);
	int drawY = y - (PLAYER_RENDER_HEIGHT / 2);
	Engine::GetInstance().render->DrawTexture(currentTexture, drawX, drawY, &currentFrameRect, 1.0, 0.0, INT_MAX, INT_MAX, PLAYER_RENDER_WIDTH, PLAYER_RENDER_HEIGHT, flipFlag);

	int mapHeight = Engine::GetInstance().map->mapData.height * Engine::GetInstance().map->mapData.tileHeight;
	if (position.getY() > mapHeight && respawnCooldown <= 0.0f)
	{
		Respawn();
	}

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
		if (isJumping)
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
	case ColliderType::WATER:
		LOG("Collision WATER");
		if (respawnCooldown <= 0.0f) {
			Respawn();
		}
		break;
	default:
		break;
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