#include "Projectile.h"
#include "Engine.h"

#include <glm/gtx/quaternion.hpp>

Projectile::Projectile(Player* owner, Texture2D* missileTexture, glm::vec3 position, float speed, glm::vec3 direction, float lifetime)
	: owner(owner), sprite(missileTexture, Color::White(), &this->transform, 1), lifetime(lifetime)
{
	velocity = glm::normalize(direction) * speed;
	transform.SetPosition(position);
	transform.SetScale(glm::vec3(30.0f, 30.0f, 1.0f));
	
	float angle = -std::atan2(direction.x, direction.y);
	glm::quat rot(std::cos(angle / 2.0f), 0.0f, 0.0f, std::sin(angle / 2.0f));
	transform.SetRotation(rot);
	/*qx = 0
	qy = 1 * sin(angle / 2)
	qz = 0
	qw = cos(angle / 2)*/

}

Projectile::~Projectile()
{
}

void Projectile::Update()
{
	transform.Translate(velocity * Time::deltaTime);
	lifeTimer += Time::deltaTime;
}

bool Projectile::ShouldBeDestroyed()
{
	return lifeTimer >= lifetime;
}
