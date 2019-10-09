#include "LobbyManagerServer.h"

#include "Debug.h"
#include "Networking/PacketTypes.h"

void LobbyManagerServer::Start()
{
	// Init
	//--------------------------------------------------------------
	if (!m_DatabaseManager.Init())
	{
		Debug::LogError("DatabaseManager::Init faied!");
		return;
	}

	if (!SocketUtil::StaticInit())
	{
		Debug::LogError("Couldn't start Winsock!");
		return;
	}

	m_ListenSocket = SocketUtil::CreateTCPSocket(INET);
	if (m_ListenSocket == nullptr)
	{
		Debug::LogError("Creating listen sokcet failed");
		return;
	}

	SocketAddress serverAddress(INADDR_ANY, SERVER_PORT_TCP);

	if (m_ListenSocket->Bind(serverAddress) != NO_ERROR)
	{
		Debug::LogError("Binding server sokcet failed");
		return;
	}

	if (m_ListenSocket->SetNonBlockingMode(true) != NO_ERROR)
	{
		Debug::LogError("Setting listen socket non blocking failed");
		return;
	}

	if (m_ListenSocket->Listen(SOMAXCONN) != NO_ERROR)
	{
		Debug::LogError("Setting socket to listen failed");
		return;
	}

	// Main loop
	//--------------------------------------------------------------
	while (true)
	{
		// Accept new clients
		//--------------------------------------------------------------
		if (m_ClientConnections.size() < k_MaxClients)
		{
			SocketAddress clientAddress;
			TCPSocketPtr newClient = m_ListenSocket->Accept(clientAddress);

			if (newClient != nullptr)
			{
				Debug::LogFormat("Accept new client from %s", clientAddress.ToString().c_str());
				m_ClientConnections.emplace_back(new ClientConnection(SocketAddressPtr(new SocketAddress(clientAddress)), newClient));
			}
		}

		// Process packets from connected clients
		//--------------------------------------------------------------
		for (auto it = m_ClientConnections.begin(); it != m_ClientConnections.end(); ++it)
		{
			ClientConnectionPtr conn = *it;

			Int32 recvBytes = conn->GetTCPSocket()->Receive(m_RecieveBuffer, sizeof(m_RecieveBuffer));
			if (recvBytes > 0) // Got data
			{
				InputMemoryBitStream packet(m_RecieveBuffer, recvBytes * 8);
				ProcessPacket(packet, conn);
			}
			else if (recvBytes == 0 || recvBytes == -WSAECONNRESET) // RecvBytes == 0 -> FIN packet sent
			{
				Debug::LogErrorFormat("Client socket closed from: %s", conn->GetTCPAddress()->ToString().c_str());
				m_ClientConnections.erase(it);
				
				// Inform other clients
				if (conn->GetUser() != nullptr)
					ClientDisconnected(conn->GetUser()->GetUserID());
				break;
			}
			else if (recvBytes < 0) // Error
			{
				if (recvBytes != -WSAEWOULDBLOCK) // <- Non error, happens with non-blocking sockets
					Debug::LogErrorFormat("Error recieving data from: %s", conn->GetTCPAddress()->ToString().c_str());
			}
		}

		// If all clients are ready, start the game
		//--------------------------------------------------------------
		// Game.Start()
	}
}

void LobbyManagerServer::ProcessPacket(InputMemoryBitStream& packet, ClientConnectionPtr client)
{
	LobbyPacketType type;
	packet.Read(type, GetRequiredBits<LPT_MAX_PACKET>::Value);

	switch (type)
	{
	case LPT_LOGIN_DATA:
	{
		// Already logged in? -> Drop packet
		if (client->GetConnectionStatus() != ConnectionStatus::NOT_LOGGED_IN)
			return;
		
		std::string username, password;
		packet.Read(username);
		packet.Read(password);

		// Create response packet
		OutputMemoryBitStream response;
		response.Write(LPT_SERVER_RESPONSE, GetRequiredBits<LPT_MAX_PACKET>::Value);

		// Check that not already logged in
		bool loggedIn = false;
		for (auto conn : m_ClientConnections)
		{
			if (conn->GetUser() != nullptr && conn->GetUser()->GetUsersName() == username)
			{
				loggedIn = true;
				break;
			}
		}

		UInt32 userID;
		bool success = !loggedIn && m_DatabaseManager.Login(username, password, userID);

		// Login success!
		if (success)
		{
			Debug::LogWarningFormat("Successfull login from %s as %s", client->GetTCPAddress()->ToString().c_str(), username.c_str());

			client->LogIn(username, userID);
			
			response.Write(SR_LOGIN_OK, GetRequiredBits<SR_MAX_RESPONSE>::Value);
			response.Write(username);
			response.Write(userID);
		}
		// Login fail
		else
		{
			response.Write(SR_LOGIN_WRONG, GetRequiredBits<SR_MAX_RESPONSE>::Value);
		}

		// Send response
		client->GetTCPSocket()->Send(response.GetBufferPtr(), response.GetByteLength());

		// Send user info
		if (success)
			SendUserData(client);
		
		break;
	}
	case LPT_CREATE_USER:
	{
		// Already logged in? -> Drop packet
		if (client->GetConnectionStatus() != ConnectionStatus::NOT_LOGGED_IN)
			return;

		std::string username, password;
		packet.Read(username);
		packet.Read(password);

		OutputMemoryBitStream response;
		response.Write(LPT_SERVER_RESPONSE, GetRequiredBits<LPT_MAX_PACKET>::Value);


		if (m_DatabaseManager.UserExists(username))
		{
			response.Write(SR_USER_ALREADY_EXISTS, GetRequiredBits<SR_MAX_RESPONSE>::Value);
		}
		else
		{
			m_DatabaseManager.AddNewUser(username, password);
			response.Write(SR_USER_CREATE_SUCCESS, GetRequiredBits<SR_MAX_RESPONSE>::Value);

			// Test print
			m_DatabaseManager.PrintUsers();
		}

		client->GetTCPSocket()->Send(response.GetBufferPtr(), response.GetByteLength());

		break;
	}
	case LPT_CLIENT_REQUEST:
	{
		// TODO
		break;
	}
	case LPT_SERVER_RESPONSE:
		Debug::LogError("Server should not recieve LPT_SERVER_RESPONSE packets");
		break;
	case LPT_MAX_PACKET:
		Debug::LogError("Server should not recieve LPT_MAX_PACKET packets");
		break;
	default:
		break;
	}
}

void LobbyManagerServer::SendPacketToAllClients(OutputMemoryBitStream& packet)
{
	for (auto conn : m_ClientConnections)
		conn->GetTCPSocket()->Send(packet.GetBufferPtr(), packet.GetByteLength());
}

// Inform existing clients of new client and new client of other clients
void LobbyManagerServer::SendUserData(ClientConnectionPtr newClient)
{
	// Packet structure
	// Type: LPT_USER_DATA
	// UserCount: UInt32
	// For UserCount:
	//		UserName: string
	//		UserID: UInt32
	//		CharacterColor: float * 4

	OutputMemoryBitStream newUserPacket, allUsersPacket;

	newUserPacket.Write(LPT_USER_DATA, GetRequiredBits<LPT_MAX_PACKET>::Value);
	newUserPacket.Write(1U, 32);
	newUserPacket.Write(newClient->GetUser()->GetUsersName());
	newUserPacket.Write(newClient->GetUser()->GetUserID());
	Color userColor = newClient->GetUser()->GetCharacterColor();
	newUserPacket.Write(static_cast<glm::vec3>(userColor));

	std::vector<User*> otherUsers;
	for (auto conn : m_ClientConnections)
	{
		if (conn == newClient || conn->GetUser() == nullptr) continue;
		
		conn->GetTCPSocket()->Send(newUserPacket.GetBufferPtr(), newUserPacket.GetByteLength());
		otherUsers.push_back(conn->GetUser());
	}

	if (otherUsers.size() > 0)
	{
		allUsersPacket.Write(LPT_USER_DATA, GetRequiredBits<LPT_MAX_PACKET>::Value);
		allUsersPacket.Write(otherUsers.size(), 32);

		for (auto u : otherUsers)
		{
			allUsersPacket.Write(u->GetUsersName());
			allUsersPacket.Write(u->GetUserID());
			userColor = u->GetCharacterColor();
			allUsersPacket.Write(static_cast<glm::vec3>(userColor));
		}

		newClient->GetTCPSocket()->Send(allUsersPacket.GetBufferPtr(), allUsersPacket.GetByteLength());
	}
}

void LobbyManagerServer::ClientDisconnected(UInt32 userID)
{
	OutputMemoryBitStream connLostPacket;
	connLostPacket.Write(LPT_USER_DISCONNECT, GetRequiredBits<LPT_MAX_PACKET>::Value);
	connLostPacket.Write(userID);
	SendPacketToAllClients(connLostPacket);
}
