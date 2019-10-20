#pragma once

#include "Player.h"

class ServerGame;
class User;

class ServerPlayer : public Player
{
public:
	static NetworkedGameObject* CreateInstance(UInt32 networkID) { return static_cast<NetworkedGameObject*>(new ServerPlayer(networkID)); }

	void Initialize(User* controllingUser, ServerGame* game);

	void Update();

	// Returns true if killed
	bool TakeDamage(float damage);
	void Reset(glm::vec3 respawnPos);

private:
	const UInt32 k_MaxMovesPerUpdate = 5U;

	ServerPlayer(UInt32 networkID) : Player(networkID) { }

	ServerGame* m_Game = nullptr;

	void ShootProjectile();
};