#pragma once

#include "GameObject.h"
#include "Sprite.h"
#include <glm/glm.hpp>

class Texture2D;
class Player;

class Projectile : public GameObject
{
public:
	Projectile(Player* owner, Texture2D* missileTexture, glm::vec3 position, float speed, glm::vec3 direction, float lifetime);
	virtual ~Projectile();

	void Update();
	bool ShouldBeDestroyed();

	Player* GetOwner() const { return owner; }

private:
	Player* owner;
	Sprite sprite;

	float lifetime;
	float lifeTimer = 0.0f;
	glm::vec3 velocity;
};

