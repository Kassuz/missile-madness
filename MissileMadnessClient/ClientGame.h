#pragma once

#include <vector>

#include "ClientPlayer.h"

class Texture2D;

class ClientGame
{
public:
	ClientGame();
	~ClientGame() { }

	void Update();

	static NetworkedGameObject* CreatePlayer(UInt32 networkID);
	static NetworkedGameObject* CreateMissile(UInt32 networkID);
	static void DisconnectPlayer(UInt32 userID);

private:
	// static bad :(
	static Texture2D* s_PlayerTexture;
	static Texture2D* s_MissileTexture;

	static std::vector<ClientPlayer*> s_Players;

	float m_FrameTimes[50];
	int m_Currentframe = 0;
};

