#include "NetworkManagerServer.h"

#include "Engine.h"
#include "Networking/User.h"
#include "Networking/SharedRPC.h"

NetworkManagerServer::NetworkManagerServer()
{
}

NetworkManagerServer::~NetworkManagerServer()
{
	for (auto it : m_AddressToUserMap)
	{
		delete it.second;
	}

	m_AddressToUserMap.clear();
	m_UserToAddressMap.clear();
}

NetworkManagerServer& NetworkManagerServer::Instance()
{
	static NetworkManagerServer instance;
	return instance;
}

void NetworkManagerServer::Initialize()
{
	SocketUtil::StaticInit();

	m_ServerSocket = SocketUtil::CreateUDPSocket(INET);

	SocketAddress serverAddress(INADDR_ANY, SERVER_PORT);

	m_ServerSocket->Bind(serverAddress);
	m_ServerSocket->SetNonBlockingMode(true);

	m_ServerState = ServerState::WELCOMING_CLIENTS;
}

void NetworkManagerServer::ProcessIncomingPackets()
{
	for (int i = 0; i < k_MaxPacketsPerFrame; ++i)
	{
		Int32 recievedBytes = m_ServerSocket->ReceiveFrom(m_RecieveBuffer, 1500, m_RecieveAddress);
		if (recievedBytes > 0)
		{
			InputMemoryBitStream packet(m_RecieveBuffer, recievedBytes * 8);
			PacketType type;
			packet.Read(type, GetRequiredBits<PacketType::MAX_PACKET>::Value);

			//Debug::LogFormat("Recieved packet of type %i, from %s", type, m_RecieveAddress.ToString().c_str());
			//packet.HexDump();

			switch (type)
			{
			case HELLO:
				HandleHelloPacket(packet, m_RecieveAddress);
				break;
			case WELCOME:
				Debug::LogError("Server should not recieve WELCOME packets");
				break;
			case GAME_START:
				Debug::LogError("Server should not recieve GAME_START packets");
				break;
			case REPLICATION_DATA: // Actually just user inputs
				HandleReplication(packet, m_RecieveAddress);
				break;
			case MAX_PACKET:
				Debug::LogError("No MAX_PACKET packets should ever be sent!");
				break;
			default:
				Debug::LogError("Unknown packet recieved from " + m_RecieveAddress.ToString());
				break;
			}
		}
		else if (recievedBytes == -WSAECONNRESET)
		{
			// Client has closed -> DC client
			auto it = m_AddressToUserMap.find(m_RecieveAddress);
			if (it != m_AddressToUserMap.end())
				DisconnectUser(it->second, "Connection reset");
		}

	}
}

void NetworkManagerServer::UpdateSendingPackets()
{
	switch (m_ServerState)
	{
	case NetworkManagerServer::ServerState::NOT_INITIALIZED:
		break;
	case NetworkManagerServer::ServerState::WELCOMING_CLIENTS:
		if (m_AddressToUserMap.size() >= k_MaxUsers)
		{
			Debug::Log("Max users joined, start game!");
			m_ServerState = ServerState::STARTING;
			SendGameStartPacket();
		}
		break;
	case NetworkManagerServer::ServerState::STARTING:
		break;
	case NetworkManagerServer::ServerState::SIMULATING:
		SendWorldState();
		break;
	default:
		break;
	}
}

User* NetworkManagerServer::GetUserWithID(UInt32 userID)
{
	for (auto it : m_UserToAddressMap)
	{
		if (it.first->GetUserID() == userID)
			return it.first;
	}
	return nullptr;
}

std::vector<User*> NetworkManagerServer::GetAllUsers()
{
	std::vector<User*> users;
	for (auto it : m_UserToAddressMap)
		users.push_back(it.first);
	return users;
}


void NetworkManagerServer::DisconnectUser(User* userToDisconnect, const char* disconnectReason)
{
	if (userToDisconnect != nullptr)
	{
		Debug::LogErrorFormat("User %s disconnected, reason %s", userToDisconnect->GetUsersName().c_str(), disconnectReason);

		// Send RPC to rest of the clients
		m_RPCManager.RemoveUser(userToDisconnect->GetUserID());
		m_RPCManager.SendRPC(RPCParams::UserDisconnedtRPC::k_RPCName, new RPCParams::UserDisconnedtRPC(userToDisconnect->GetUserID()));

		auto it = m_UserToAddressMap.find(userToDisconnect);
		m_AddressToUserMap.erase(it->second);
		m_UserToAddressMap.erase(userToDisconnect);

		delete userToDisconnect;
	}
}

void NetworkManagerServer::CheckForDisconnectedUsers()
{
	for (auto it : m_UserToAddressMap)
	{
		if (it.first->GetLastRecievedPacketTime() > 0.0 && Time::GetTime() - it.first->GetLastRecievedPacketTime() >= k_DisconnectTime)
		{
			DisconnectUser(it.first, "Did not recieve any packets for 10s.");
			return; // Disconnect one user per check for simplicity
		}
	}
}

void NetworkManagerServer::HandleHelloPacket(InputMemoryBitStream& packet, SocketAddress sender)
{
	if (m_ServerState == ServerState::WELCOMING_CLIENTS)
	{
		if (m_AddressToUserMap.find(sender) == m_AddressToUserMap.end())
		{
			if (m_AddressToUserMap.size() >= k_MaxUsers)
			{
				Debug::LogWarning("Already enough users, ignore this one :(");
				return;
			}
			// Client not yet welcomed
			std::string userName;
			packet.Read(userName);
			User* u = new User(userName, m_NextUserID++);
			m_AddressToUserMap.emplace(sender, u);
			m_UserToAddressMap.emplace(u, sender);
			m_RPCManager.InitUser(u->GetUserID());

			Debug::LogFormat("Welcomed user %s, with id %u!", u->GetUsersName().c_str(), u->GetUserID());
		}
		else
		{
			Debug::LogWarningFormat("%s already welcomed, ignore packet and resend welcome", sender.ToString().c_str());
		}

		// Send welcome packet even if user already welcomed, packet might not have been recieved
		SendWelcomePacket(sender);
	}
	else
	{
		Debug::LogWarning("Server can't welcome clients!");
	}
}

void NetworkManagerServer::HandleReplication(InputMemoryBitStream& packet, SocketAddress sender)
{
	if (m_ServerState == ServerState::SIMULATING)
	{
		UInt32 userID;
		packet.Read(userID);

		auto it = m_AddressToUserMap.find(sender);
		if (it != m_AddressToUserMap.end())
		{
			User* u = it->second;
			if (u->GetUserID() != userID) // Check just in if case something has gone wrong
			{
				Debug::LogErrorFormat("UserID from packet %u is different from user id assigned for %s, ID: %u", userID, u->GetUserID(), sender.ToString().c_str());
				return;
			}

			u->ReadMoves(packet);

			UInt32 lastRPC;
			packet.Read(lastRPC);
			m_RPCManager.ACKRPCs(userID, lastRPC);

			// Set last recieved time
			u->SetLastRecievedPacketTime(Time::GetTime());
		}

	}
	else
	{
		Debug::LogWarningFormat("Recieved replication data from %s, even though not yet simulating!", sender.ToString().c_str());
	}
}

void NetworkManagerServer::SendWelcomePacket(SocketAddress dest)
{
	auto it = m_AddressToUserMap.find(dest);
	if (it != m_AddressToUserMap.end() && it->second != nullptr)
	{
		OutputMemoryBitStream welcomePacket;
		welcomePacket.Write(PacketType::WELCOME, GetRequiredBits<PacketType::MAX_PACKET>::Value);
		welcomePacket.Write(it->second->GetUserID());

		m_ServerSocket->SendTo(welcomePacket.GetBufferPtr(), welcomePacket.GetByteLength(), dest);
	}
	else
	{
		Debug::LogError("Tried to send welcome-packet, but couldt find user!");
	}
}

void NetworkManagerServer::SendGameStartPacket()
{
	OutputMemoryBitStream packet;
	packet.Write(PacketType::GAME_START, GetRequiredBits<PacketType::MAX_PACKET>::Value);
	packet.Write(m_AddressToUserMap.size(), 32);
	for (auto it : m_AddressToUserMap)
	{
		User* u = it.second;
		packet.Write(u->GetUsersName());
		packet.Write(u->GetUserID());
	}

	// Send to all clients
	SendPacketToAllClients(packet, PacketType::GAME_START);
}

void NetworkManagerServer::SendWorldState()
{
	if (Time::GetTime() < m_NextReplicationTime)
		return;

	m_NextReplicationTime = Time::GetTime() + k_ReplicationSendIntervall;

	OutputMemoryBitStream packet;
	packet.Write(PacketType::REPLICATION_DATA, GetRequiredBits<PacketType::MAX_PACKET>::Value);

	// Write packet id
	packet.Write(m_NextPacketID++, 32);

	// Write all networked gameobjects
	packet.Write(m_NetworkIDTOGameObjMap.size(), 32);
	for (auto it : m_NetworkIDTOGameObjMap)
	{
		it.second->Write(packet);
	}

	SendPacketToAllClients(packet, PacketType::REPLICATION_DATA);
}

void NetworkManagerServer::SendPacketToAllClients(OutputMemoryBitStream& packet, PacketType packetType)
{
	for (auto it : m_AddressToUserMap)
	{
		// Create copy of packet for client
		OutputMemoryBitStream clientPacket(packet);

		// ACKs and RPC present only when replication data is being sent
		if (packetType == PacketType::REPLICATION_DATA)
		{
			// Write pending rpcs
			m_RPCManager.WritePendingRPCs(it.second->GetUserID(), clientPacket);
			// Write acks
			WriteACKs(clientPacket, it.second);
		}

		m_ServerSocket->SendTo(clientPacket.GetBufferPtr(), clientPacket.GetByteLength(), it.first);
	}
}

void NetworkManagerServer::WriteACKs(OutputMemoryBitStream& packet, User* user)
{
	// Write last processed move for user, so client can update movelist accordingly
	packet.Write(user->GetLastProcessedMoveTimestamp());
}
