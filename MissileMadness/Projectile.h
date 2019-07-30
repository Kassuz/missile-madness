#pragma once

#include "Networking/NetworkedGameObject.h"

//#include "Sprite.h"
#include <glm/glm.hpp>

class Player;

class Projectile : public NetworkedGameObject
{
public:
	CLASS_IDENTIFICATION('MISS', Projectile);

	virtual void Write(OutputMemoryBitStream& packet) override;
	virtual void Read(InputMemoryBitStream& packet) override;

	virtual ~Projectile() { }
	//No making copies of projectiles
	Projectile(const Projectile&) = delete;
	Projectile& operator=(const Projectile&) = delete;

	Player* GetOwner() const { return m_Owner; }

protected:
	Projectile(UInt32 networkID) : NetworkedGameObject(networkID) 
	{
		m_Transform.SetScale(k_ProjectileScale);
	}

	Player* m_Owner = nullptr;

	const glm::vec3 k_ProjectileScale = glm::vec3(30.0f, 30.0f, 1.0f);
};

