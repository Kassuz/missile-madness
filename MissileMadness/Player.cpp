#include "Player.h"
#include "Engine.h"
#include "Move.h"

void Player::Write(OutputMemoryBitStream& packet)
{
	packet.Write(GetNetworkID());
	packet.Write(GetClassID());

	packet.Write(m_User);

	packet.Write(m_Transform.GetPosition());
	packet.Write(m_Transform.GetRotation());

	packet.Write(m_Health);
	packet.Write(m_VelocityDir);
}

void Player::Read(InputMemoryBitStream& packet)
{
	// ClassID and NetworkID have been read elsewhere
	packet.Read(m_User);

	glm::vec3 pos;
	packet.Read(pos);
	m_Transform.SetPosition(pos);

	glm::quat rot;
	packet.Read(rot);
	m_Transform.SetRotation(rot);

	packet.Read(m_Health);
	packet.Read(m_VelocityDir);
}

void Player::ProcessMove(Move* move, float deltaT)
{
	if (move->GetRightState())
	{
		glm::quat rot = glm::angleAxis(-k_RotateSpeed * deltaT, glm::vec3(0.0f, 0.0f, 1.0f));
		m_Transform.Rotate(rot);
	}

	if (move->GetLeftState())
	{
		glm::quat rot = glm::angleAxis(k_RotateSpeed * deltaT, glm::vec3(0.0f, 0.0f, 1.0f));
		m_Transform.Rotate(rot);
	}

	if (move->GetUpState())
	{
		m_Transform.Translate(m_Transform.GetUp() * deltaT * k_MovementSpeed);
	}

	if (move->GetDownState())
	{
		m_Transform.Translate(-m_Transform.GetUp() * deltaT * k_MovementSpeed);
	}
}

