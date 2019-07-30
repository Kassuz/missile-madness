#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Types.h"

#include "ProjectileManager.h"

class ServerPlayer;
class User;

class ServerGame
{

public:
	ServerGame(UInt32 screenWidth, UInt32 screenHeight, std::vector<User*> users);
	~ServerGame();

	void Update();
	void HandlePlayerDisconnect(ServerPlayer* player);

	ProjectileManager& GetProjectileManager() { return m_ProjectileManager; }

private:
	std::vector<ServerPlayer*> m_Players;
	std::queue<ServerPlayer*>  m_Disconnected;

	struct Wall
	{
	public:
		glm::vec2 GetStart()  const { return start; }
		glm::vec2 GetEnd()    const { return end; }
		glm::vec2 GetNormal() const { return normal; }

		Wall() : start(glm::vec2()), end(glm::vec2()), normal(glm::vec2())
		{}
		Wall(glm::vec2 start, glm::vec2 end, glm::vec2 normal) : start(start), end(end), normal(normal)
		{}
		Wall& operator=(const Wall& ohter)
		{
			this->start = ohter.start;
			this->end = ohter.end;
			this->normal = ohter.normal;
			return *this;
		}
		~Wall() {}
	private:
		glm::vec2 start, end, normal;
	};

	Wall m_Walls[4];
	ProjectileManager m_ProjectileManager;
	
	const float k_PlayerColliderRadius = 35.0f;
	const float k_MissileColliderRadius = 10.0f;
	const float k_CollisionPushForce = 12.0f;

	void HandleWallCollisions();
	void HandlePlayerToPlayerCollisions();
	void HandleProjectileCollisons();

	const float k_RespawnTime = 3.0f;
	std::unordered_map<ServerPlayer*, float> m_Respawns;
	void RespawnPlayers();

	
};

