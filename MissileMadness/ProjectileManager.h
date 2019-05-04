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

private:
	ProjectileManager();
	~ProjectileManager();
	
	Texture2D* missileTexture;

	float projectileLifetime = 2.0f;
	float projectileSpeed = 500.0f;

	std::vector<Projectile*> projectiles;
};

