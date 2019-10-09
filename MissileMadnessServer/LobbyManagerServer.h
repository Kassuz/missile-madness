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
	~LobbyManagerServer() { }
	LobbyManagerServer(const LobbyManagerServer& other) = delete;
	LobbyManagerServer& operator=(const LobbyManagerServer& ohter) = delete;

	void Start();

private:

	void ProcessPacket(InputMemoryBitStream& packet, ClientConnectionPtr client);
	void SendPacketToAllClients(OutputMemoryBitStream& packet);

	void SendUserData(ClientConnectionPtr newClient);

	void ClientDisconnected(UInt32 userID);

	DatabaseManager m_DatabaseManager;

	TCPSocketPtr m_ListenSocket;

	const UInt32 k_MaxClients = 4;
	std::vector<ClientConnectionPtr> m_ClientConnections;

	Byte m_RecieveBuffer[1500];
};

