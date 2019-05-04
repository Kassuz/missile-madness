#pragma once

#include "GameObject.h"
#include "Sprite.h"

#include <glm/glm.hpp>

class Texture2D;

class Player : public GameObject
{
public:
	Player(Texture2D* palyerTexture, float health, bool controlledByUser);
	virtual ~Player();
	// No making copies of players
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void Update();

	void TakeDamage(float amount);
	bool IsDead() { return health <= 0.0f; }

	void Reset(glm::vec3 respawnPos);

private:

	Sprite sprite;

	bool isControlledByUser = false;

	const float maxHealth;
	float health;

	float maxSpeed = 350.0f;
	float rotateSpeed = 0.05f;

	float acceleration = 1000.0f;
	glm::vec3 velocity = glm::vec3(0.0f);

	float projectileCooldown = 0.1f;
	float projectileTimer = 0.0f;

	const glm::vec4 shootPos = glm::vec4(0.25f, 0.45f, 0.0f, 1.0f);

	void ShootProjectile();
};

