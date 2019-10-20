#include "ServerGame.h"

#include "Engine.h"

#include "ServerPlayer.h"
#include "Networking/User.h"
#include "ServerProjectile.h"

#include <glm/gtc/random.hpp>

ServerGame::ServerGame(UInt32 screenWidth, UInt32 screenHeight)
{
	glm::vec2 bLeft(-(float)screenWidth / 2.0f, -(float)screenHeight / 2.0f);
	glm::vec2 tLeft(-(float)screenWidth / 2.0f,  (float)screenHeight / 2.0f);
	glm::vec2 tRight((float)screenWidth / 2.0f,  (float)screenHeight / 2.0f);
	glm::vec2 bRight((float)screenWidth / 2.0f, -(float)screenHeight / 2.0f);

	m_Walls[0] = Wall(tLeft,  tRight, glm::vec2( 0.0f, -1.0f));
	m_Walls[1] = Wall(bRight, tRight, glm::vec2(-1.0f,  0.0f));
	m_Walls[2] = Wall(bLeft,  bRight, glm::vec2( 0.0f,  1.0f));
	m_Walls[3] = Wall(bLeft,  tLeft,  glm::vec2( 1.0f,  0.0f));
}


ServerGame::~ServerGame()
{
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it)
	{
		m_NetworkManager.RemoveGameObject((*it)->GetNetworkID());
		delete (*it);
	}
}

bool ServerGame::StartGame(std::vector<ClientConnectionPtr> clients)
{
	m_Clients = clients;

	if (!m_NetworkManager.Initialize(clients))
	{
		Debug::LogError("Initializing NetworkManager failed!");
		return false;
	}

	m_ProjectileManager.Init(&m_NetworkManager);

	// Set server tick rate
	Time::SetFixedTimeStep(60U);
	
	// Establishing udp connections
	while (true)
	{
		Time::Update();

		m_NetworkManager.ProcessIncomingPackets();
		m_NetworkManager.UpdateSendingPackets();

		if (m_NetworkManager.IsGameStarting())
			break;
	}

	// Create players
	UInt32 playerCount = clients.size();
	for (int i = 0; i < playerCount; ++i)
	{
		ServerPlayer* p = m_NetworkManager.CreateNetworkedGameObject<ServerPlayer>();
		p->Initialize(clients[i]->GetUser(), this);

		float posX = std::cos((i / (float)playerCount) * glm::two_pi<float>());
		float posY = std::sin((i / (float)playerCount) * glm::two_pi<float>());

		p->GetTransform()->SetPosition(glm::vec3(posX, posY, 0.0f) * 250.0f);

		m_Players.push_back(p);
		m_GameStats.emplace(clients[i]->GetUser()->GetUserID(), new GameStats);
	}

	m_NetworkManager.StartGame();

	UInt32 winner = 0U;
	// Game Loop
	while (true)
	{
		Time::Update();

		m_NetworkManager.ProcessIncomingPackets();

		Update();

		m_NetworkManager.UpdateSendingPackets();

		m_NetworkManager.CheckForDisconnectedUsers();

		if (m_NetworkManager.UserCount() <= 1)
		{
			Debug::LogError("One or less users remaining. End game!");
			auto users = m_NetworkManager.GetAllUsers();
			if (users.size() == 1)
				winner = users[0]->GetUserID();
			break;
		}
		else if (GameShouldEnd(winner))
		{
			Debug::LogWarningFormat("Game ended user %u reached the match kill limit!", winner);
			break;
		}
	}

	m_NetworkManager.EndGame(winner);

	while (!m_NetworkManager.IsGameFinished())
	{
		Time::Update();
		m_NetworkManager.ProcessIncomingPackets();
		m_NetworkManager.UpdateSendingPackets();
		m_NetworkManager.CheckForDisconnectedUsers();
	}

	Debug::Log("Game ended!");
	return true;
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
	for (auto conn : m_Clients)
	{
		if (conn->GetConnectionStatus() == ConnectionStatus::DISCONNECTED)
		{
			for (auto player : m_Players)
			{
				if (player->GetUserID() == conn->GetUser()->GetUserID())
				{
					m_Disconnected.push(player);
					GetStatsForUserID(player->GetUserID())->m_IsDisconnected = true;
					break;
				}
			}
		}
	}
	while (m_Disconnected.size() > 0)
	{
		ServerPlayer* p = m_Disconnected.front();
		m_Disconnected.pop();
		
		for (auto it = m_Players.begin(); it != m_Players.end(); ++it)
		{
			if ((*it) == p)
			{
				m_NetworkManager.RemoveGameObject(p->GetNetworkID());
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

GameStats* ServerGame::GetStatsForUserID(UInt32 userID)
{
	auto it = m_GameStats.find(userID);
	if (it != m_GameStats.end())
		return it->second;
	else
		return nullptr;
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
			if (dist < k_PlayerColliderRadius || glm::dot(toPlayer, m_Walls[i].GetNormal()) < 0.0f)
			{
				glm::vec2 pos = pointOnWall + glm::normalize(toPlayer) * k_PlayerColliderRadius;
				playerTransform->SetPosition(glm::vec3(pos, 0.0f));
			}
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

			// Missile hit player
			if (dist < k_MissileColliderRadius + k_PlayerColliderRadius)
			{
				(*pr)->ProjectileHit();
				if ((*pl)->TakeDamage((*pr)->GetProjectileDamage()))
				{
					// Player died increase stats
					UInt32 killerID = (*pr)->GetOwner()->GetUserID();
					UInt32 deadID = (*pl)->GetUserID();

					auto killerStats = GetStatsForUserID(killerID);
					if (killerStats != nullptr) killerStats->m_Kills++;
					else Debug::LogErrorFormat("Can't find stats for user %u", killerID);
					
					auto deadStats = GetStatsForUserID(deadID);
					if (deadStats != nullptr) deadStats->m_Deaths++;
					else Debug::LogErrorFormat("Can't find stats for user %u", deadID);
				}

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
		if (it->second < Time::GetRealTime())
		{
			glm::vec2 pos = glm::circularRand<float>(250.0f);
			it->first->Reset(glm::vec3(pos.x, pos.y, 0.0f));
			// Send RPC
			m_NetworkManager.RPC().SendSetNGOJBActive(it->first->GetNetworkID(), true);

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
			m_Respawns.emplace(p, Time::GetRealTime() + k_RespawnTime);

			// Send RPC
			m_NetworkManager.RPC().SendSetNGOJBActive(p->GetNetworkID(), false);
			//Debug::Log("Player should respawn at " + std::to_string(Time::GetTime() + k_RespawnTime));
		}
	}
}

bool ServerGame::GameShouldEnd(UInt32& winner)
{
	for (auto it : m_GameStats)
	{
		if (it.second->m_IsDisconnected) continue;

		if (it.second->m_Kills >= k_KillLimit)
		{
			// We have a winner!!
			winner = it.first;
			return true;
		}
	}

	return false;
}
