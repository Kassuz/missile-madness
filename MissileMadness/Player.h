#pragma once

#include "Networking/NetworkedGameObject.h"
#include "Sprite.h"

#include <glm/glm.hpp>

class Texture2D;

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

	const UInt32 GetUserID() const { return m_User; }

protected:
	Player(UInt32 networkID) : NetworkedGameObject(networkID) { }

	UInt32 m_User = 0U;

	const float k_MaxHealth = 100.0f;
	float m_Health = 100.0f;

	const float k_MovementSpeed = 350.0f;
	const float k_RotateSpeed = 5.0f;

	const float k_ProjectileCooldown = 0.1f;
	float m_ProjectileTimer = 0.0f;

	const float k_PlayerScale = 100.0f; // Player scale is constant

	const glm::vec4 k_ShootPos = glm::vec4(0.25f, 0.45f, 0.0f, 1.0f);

};

//class Player : public GameObject
//{
//public:
//	Player(Texture2D* palyerTexture, float health, bool controlledByUser);
//	virtual ~Player();
//	// No making copies of players
//	Player(const Player&) = delete;
//	Player& operator=(const Player&) = delete;
//
//	void Update();
//
//	void TakeDamage(float amount);
//	bool IsDead() { return health <= 0.0f; }
//
//	void Reset(glm::vec3 respawnPos);
//
//private:
//
//	Sprite sprite;
//
//	bool isControlledByUser = false;
//
//	const float maxHealth;
//	float health;
//
//	float maxSpeed = 350.0f;
//	float rotateSpeed = 5.0f;
//
//	//float acceleration = 1000.0f;
//	//glm::vec3 velocity = glm::vec3(0.0f);
//
//	float projectileCooldown = 0.1f;
//	float projectileTimer = 0.0f;
//
//	const glm::vec4 shootPos = glm::vec4(0.25f, 0.45f, 0.0f, 1.0f);
//
//	void ShootProjectile();
//};

