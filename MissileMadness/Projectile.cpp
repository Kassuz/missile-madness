#include "Projectile.h"
#include "Engine.h"

#include <glm/gtx/quaternion.hpp>

Projectile::Projectile(Player* owner, Texture2D* missileTexture, glm::vec3 position, glm::vec3 direction)
	: owner(owner), sprite(missileTexture, Color::White(), this, 1)
{
	velocity = glm::normalize(direction) * projectileSpeed;
	transform.SetPosition(position);
	transform.SetScale(glm::vec3(30.0f, 30.0f, 1.0f));
	
	float angle = -std::atan2(direction.x, direction.y);
	glm::quat rot(std::cos(angle / 2.0f), 0.0f, 0.0f, std::sin(angle / 2.0f));
	transform.SetRotation(rot);
}

Projectile::~Projectile()
{
}

void Projectile::Update()
{
	if (!isActive) return;

	transform.Translate(velocity * Time::deltaTime);
	lifeTimer += Time::deltaTime;
}
