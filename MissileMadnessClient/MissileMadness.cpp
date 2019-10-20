#include <iostream>

#include "CommandLineArgs.h"
#include "LobbyManagerClient.h"

int main(int argc, char* argv[])
{
	// cool logo
	std::cout << "  /\\/\\ (_)___ ___(_) | ___    /\\/\\   __ _  __| |_ __   ___  ___ ___ " << std::endl;
	std::cout << " /    \\| / __/ __| | |/ _ \\  /    \\ / _` |/ _` | '_ \\ / _ \\/ __/ __|" << std::endl;
	std::cout << "/ /\\/\\ \\ \\__ \\__ \\ | |  __/ / /\\/\\ \\ (_| | (_| | | | |  __/\\__ \\__ \\" << std::endl;
	std::cout << "\\/    \\/_|___/___/_|_|\\___| \\/    \\/\\__,_|\\__,_|_| |_|\\___||___/___/" << std::endl;
	std::cout << "\n" << std::endl;

	if (!CommandLineArgs::ProcessCommandLineArgs(argc, argv))
	{
		Debug::LogError("Failed to read command line arguments!");
		return 1;
	}

	// Start Lobby
	LobbyManagerClient lobbyManager;
	lobbyManager.Start();

	return 0;
}
