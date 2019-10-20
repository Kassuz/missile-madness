#include "ProjectileManager.h"

#include "Engine.h"
#include "ServerProjectile.h"
#include "ServerPlayer.h"

#include "NetworkManagerServer.h"


ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
}

void ProjectileManager::SpawnProjectile(ServerPlayer* owner, glm::vec3 position, glm::vec3 direction)
{
	ServerProjectile* p = m_NetworkManagerServer->CreateNetworkedGameObject<ServerProjectile>();
	p->InitializeProjectile(owner, position, direction);
	m_Projectiles.push_back(p);
}

void ProjectileManager::Update()
{
	for (auto it = m_Projectiles.begin(); it != m_Projectiles.end(); /**/)
	{
		ServerProjectile* p = *it;
		if (p->ShouldBeDestroyed())
		{
			it = m_Projectiles.erase(it);

			m_NetworkManagerServer->RemoveGameObject(p->GetNetworkID());
			m_NetworkManagerServer->RPC().SendDestroyNetworkedGameObjectRPC(p->GetNetworkID());
			delete p;
		}
		else
		{
			p->Update();
			++it;
		}
	}
}

