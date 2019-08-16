#include "ClientGame.h"

#include "Engine.h"

#include "NetworkManagerClient.h"
#include "Networking/User.h"
#include "Move.h"
#include "ResourceManager.h"

#include "ClientProjectile.h"
#include "CommandLineArgs.h"

#include <GLFW/glfw3.h>

Texture2D* ClientGame::s_PlayerTexture = nullptr;
Texture2D* ClientGame::s_MissileTexture = nullptr;
std::vector<ClientPlayer*> ClientGame::s_Players = std::vector<ClientPlayer*>();
std::vector<ClientProjectile*> ClientGame::s_Projectiles = std::vector<ClientProjectile*>();


//float ClientGame::s_DataIntervall = 0.0f;

ClientGame::ClientGame() : m_AvgFrameTime(50)
{
	s_PlayerTexture  = ResourceManager::Instance().LoadTexture2D("Resources/Textures/Character.png");
	s_MissileTexture = ResourceManager::Instance().LoadTexture2D("Resources/Textures/Missile.png");
}

void ClientGame::Update()
{
	// Get move
	bool right = InputManager::Instance().GetKey(GLFW_KEY_D) || InputManager::Instance().GetKey(GLFW_KEY_RIGHT);
	bool left  = InputManager::Instance().GetKey(GLFW_KEY_A) || InputManager::Instance().GetKey(GLFW_KEY_LEFT);
	bool up    = InputManager::Instance().GetKey(GLFW_KEY_W) || InputManager::Instance().GetKey(GLFW_KEY_UP);
	bool down  = InputManager::Instance().GetKey(GLFW_KEY_S) || InputManager::Instance().GetKey(GLFW_KEY_DOWN);
	bool space = InputManager::Instance().GetKeyDown(GLFW_KEY_SPACE);

	Move* m = new Move(right, left, up, down, space, Time::GetTime());
	//Debug::LogFormat("Create move %u, time: %f", m->GetID(), m->GetTimestamp());
	User::Me->AddNewMove(m);
	//if (space)
	//	Debug::LogWarningFormat("Move no: %u, player shoots!", m->GetID());

	// Update all players
	float interpolateIntervall = NetworkManagerClient::Instance().GetServerSendRate();
	for (auto p : s_Players)
	{
		p->Update(interpolateIntervall);
	}

	// Update projectile interpolation
	for (auto p : s_Projectiles)
		p->Update(interpolateIntervall);

	// Show stats
	if (CommandLineArgs::ShouldShowStats())
	{
		m_AvgFrameTime.AddValue(Time::deltaTime);
		int fps = (int)(1.0f / m_AvgFrameTime.GetAverage());
		TextRenderer::Instance().RenderText(std::to_string(fps), 10.0f, 10.0f, 1.0f, 0.0f, Color::Cyan());

		int rtt = NetworkManagerClient::Instance().GetRTT() * 1000.0f;
		TextRenderer::Instance().RenderText("RTT: " + std::to_string(rtt) + "ms", 600.0f, 10.0f, 0.75f, 0.0f, Color::Cyan());
	
		TextRenderer::Instance().RenderText("Packets Dropped: " + std::to_string(NetworkManagerClient::Instance().GetDroppedPacketCount()), 300.0f, 10.0f, 0.75f, 0.0f, Color::Cyan());
	}
}

NetworkedGameObject* ClientGame::CreatePlayer(UInt32 networkID)
{
	NetworkedGameObject* ngobj = ClientPlayer::CreateInstance(networkID);
	
	ClientPlayer* player = static_cast<ClientPlayer*>(ngobj);
	player->Initialize(s_PlayerTexture);
	s_Players.push_back(player);
	Debug::LogError("Player initialized!");

	return ngobj;
}

NetworkedGameObject* ClientGame::CreateMissile(UInt32 networkID)
{
	NetworkedGameObject* ngobj = ClientProjectile::CreateInstance(networkID);
	ClientProjectile* missile = static_cast<ClientProjectile*>(ngobj);
	s_Projectiles.push_back(missile);

	if (s_MissileTexture != nullptr)
		missile->Initialize(s_MissileTexture);
	else
		Debug::LogError("Missile texture has failed to initialize!");

	return ngobj;
}

void ClientGame::DisconnectPlayer(UInt32 userID)
{
	for (auto it = s_Players.begin(); it != s_Players.end(); ++it)
	{
		Player* p = *it;
		if (p->GetUserID() == userID)
		{
			Debug::LogErrorFormat("User %u disconnected, removed player with id %u", userID, p->GetNetworkID());
			NetworkManagerClient::Instance().RemoveGameObject(p->GetNetworkID());
			delete p;
			s_Players.erase(it);
			return;
		}
	}

	Debug::LogErrorFormat("User %u disconnected, but did not find corresponding player!", userID);
}

void ClientGame::RemoveProjectile(ClientProjectile* p)
{
	for (auto it = s_Projectiles.begin(); it != s_Projectiles.end(); ++it)
	{
		if ((*it) == p)
		{
			s_Projectiles.erase(it);
			return;
		}
	}
}
