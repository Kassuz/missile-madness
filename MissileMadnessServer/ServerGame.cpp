#include "ServerGame.h"

#include "Engine.h"

#include "ServerPlayer.h"
#include "NetworkManagerServer.h"
#include "Networking/User.h"
#include "ServerProjectile.h"

#include <glm/gtc/random.hpp>

ServerGame::ServerGame(UInt32 screenWidth, UInt32 screenHeight, std::vector<User*> users)
{
	UInt32 playerCount = users.size();
	UInt32 counter = 0U;
	for (auto it : users)
	{
		ServerPlayer* p = NetworkManagerServer::Instance().CreateNetworkedGameObject<ServerPlayer>();
		p->Initialize(it->GetUserID(), this);

		float posX = std::cos((counter / (float)playerCount) * glm::two_pi<float>());
		float posY = std::sin((counter / (float)playerCount) * glm::two_pi<float>());

		p->GetTransform()->SetPosition(glm::vec3(posX, posY, 0.0f) * 250.0f);

		m_Players.push_back(p);

		counter++;
	}

	glm::vec2 bLeft(-(float)screenWidth / 2.0f, -(float)screenHeight / 2.0f);
	glm::vec2 tLeft(-(float)screenWidth / 2.0f, (float)screenHeight / 2.0f);
	glm::vec2 tRight((float)screenWidth / 2.0f, (float)screenHeight / 2.0f);
	glm::vec2 bRight((float)screenWidth / 2.0f, -(float)screenHeight / 2.0f);

	m_Walls[0] = Wall(tLeft, tRight, glm::vec2(0.0f, -1.0f));
	m_Walls[1] = Wall(bRight, tRight, glm::vec2(-1.0f, 0.0f));
	m_Walls[2] = Wall(bLeft, bRight, glm::vec2(0.0f, 1.0f));
	m_Walls[3] = Wall(bLeft, tLeft, glm::vec2(1.0f, 0.0f));


}


ServerGame::~ServerGame()
{
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it)
	{
		NetworkManagerServer::Instance().RemoveGameObject((*it)->GetNetworkID());
		delete (*it);
	}
}

void ServerGame::Update()
{
	// Update players
	//------------------------------------------------------------
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it)
	{
		(*it)->Update();
	}

	// Handle disconnects
	//------------------------------------------------------------
	while (m_Disconnected.size() > 0)
	{
		ServerPlayer* p = m_Disconnected.front();
		m_Disconnected.pop();
		
		for (auto it = m_Players.begin(); it != m_Players.end(); ++it)
		{
			if ((*it) == p)
			{
				NetworkManagerServer::Instance().RemoveGameObject(p->GetNetworkID());
				m_Players.erase(it);
				break;
			}
		}

		delete p;
	}

	// Update projectiles
	//------------------------------------------------------------
	m_ProjectileManager.Update();

	// Handle all collisions
	//------------------------------------------------------------
	HandleProjectileCollisons();
	HandlePlayerToPlayerCollisions();
	HandleWallCollisions();

	// Check respawns
	//------------------------------------------------------------
	RespawnPlayers();
}

void ServerGame::HandlePlayerDisconnect(ServerPlayer* player)
{
	m_Disconnected.push(player);
}

void ServerGame::HandleWallCollisions()
{
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it)
	{
		if (!(*it)->IsActive()) continue;
		for (int i = 0; i < 4; ++i)
		{
			Transform* playerTransform = (*it)->GetTransform();
			glm::vec2 playerPos = glm::vec2(playerTransform->GetPosition());
			glm::vec2 pointOnWall = ExtraMath::ClosestPointOnLineSegment(playerPos, m_Walls[i].GetStart(), m_Walls[i].GetEnd());
			glm::vec2 toPlayer = playerPos - pointOnWall;

			float dist = glm::length(toPlayer);

			//Debug::Log("Player dist from top: " + std::to_string(dist));
			//Debug::Log("Player pos: " + std::to_string(playerPos.x) + " " + std::to_string(playerPos.y));
			//Debug::Log("Wall pos: " + std::to_string(pointOnWall.x) + " " + std::to_string(pointOnWall.y));

			if (dist < k_PlayerColliderRadius || glm::dot(toPlayer, m_Walls[i].GetNormal()) < 0.0f)
			{
				glm::vec2 pos = pointOnWall + glm::normalize(toPlayer) * k_PlayerColliderRadius;
				playerTransform->SetPosition(glm::vec3(pos, 0.0f));
			}

			//if (glm::dot(toPlayer, m_Walls[i].GetNormal()) < 0.0f)
			//{
			//	glm::vec3 dir(m_Walls[i].GetNormal().x, m_Walls[i].GetNormal().y, 0.0f);
			//	playerTransform->Translate(dir * (dist + k_PlayerColliderRadius) * Time::deltaTime * k_CollisionPushForce);
			//}
			//else if (dist < k_PlayerColliderRadius)
			//{
			//	glm::vec3 dir(m_Walls[i].GetNormal().x, m_Walls[i].GetNormal().y, 0.0f);
			//	playerTransform->Translate(dir * dist * Time::deltaTime * k_CollisionPushForce);
			//}
		}
	}
}

void ServerGame::HandlePlayerToPlayerCollisions()
{
	Transform* p1Trans, * p2Trans;
	for (auto p1 = m_Players.begin(); p1 != m_Players.end(); ++p1)
	{
		if (!(*p1)->IsActive()) continue;

		p1Trans = (*p1)->GetTransform();
		for (auto p2 = m_Players.begin(); p2 != m_Players.end(); ++p2)
		{
			if (!(*p2)->IsActive()) continue;

			if (p1 == p2) continue;
			p2Trans = (*p2)->GetTransform();
			
			glm::vec3 toP2 = p2Trans->GetPosition() - p1Trans->GetPosition();
			float dist = glm::length(toP2);

			if (dist < 2.0f * k_PlayerColliderRadius)
			{
				float mult = 10.0f * k_PlayerColliderRadius / dist;
				glm::vec3 pushForce = glm::normalize(toP2) * k_CollisionPushForce * mult * Time::deltaTime;
				p2Trans->Translate(pushForce);
				p1Trans->Translate(-pushForce);
			}
		}
	}
}

void ServerGame::HandleProjectileCollisons()
{
	auto projectiles = m_ProjectileManager.GetProjectiles();
	Transform* playerTrans, * projectileTrans;

	
	for (auto pr = projectiles.begin(); pr != projectiles.end(); ++pr)
	{
		projectileTrans = (*pr)->GetTransform();
		for (auto pl = m_Players.begin(); pl != m_Players.end(); ++pl)
		{
			if (!(*pl)->IsActive()) continue;

			if ((*pr)->GetOwner() == (*pl)) continue;

			playerTrans = (*pl)->GetTransform();

			glm::vec3 toProjectile = projectileTrans->GetPosition() - playerTrans->GetPosition();
			float dist = glm::length(toProjectile);

			if (dist < k_MissileColliderRadius + k_PlayerColliderRadius)
			{
				//Debug::LogError("MISSILE HIT");
				(*pr)->ProjectileHit();
				(*pl)->TakeDamage((*pr)->GetProjectileDamage());
				break; // this missile should not hit anything anymore
			}
		}
	}
}

void ServerGame::RespawnPlayers()
{
	// Check if players should respawn
	for (auto it = m_Respawns.begin(); it != m_Respawns.end(); /**/)
	{
		if (it->second < Time::GetTime())
		{
			glm::vec2 pos = glm::circularRand<float>(250.0f);
			it->first->Reset(glm::vec3(pos.x, pos.y, 0.0f));
			// Send RPC
			NetworkManagerServer::Instance().RPC().SendSetNGOJBActive(it->first->GetNetworkID(), true);

			it = m_Respawns.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Check if there are players who died this frame
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it)
	{
		ServerPlayer* p = (*it);
		if (p->IsDead() && m_Respawns.find(p) == m_Respawns.end())
		{
			m_Respawns.emplace(p, Time::GetTime() + k_RespawnTime);

			// Send RPC
			NetworkManagerServer::Instance().RPC().SendSetNGOJBActive(p->GetNetworkID(), false);
			//Debug::Log("Player should respawn at " + std::to_string(Time::GetTime() + k_RespawnTime));
		}
	}
}
