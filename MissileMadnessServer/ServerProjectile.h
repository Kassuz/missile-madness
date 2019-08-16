#pragma once

#include "Projectile.h"

class ServerProjectile : public Projectile
{
public:
	static NetworkedGameObject* CreateInstance(UInt32 networkID) { return static_cast<NetworkedGameObject*>(new ServerProjectile(networkID)); }

	virtual ~ServerProjectile() { }
	//No making copies of projectiles
	ServerProjectile(const ServerProjectile&) = delete;
	ServerProjectile& operator=(const ServerProjectile&) = delete;

	void InitializeProjectile(Player* owner, glm::vec3 position, glm::vec3 direction);

	void Update();
	bool ShouldBeDestroyed() { return m_HasHit || m_LifeTimer >= k_ProjectileLifetime; }
	void ProjectileHit() { m_HasHit = true; }

	constexpr float GetProjectileDamage() { return k_ProjectileDamage; }

private:

	ServerProjectile(UInt32 networkID) : Projectile(networkID) { }

	const float k_ProjectileLifetime = 2.0f;
	const float k_ProjectileSpeed = 500.0f;
	const float k_ProjectileDamage = 150.0f;

	float m_LifeTimer = 0.0f;
	

	bool m_HasHit = false;

};

