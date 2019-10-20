#include "ClientGame.h"

#include "Engine.h"

#include "NetworkManagerClient.h"
#include "Networking/User.h"
#include "Move.h"
#include "ResourceManager.h"

#include "ClientProjectile.h"
#include "CommandLineArgs.h"

#include "InputManager.h"

#include <GLFW/glfw3.h>

Texture2D* ClientGame::s_PlayerTexture = nullptr;
Texture2D* ClientGame::s_MissileTexture = nullptr;
std::vector<ClientPlayer*> ClientGame::s_Players = std::vector<ClientPlayer*>();
std::vector<ClientProjectile*> ClientGame::s_Projectiles = std::vector<ClientProjectile*>();

ClientGame::ClientGame() : m_AvgFrameTime(50)
{
	s_PlayerTexture  = ResourceManager::Instance().LoadTexture2D("Resources/Textures/Character.png");
	s_MissileTexture = ResourceManager::Instance().LoadTexture2D("Resources/Textures/Missile.png");
}

ClientGame::~ClientGame()
{
	for (auto player : s_Players)
	{
		NetworkManagerClient::Instance().RemoveGameObject(player->GetNetworkID());
		delete player;
	}
	s_Players.clear();

	for (auto miss : s_Projectiles)
	{
		NetworkManagerClient::Instance().RemoveGameObject(miss->GetNetworkID());
		delete miss;
	}
	s_Projectiles.clear();
}

bool ClientGame::StartGame(WindowPtr window, std::vector<User*> users, std::string& winnerName)
{
	if (!NetworkManagerClient::Instance().Initialize(users))
	{
		Debug::LogError("NetworkManager failed to initialize!");
		return false;
	}

	// Lock client to 60fps
	Time::SetFixedTimeStep(60U);

	// Wait for game to actually start
	while (true)
	{
		Time::Update();

		NetworkManagerClient::Instance().UpdateSendingPackets();
		NetworkManagerClient::Instance().ProcessIncomingPackets();

		if (NetworkManagerClient::Instance().GameShouldStart())
			break;
		else if (NetworkManagerClient::Instance().GetClientState() == NetworkManagerClient::ClientState::DISCONNECTED)
			return false;
	}

	NetworkManagerClient::Instance().StartGame();

	// Main game loop
	while (!window->WindowShouldClose())
	{
		Time::Update();
			
		// Process packets
		NetworkManagerClient::Instance().ProcessIncomingPackets();
		// Check disconnect
		if (NetworkManagerClient::Instance().GetClientState() == NetworkManagerClient::ClientState::DISCONNECTED)
			return false;
		else if (NetworkManagerClient::Instance().GameShouldEnd())
			break;

		// Update Game
		Update();

		// Rendering
		RenderingEngine::Render();

		// Send packets
		NetworkManagerClient::Instance().UpdateSendingPackets();


		// Update inputmanager and poll for events
		InputManager::Update();
		window->PollEvents();
	}

	User* winner = NetworkManagerClient::Instance().GetWinningUser();
	if (winner != nullptr)
		winnerName = winner->GetUsersName();

	NetworkManagerClient::Instance().EndGame();
	Debug::Log("Game Ended!");
	return true;
}

void ClientGame::Update()
{
	// Get move
	bool right = InputManager::GetKey(GLFW_KEY_D) || InputManager::GetKey(GLFW_KEY_RIGHT);
	bool left  = InputManager::GetKey(GLFW_KEY_A) || InputManager::GetKey(GLFW_KEY_LEFT);
	bool up    = InputManager::GetKey(GLFW_KEY_W) || InputManager::GetKey(GLFW_KEY_UP);
	bool down  = InputManager::GetKey(GLFW_KEY_S) || InputManager::GetKey(GLFW_KEY_DOWN);
	bool space = InputManager::GetKeyDown(GLFW_KEY_SPACE);

	Move* m = new Move(right, left, up, down, space, Time::GetTime());
	User::Me->AddNewMove(m);

	// Update all players
	for (auto p : s_Players)
	{
		p->Update();
	}

	// Update projectile interpolation
	for (auto p : s_Projectiles)
		p->Update();

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
		ClientPlayer* p = *it;
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
