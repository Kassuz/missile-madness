#include "ProjectileManager.h"

#include "Engine.h"
#include "Projectile.h"

ProjectileManager& ProjectileManager::Instance()
{
	static ProjectileManager instance;
	return instance;
}

ProjectileManager::ProjectileManager()
{
	missileTexture = ResourceManager::Instance().LoadTexture2D("../Missile.png");
}

ProjectileManager::~ProjectileManager()
{
}

void ProjectileManager::SpawnProjectile(Player* owner, glm::vec3 position, glm::vec3 direction)
{
	Projectile* p = new Projectile(owner, missileTexture, position, direction);
	projectiles.push_back(p);
}

void ProjectileManager::Update()
{
	for (auto it = projectiles.begin(); it != projectiles.end(); /**/)
	{
		Projectile* p = *it;
		if (p->ShouldBeDestroyed())
		{
			it = projectiles.erase(it);
			delete p;
		}
		else
		{
			p->Update();
			++it;
		}
	}
}

