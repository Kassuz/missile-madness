#include "Projectile.h"
#include "Engine.h"

#include "Player.h"

#include <glm/gtx/quaternion.hpp>

void Projectile::Write(OutputMemoryBitStream& packet)
{
	packet.Write(GetNetworkID());
	packet.Write(GetClassID());

	packet.Write(m_Transform.GetPosition());
	packet.Write(m_Transform.GetRotation());

	//packet.Write(m_Owner->GetNetworkID());
}

void Projectile::Read(InputMemoryBitStream& packet)
{
	// NetworkID and CLassID read elsewhere
	glm::vec3 pos;
	glm::quat rot;

	packet.Read(pos);
	packet.Read(rot);

	m_Transform.SetPosition(pos);
	m_Transform.SetRotation(rot);
}



