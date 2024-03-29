#pragma once

#include "Networking/Sockets.h"
#include "Networking/MemoryBitStream.h"

#include "DatabaseManager.h"
#include "ClientConnection.h"

#include "Types.h"

#include <vector>

constexpr UInt16 SERVER_PORT_TCP = 45000;

class LobbyManagerServer
{
public:

	LobbyManagerServer() { }
	~LobbyManagerServer() { SocketUtil::CleanUp(); }
	LobbyManagerServer(const LobbyManagerServer& other) = delete;
	LobbyManagerServer& operator=(const LobbyManagerServer& ohter) = delete;

	void Start();

private:

	void ProcessPacket(InputMemoryBitStream& packet, ClientConnectionPtr client);

	void ProcessLoginData(InputMemoryBitStream& packet, ClientConnectionPtr client);
	void ProcessNewUser(InputMemoryBitStream& packet, ClientConnectionPtr client);
	void ProcessClientRequest(InputMemoryBitStream& packet, ClientConnectionPtr client);
	void ProcessMatchDataRequest(InputMemoryBitStream& packet, ClientConnectionPtr client);

	void SendPacketToAllClients(OutputMemoryBitStream& packet);

	void SendNewUserData(ClientConnectionPtr newClient);
	void SendUserData(User* u);

	void ClientDisconnected(UInt32 userID);

	DatabaseManager m_DatabaseManager;

	TCPSocketPtr m_ListenSocket;

	const UInt32 k_MaxClients = 4;
	std::vector<ClientConnectionPtr> m_ClientConnections;

	Byte m_RecieveBuffer[1500];

	const UInt32 k_MaxUsernamePWLength = 50;
	const UInt32 k_MinUsernamePWLength = 3U;
};

