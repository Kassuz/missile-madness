#pragma once

#include "Projectile.h"

class Texture2D;
class Sprite;
class DebugPos;

class ClientProjectile : public Projectile
{
public:
	static NetworkedGameObject* CreateInstance(UInt32 networkID) { return static_cast<NetworkedGameObject*>(new ClientProjectile(networkID)); }

	virtual void Read(InputMemoryBitStream& packet) override;

	void Initialize(Texture2D* tex);

	virtual ~ClientProjectile();
	ClientProjectile(const ClientProjectile&) = delete;
	ClientProjectile& operator=(const ClientProjectile&) = delete;

	void Update(float dataIntervall);

private:
	ClientProjectile(UInt32 networkID);

	Sprite* m_Sprite = nullptr;

	bool m_HasRecievedData = false;
	//glm::vec3 m_StartPos, m_InterpolatePos;
	//float m_LastDataTime = 0.0f;

	DebugPos* m_DebugPos = nullptr;
};

