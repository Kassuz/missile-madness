#include <iostream>

#include "Engine.h"

#include "NetworkManagerServer.h"
#include "LobbyManagerServer.h"
#include "ServerGame.h"

#include "Networking/TestObj.h"

int main(int argc, char* argv[])
{
	std::cout << "Server started" << std::endl;

	LobbyManagerServer lobbyManager;
	lobbyManager.Start();

	return 0;

	NetworkManagerServer::Instance().Initialize();

	// Set server tick rate
	Time::SetFixedTimeStep(60U);
	
	// Player register loop
	while (true)
	{
		Time::Update();

		NetworkManagerServer::Instance().ProcessIncomingPackets();
		NetworkManagerServer::Instance().UpdateSendingPackets();

		if (NetworkManagerServer::Instance().IsGameStarting())
			break;
	}


	ServerGame game(800, 600, NetworkManagerServer::Instance().GetAllUsers());
	NetworkManagerServer::Instance().StartGame();

	// Game Loop
	while (true)
	{
		Time::Update();

		NetworkManagerServer::Instance().ProcessIncomingPackets();

		game.Update();

		NetworkManagerServer::Instance().UpdateSendingPackets();

		NetworkManagerServer::Instance().CheckForDisconnectedUsers();

		if (NetworkManagerServer::Instance().UserCount() == 0)
		{
			Debug::LogError("All Users have disconnected. End game!");
			break;
		}
	}

	// Do winsock cleanup
	SocketUtil::CleanUp();

	return 0;
}