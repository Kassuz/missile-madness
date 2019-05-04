#pragma once

#include "GameObject.h"
#include "Sprite.h"
#include <glm/glm.hpp>

class Texture2D;
class Player;

class Projectile : public GameObject
{
public:
	Projectile(Player* owner, Texture2D* missileTexture, glm::vec3 position, glm::vec3 direction);
	virtual ~Projectile();
	//No making copies of projectiles
	Projectile(const Projectile&) = delete;
	Projectile& operator=(const Projectile&) = delete;

	void Update();
	bool ShouldBeDestroyed() { return hasHit || lifeTimer >= projectileLifetime; }
	void ProjectileHit() { hasHit = true; }

	Player* GetOwner() const { return owner; }

	constexpr float GetProjectileDamage() { return projectileDamage; }

private:

	Player* owner;
	Sprite sprite;

	const float projectileLifetime = 2.0f;
	const float projectileSpeed = 500.0f;
	const float projectileDamage = 150.0f;

	float lifeTimer = 0.0f;
	glm::vec3 velocity;

	bool hasHit = false;
};

