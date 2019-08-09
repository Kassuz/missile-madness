#include "ClientGame.h"

#include "Engine.h"

#include "NetworkManagerClient.h"
#include "Networking/User.h"
#include "Move.h"
#include "ResourceManager.h"

#include "ClientProjectile.h"

#include <GLFW/glfw3.h>

Texture2D* ClientGame::s_PlayerTexture = nullptr;
Texture2D* ClientGame::s_MissileTexture = nullptr;
std::vector<ClientPlayer*> ClientGame::s_Players = std::vector<ClientPlayer*>();

ClientGame::ClientGame()
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
	for (auto p : s_Players)
	{
		p->Update();
	}

	// Show fps
	m_FrameTimes[m_Currentframe++ % 50] = Time::deltaTime;
	if (m_Currentframe > 50)
	{
		float sum = 0;
		for (int i = 0; i < 50; ++i)
			sum += m_FrameTimes[i];
		sum = 50.0f / sum;
		int fps = (int)sum;
		TextRenderer::Instance().RenderText(std::to_string(fps), 10.0f, 10.0f, 1.0f, 0.0f, Color::Cyan());
	}

	int rtt = NetworkManagerClient::Instance().GetRTT();
	TextRenderer::Instance().RenderText("RTT: " + std::to_string(rtt) + "ms", 600.0f, 10.0f, 0.75f, 0.0f, Color::Cyan());
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
