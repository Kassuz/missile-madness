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

//NetworkManagerServer& NetworkManagerServer::Instance()
//{
//	static NetworkManagerServer instance;
//	return instance;
//}

bool NetworkManagerServer::Initialize(std::vector<ClientConnectionPtr> clients)
{
	m_ServerSocket = SocketUtil::CreateUDPSocket(INET);
	if (m_ServerSocket == nullptr)
	{
		Debug::LogError("Creating Server Socket failed");
		return false;
	}

	SocketAddress serverAddress(INADDR_ANY, SERVER_PORT_UDP);

	if (m_ServerSocket->Bind(serverAddress) != NO_ERROR)
	{
		Debug::LogErrorFormat("Binding server socket to %s failed", serverAddress.ToString().c_str());
		return false;
	}
	if (m_ServerSocket->SetNonBlockingMode(true) != NO_ERROR)
	{
		Debug::LogError("Setting sever socket to non blocking mode failed");
		return false;
	}

	m_ServerState = ServerState::WELCOMING_CLIENTS;

	m_Clients = clients;

	return true;
}

void NetworkManagerServer::ProcessIncomingPackets()
{
	for (int i = 0; i < k_MaxPacketsPerFrame; ++i)
	{
		Int32 recievedBytes = m_ServerSocket->ReceiveFrom(m_RecieveBuffer, 1500, m_RecieveAddress);
		if (recievedBytes > 0)
		{
			InputMemoryBitStream packet(m_RecieveBuffer, recievedBytes * 8);
			GameplayPacketType type;
			packet.Read(type, GetRequiredBits<GameplayPacketType::GPT_MAX_PACKET>::Value);

			//Debug::LogFormat("Recieved packet of type %i, from %s", type, m_RecieveAddress.ToString().c_str());
			//packet.HexDump();

			switch (type)
			{
			case GPT_HELLO:
				HandleHelloPacket(packet, m_RecieveAddress);
				break;
			case GPT_WELCOME:
				Debug::LogError("Server should not recieve WELCOME packets");
				break;
			case GPT_GAME_START:
				Debug::LogError("Server should not recieve GAME_START packets");
				break;
			case GPT_REPLICATION_DATA: // Actually just user inputs
				HandleReplication(packet, m_RecieveAddress);
				break;
			case GPT_GAME_ENDED:
				Debug::LogError("No GAME_ENDED packets should ever be sent!");
				break;
			case GPT_MAX_PACKET:
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
			if (m_ServerState == ServerState::ENDING)
			{
				HandleGameEnded(m_RecieveAddress);
			}
			else
			{
				auto it = m_AddressToUserMap.find(m_RecieveAddress);
				if (it != m_AddressToUserMap.end())
					DisconnectUser(it->second, "Connection reset");
			}
		}

	}
}

void NetworkManagerServer::UpdateSendingPackets()
{
	switch (m_ServerState)
	{
	case ServerState::NOT_INITIALIZED:
		break;
	case ServerState::WELCOMING_CLIENTS:
		if (m_AddressToUserMap.size() == m_Clients.size())
		{
			//Debug::Log("Max users joined, start game!");s
			m_ServerState = ServerState::STARTING;
			//SendGameStartPacket();
		}
		break;
	case ServerState::STARTING:
		break;
	case ServerState::SIMULATING:
		SendWorldState();
		break;
	case ServerState::ENDING:
		SendGameEndedPacket();
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

		// Don't delete user yet, just set as disconnected
		for (auto conn : m_Clients)
		{
			if (conn->GetUser() == userToDisconnect)
			{
				conn->SetConnectionStatus(ConnectionStatus::DISCONNECTED);
				break;
			}
		}

		//delete userToDisconnect;
	}
}

void NetworkManagerServer::CheckForDisconnectedUsers()
{
	for (auto it : m_UserToAddressMap)
	{
		if (it.first->GetLastRecievedPacketTime() > 0.0 && Time::GetRealTime() - it.first->GetLastRecievedPacketTime() >= k_DisconnectTime)
		{
			if (m_ServerState == ServerState::ENDING)
				HandleGameEnded(it.second);
			else
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
			// Client not yet welcomed
			UInt32 userID; packet.Read(userID);

			User* u = nullptr;
			for (auto conn : m_Clients)
			{
				if (conn->GetUser()->GetUserID() == userID)
				{
					u = conn->GetUser();
					break;
				}
			}

			if (u != nullptr)
			{
				m_AddressToUserMap.emplace(sender, u);
				m_UserToAddressMap.emplace(u, sender);
				m_RPCManager.InitUser(u->GetUserID());
				u->SetLastRecievedPacketTime(0.0f);
				Debug::LogFormat("User %s, with id %u, UDP connection working", u->GetUsersName().c_str(), u->GetUserID());
			}
			else
			{
				Debug::LogErrorFormat("Don't have client connection with userID:%u", userID);
				return;
			}
		}
		else
		{
			Debug::LogWarningFormat("%s already welcomed, ignore packet and resend welcome", sender.ToString().c_str());
		}

		// Send welcome packet even if user already welcomed, packet might not have been recieved
		OutputMemoryBitStream welcomePacket;
		welcomePacket.Write(GameplayPacketType::GPT_WELCOME, GetRequiredBits<GameplayPacketType::GPT_MAX_PACKET>::Value);
		m_ServerSocket->SendTo(welcomePacket.GetBufferPtr(), welcomePacket.GetByteLength(), sender);
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
			u->SetLastRecievedPacketTime(Time::GetRealTime());
		}

	}
	else
	{
		Debug::LogWarningFormat("Recieved replication data from %s, even though not yet simulating!", sender.ToString().c_str());
	}
}

void NetworkManagerServer::HandleGameEnded(SocketAddress sender)
{
	auto it = m_AddressToUserMap.find(sender);
	if (it != m_AddressToUserMap.end())
	{
		// Can disconnect user
		Debug::LogWarningFormat("User %s game ended successfully!", it->second->GetUsersName().c_str());
		m_UserToAddressMap.erase(it->second);
		m_AddressToUserMap.erase(it);

		if (UserCount() == 0)
			m_ServerState = ServerState::FINISHED;
	}
}



void NetworkManagerServer::SendGameStartPacket()
{
	OutputMemoryBitStream packet;
	packet.Write(GameplayPacketType::GPT_GAME_START, GetRequiredBits<GameplayPacketType::GPT_MAX_PACKET>::Value);
	packet.Write(m_AddressToUserMap.size(), 32);
	for (auto it : m_AddressToUserMap)
	{
		User* u = it.second;
		packet.Write(u->GetUsersName());
		packet.Write(u->GetUserID());
	}

	// Write replication packet send intervall, so clients can interpolate
	//packet.Write(k_ReplicationSendIntervall);

	// Send to all clients
	SendPacketToAllClients(packet, GameplayPacketType::GPT_GAME_START);
}

void NetworkManagerServer::SendWorldState()
{
	if (Time::GetTime() < m_NextReplicationTime)
		return;

	m_NextReplicationTime = Time::GetTime() + k_ReplicationSendIntervall;

	OutputMemoryBitStream packet;
	packet.Write(GameplayPacketType::GPT_REPLICATION_DATA, GetRequiredBits<GameplayPacketType::GPT_MAX_PACKET>::Value);

	// Write packet id
	packet.Write(m_NextPacketID++, 32);

	// Write all networked gameobjects
	packet.Write(m_NetworkIDTOGameObjMap.size(), 32);
	for (auto it : m_NetworkIDTOGameObjMap)
	{
		it.second->Write(packet);
	}

	SendPacketToAllClients(packet, GameplayPacketType::GPT_REPLICATION_DATA);
}

void NetworkManagerServer::SendGameEndedPacket()
{
	OutputMemoryBitStream packet;
	packet.Write(GPT_GAME_ENDED, GetRequiredBits<GPT_MAX_PACKET>::Value);
	packet.Write(m_GameWinner);

	SendPacketToAllClients(packet, GPT_GAME_ENDED);
}

void NetworkManagerServer::SendPacketToAllClients(OutputMemoryBitStream& packet, GameplayPacketType packetType)
{
	for (auto it : m_AddressToUserMap)
	{
		// Create copy of packet for client
		OutputMemoryBitStream clientPacket(packet);

		// ACKs and RPC present only when replication data is being sent
		if (packetType == GameplayPacketType::GPT_REPLICATION_DATA)
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
