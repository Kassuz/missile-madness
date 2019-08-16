#include "ServerProjectile.h"

#include "Engine.h"

void ServerProjectile::InitializeProjectile(Player* owner, glm::vec3 position, glm::vec3 direction)
{
	m_Owner = owner;

	m_Velocity = glm::normalize(direction) * k_ProjectileSpeed;
	m_Transform.SetPosition(position);

	SetRotation(direction);
}

void ServerProjectile::Update()
{
	if (!m_IsActive) return;

	Move(Time::deltaTime);
	m_LifeTimer += Time::deltaTime;
}