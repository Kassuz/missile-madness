#include "ProjectileManager.h"

#include "Engine.h"
#include "ServerProjectile.h"
#include "ServerPlayer.h"

#include "NetworkManagerServer.h"

//ProjectileManager& ProjectileManager::Instance()
//{
//	static ProjectileManager instance;
//	return instance;
//}

ProjectileManager::ProjectileManager()
{
	//missileTexture = ResourceManager::Instance().LoadTexture2D("Resources/Textures/Missile.png");
}

ProjectileManager::~ProjectileManager()
{
}

void ProjectileManager::SpawnProjectile(ServerPlayer* owner, glm::vec3 position, glm::vec3 direction)
{
	ServerProjectile* p = NetworkManagerServer::Instance().CreateNetworkedGameObject<ServerProjectile>();
	p->InitializeProjectile(owner, position, direction);
	m_Projectiles.push_back(p);
	
	//Projectile* p = new Projectile(owner, missileTexture, position, direction);
}

void ProjectileManager::Update()
{
	for (auto it = m_Projectiles.begin(); it != m_Projectiles.end(); /**/)
	{
		ServerProjectile* p = *it;
		if (p->ShouldBeDestroyed())
		{
			it = m_Projectiles.erase(it);

			NetworkManagerServer::Instance().RemoveGameObject(p->GetNetworkID());
			NetworkManagerServer::Instance().RPC().SendDestroyNetworkedGameObjectRPC(p->GetNetworkID());
			delete p;
		}
		else
		{
			p->Update();
			++it;
		}
	}
}

