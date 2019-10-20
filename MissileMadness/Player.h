#pragma once

#include "Networking/NetworkedGameObject.h"
#include "Sprite.h"

#include <glm/glm.hpp>

class Texture2D;
class Move;
class User;

class Player : public NetworkedGameObject
{
public:
	CLASS_IDENTIFICATION('PLAY', Player);

	virtual void Write(OutputMemoryBitStream& packet) override;
	virtual void Read(InputMemoryBitStream& packet) override;

	virtual ~Player() { }
	// No making copies of players
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	const float GetHealth() const { return m_Health; }
	bool IsDead() { return m_Health <= 0.0f; }

	const UInt32 GetUserID() const;

	void ProcessMove(Move* m, float deltaT);

protected:
	Player(UInt32 networkID) : NetworkedGameObject(networkID) 
	{
		m_Transform.SetScale(glm::vec3(k_PlayerScale));
	}

	User* m_User = nullptr;

	const float k_MaxHealth = 100.0f;
	float m_Health = 100.0f;

	const float k_MovementSpeed = 350.0f;
	const float k_RotateSpeed = 5.0f;

	glm::vec3 m_VelocityDir;

	const float k_ProjectileCooldown = 0.1f;
	float m_ProjectileTimer = 0.0f;

	const float k_PlayerScale = 100.0f; // Player scale is constant

	const glm::vec4 k_ShootPos = glm::vec4(0.25f, 0.0f, 0.0f, 1.0f);

};



