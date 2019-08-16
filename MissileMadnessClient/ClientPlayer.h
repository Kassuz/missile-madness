#pragma once

#include "Player.h"

class Sprite;
class DebugPos;

class ClientPlayer : public Player
{
public:
	static NetworkedGameObject* CreateInstance(UInt32 networkID) { return static_cast<NetworkedGameObject*>(new ClientPlayer(networkID)); }

	virtual void Read(InputMemoryBitStream& packet) override;

	void Initialize(Texture2D* playerTexture);

	virtual ~ClientPlayer();
	ClientPlayer(const ClientPlayer&) = delete;
	ClientPlayer& operator=(const ClientPlayer&) = delete;

	bool IsLocal();

	void Update(float dataIntervall);

private:
	ClientPlayer(UInt32 networkID);

	Sprite* m_PlayerSprite = nullptr;

	bool m_HasRecievedData = false;
	glm::vec3 m_StartPos, m_InterpolatePos;
	glm::quat m_StartRot, m_InterpolateRot;

	float m_LastDataTime = 0.0f;

	DebugPos* m_DebugPos = nullptr;
};

