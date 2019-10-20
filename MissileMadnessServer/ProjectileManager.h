#pragma once

#include <glm/glm.hpp>
#include <vector>

class ServerPlayer;
class ServerProjectile;
class NetworkManagerServer;

class ProjectileManager
{

public:
	ProjectileManager();
	~ProjectileManager();
	ProjectileManager(const ProjectileManager& other) = delete;
	ProjectileManager& operator=(const ProjectileManager& other) = delete;

	void Init(NetworkManagerServer* netManager)
	{
		m_NetworkManagerServer = netManager;
	}

	void SpawnProjectile(ServerPlayer* owner, glm::vec3 position, glm::vec3 direction);

	void Update();

	std::vector<ServerProjectile*> GetProjectiles() const { return m_Projectiles; }


private:
	NetworkManagerServer* m_NetworkManagerServer = nullptr;
	std::vector<ServerProjectile*> m_Projectiles;
};

