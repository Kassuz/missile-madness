#include "Projectile.h"
#include "Engine.h"

#include "Player.h"

#include <glm/gtx/quaternion.hpp>

void Projectile::Write(OutputMemoryBitStream& packet)
{
	packet.Write(GetNetworkID());
	packet.Write(GetClassID());

	packet.Write(m_Transform.GetPosition());
	packet.Write(m_Velocity);
	//packet.Write(m_Transform.GetRotation());

	//packet.Write(m_Owner->GetNetworkID());
}

void Projectile::Read(InputMemoryBitStream& packet)
{
	// NetworkID and CLassID read elsewhere
	glm::vec3 pos;
	//glm::quat rot;

	packet.Read(pos);
	packet.Read(m_Velocity);
	//packet.Read(rot);

	m_Transform.SetPosition(pos);
	//m_Transform.SetRotation(rot);
}

void Projectile::Move(float deltaTime)
{
	m_Transform.Translate(m_Velocity * deltaTime);
}

void Projectile::SetRotation(glm::vec3 direction)
{
	float angle = -std::atan2(direction.x, direction.y);
	glm::quat rot(std::cos(angle / 2.0f), 0.0f, 0.0f, std::sin(angle / 2.0f));
	m_Transform.SetRotation(rot);
}



