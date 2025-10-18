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

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {}

bool Player::Awake() {
	position = Vector2D(150, 300); // Ajustada la posición inicial Y para que no empiece en el suelo
	initialPosition = position;
	return true;
}

bool Player::Start() {
	// Se corrige la ruta de la textura del jugador
	texture = Engine::GetInstance().textures->Load("Assets/Player1/player1.png");

	// Nota: La textura HP.png no se encuentra en "Assets/Textures/". El programa no fallará, pero no se verá.
	HP = Engine::GetInstance().textures->Load("Assets/Textures/HP.png");

	Engine::GetInstance().textures->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	pbody->body = pbody->body;

	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	return true;
}

bool Player::Update(float dt)
{
	if (respawnCooldown > 0.0f) {
		respawnCooldown -= dt / 1000.0f;
	}

	Physics* physics = Engine::GetInstance().physics.get();

	float desiredVelX = 0;
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		desiredVelX = -5.0f;
	}
	else if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		desiredVelX = 5.0f;
	}
	physics->SetXVelocity(pbody, desiredVelX);

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !isJumping) {
		physics->SetYVelocity(pbody, 0);
		physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		isJumping = true;
	}

	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2);

	if (position.getY() > Engine::GetInstance().window->height + texH && respawnCooldown <= 0.0f)
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
	if (texture) Engine::GetInstance().textures->UnLoad(texture);
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
		isJumping = false;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);
		physB->listener->Destroy();
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {}

void Player::Respawn()
{
	lives--;
	if (lives > 0)
	{
		Engine::GetInstance().physics->SetPosition(pbody, initialPosition.getX(), initialPosition.getY());
		Engine::GetInstance().physics->SetLinearVelocity(pbody, 0.0f, 0.0f);
		isJumping = false;
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