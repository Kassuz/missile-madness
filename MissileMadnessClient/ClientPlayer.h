#pragma once

#include "Player.h"

class Sprite;

class ClientPlayer : public Player
{
public:
	static NetworkedGameObject* CreateInstance(UInt32 networkID) { return static_cast<NetworkedGameObject*>(new ClientPlayer(networkID)); }

	void Initialize(Texture2D* playerTexture);

	virtual ~ClientPlayer() { delete m_PlayerSprite; }
	ClientPlayer(const ClientPlayer&) = delete;
	ClientPlayer& operator=(const ClientPlayer&) = delete;

	bool IsLocal();

private:
	ClientPlayer(UInt32 networkID) : Player(networkID) { }

	Sprite* m_PlayerSprite = nullptr;
};

