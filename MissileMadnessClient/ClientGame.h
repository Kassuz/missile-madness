#pragma once

#include <vector>

#include "ClientPlayer.h"
#include "ClientProjectile.h"
#include "ExtraMath.h"

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
	static void RemoveProjectile(ClientProjectile*);

	//static void SetDataIntervall(float intervall) { s_DataIntervall = intervall; }

private:
	// static bad :(
	// TODO: Figure out better solution
	static Texture2D* s_PlayerTexture;
	static Texture2D* s_MissileTexture;

	static std::vector<ClientPlayer*> s_Players;
	static std::vector<ClientProjectile*> s_Projectiles;

	//static float s_DataIntervall;

	ExtraMath::AverageValue<float> m_AvgFrameTime;
};

