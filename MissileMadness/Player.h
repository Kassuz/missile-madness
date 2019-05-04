#pragma once

#include "GameObject.h"
#include "Sprite.h"

class Texture2D;

class Player : public GameObject
{
public:
	Player(Texture2D* palyerTexture, bool controlledByUser);
	virtual ~Player();

	void Update();

	Sprite sprite;

private:
	bool isControlledByUser = false;

	float health = 100.0f;

	float maxSpeed = 350.0f;
	float rotateSpeed = 0.05f;

	float acceleration = 1000.0f;
	glm::vec3 velocity = glm::vec3(0.0f);

	float projectileCooldown = 1.0f;
	float projectileTimer = 0.0f;

	const glm::vec4 shootPos = glm::vec4(0.25f, 0.45f, 0.0f, 1.0f);

	void ShootProjectile();
};

