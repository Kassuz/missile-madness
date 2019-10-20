#pragma once

#include "Networking/Sockets.h"
#include "Networking/MemoryBitStream.h"
#include "Networking/PacketTypes.h"

#include "Types.h"
#include "LobbyMenu.h"

#include <unordered_map>

class User;

constexpr auto SERVER_ADDRESS_TCP = "127.0.0.1:45000";

class LobbyManagerClient
{
public:
	LobbyManagerClient() : m_LobbyMenu(this) { }
	~LobbyManagerClient() { SocketUtil::CleanUp(); }
	LobbyManagerClient(const LobbyManagerClient& other) = delete;
	LobbyManagerClient& operator=(const LobbyManagerClient& ohter) = delete;

	void Start();

	void SetReady();
	void ChangeUserColor(Color newColor);
	void RequestStats(MatchDataFormat type);

private:
	enum class LobbyStatus { LOGIN_MENU, LOGGED_IN, EXIT };
	LobbyStatus m_LobbyStatus = LobbyStatus::LOGIN_MENU;

	bool m_WaitingForResponse = false;

	Byte m_RecieveBuffer[1500];

	TCPSocketPtr m_ClientSocket;

	std::unordered_map<UInt32, User*> m_Users;

	const float k_ReconnectIntervall = 5.0f;

	LobbyMenu m_LobbyMenu;

	bool m_GameShouldStart = false;

	void ProcessPackets();

	void ProcessUserData(InputMemoryBitStream& packet);
	void ProcessServerResponse(InputMemoryBitStream& packet);
	void ProcessUserDisconnect(InputMemoryBitStream& packet);
	void ProcessMatchData(InputMemoryBitStream& packet);

	void LoginMenu();
	bool GetUsernameAndPassword(std::string& username, std::string& password, bool retypePassword = false);
};

