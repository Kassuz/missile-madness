#pragma once

#include "Projectile.h"

class Texture2D;
class Sprite;

class ClientProjectile : public Projectile
{
public:
	static NetworkedGameObject* CreateInstance(UInt32 networkID) { return static_cast<NetworkedGameObject*>(new ClientProjectile(networkID)); }

	void Initialize(Texture2D* tex);

	virtual ~ClientProjectile() { delete m_Sprite; }
	ClientProjectile(const ClientProjectile&) = delete;
	ClientProjectile& operator=(const ClientProjectile&) = delete;

private:
	ClientProjectile(UInt32 networkID) : Projectile(networkID) { }

	Sprite* m_Sprite = nullptr;
};

