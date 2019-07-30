#include "ServerProjectile.h"

#include "Engine.h"

void ServerProjectile::InitializeProjectile(Player* owner, glm::vec3 position, glm::vec3 direction)
{
	m_Owner = owner;

	m_Velocity = glm::normalize(direction) * k_ProjectileSpeed;
	m_Transform.SetPosition(position);

	float angle = -std::atan2(direction.x, direction.y);
	glm::quat rot(std::cos(angle / 2.0f), 0.0f, 0.0f, std::sin(angle / 2.0f));
	m_Transform.SetRotation(rot);
}

void ServerProjectile::Update()
{
	if (!m_IsActive) return;

	m_Transform.Translate(m_Velocity * Time::deltaTime);
	m_LifeTimer += Time::deltaTime;
}