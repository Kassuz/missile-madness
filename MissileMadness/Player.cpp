#include "Player.h"
#include "Engine.h"
#include "ProjectileManager.h"

#include <GLFW/glfw3.h>

Player::Player(Texture2D* playerTexture, float health, bool controlledByUser) 
	: sprite(playerTexture, Color::White(), this, 0), health(health), maxHealth(health), isControlledByUser(controlledByUser)
{
	transform.SetScale(glm::vec3(100.0f));
}


Player::~Player()
{
}

void Player::Update()
{
	if (!isActive) return;
	if (!isControlledByUser) return;

	// MOVEMENT
	if (InputManager::Instance().GetKey(GLFW_KEY_D) || InputManager::Instance().GetKey(GLFW_KEY_RIGHT))
	{
		glm::quat rot = glm::angleAxis(-rotateSpeed, glm::vec3(0.0f, 0.0f, 1.0f)) * Time::deltaTime;
		transform.Rotate(rot);
	}

	if (InputManager::Instance().GetKey(GLFW_KEY_A) || InputManager::Instance().GetKey(GLFW_KEY_LEFT))
	{
		glm::quat rot = glm::angleAxis(rotateSpeed, glm::vec3(0.0f, 0.0f, 1.0f)) * Time::deltaTime;
		transform.Rotate(rot);
	}

	if (InputManager::Instance().GetKey(GLFW_KEY_W) || InputManager::Instance().GetKey(GLFW_KEY_UP))
		velocity += transform.GetUp() * Time::deltaTime * acceleration;
	
	if (InputManager::Instance().GetKey(GLFW_KEY_S) || InputManager::Instance().GetKey(GLFW_KEY_DOWN))
		velocity -= transform.GetUp() * Time::deltaTime * acceleration;

	velocity = glm::clamp(velocity, -maxSpeed, maxSpeed);
	transform.Translate(velocity * Time::deltaTime);

	velocity *= 0.98f;

	// SHOOTING
	if (projectileTimer > 0.0f)
	{
		projectileTimer -= Time::deltaTime;
	}
	else
	{
		if (InputManager::Instance().GetKeyDown(GLFW_KEY_SPACE))
			ShootProjectile();
	}
}

void Player::TakeDamage(float amount)
{
	this->health -= amount;

	Debug::LogWarning("Player take damage. Current health: " + std::to_string(health));

	if (IsDead())
		SetActive(false);
}

void Player::Reset(glm::vec3 respawnPos)
{
	Debug::LogWarning("Player Respawn");
	transform.SetPosition(respawnPos);
	health = maxHealth;
	SetActive(true);
}

void Player::ShootProjectile()
{
	projectileTimer = projectileCooldown;

	// Pew pew
	Debug::Log("Pew pew");
	//auto pos = transform.GetModel() * shootPos;
	glm::vec3 missilePos = glm::vec3(transform.GetModel() * shootPos);
	ProjectileManager::Instance().SpawnProjectile(this, missilePos, transform.GetUp());
}
