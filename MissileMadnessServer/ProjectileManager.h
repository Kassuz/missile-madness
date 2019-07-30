#pragma once

#include <glm/glm.hpp>
#include <vector>

class ServerPlayer;
class ServerProjectile;

class ProjectileManager
{

public:
	//static ProjectileManager& Instance();
	ProjectileManager();

	void SpawnProjectile(ServerPlayer* owner, glm::vec3 position, glm::vec3 direction);

	void Update();

	std::vector<ServerProjectile*> GetProjectiles() const { return m_Projectiles; }

	~ProjectileManager();
	ProjectileManager(const ProjectileManager& other) = delete;
	ProjectileManager& operator=(const ProjectileManager& other) = delete;

private:
	std::vector<ServerProjectile*> m_Projectiles;
};

