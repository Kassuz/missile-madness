#pragma once

#include <glm/glm.hpp>
#include <vector>

class Texture2D;
class Player;
class Projectile;

class ProjectileManager
{

public:
	static ProjectileManager& Instance();

	void SpawnProjectile(Player* owner, glm::vec3 position, glm::vec3 direction);

	void Update();

	std::vector<Projectile*> GetProjectiles() const { return projectiles; }

private:
	ProjectileManager();
	~ProjectileManager();
	ProjectileManager(const ProjectileManager& other) = delete;
	ProjectileManager& operator=(const ProjectileManager& other) = delete;

	
	Texture2D* missileTexture;

	std::vector<Projectile*> projectiles;
};

