#include "NetworkManagerServer.h"

#include "Engine.h"
#include "Networking/User.h"

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
			//Debug::Log(m_RecieveAddress.ToString());
			//Debug::LogFormat("Recieved packet of type %i", type);
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
			case REPLICATION_DATA:
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
			// DC client
			auto it = m_AddressToUserMap.find(m_RecieveAddress);
			if (it == m_AddressToUserMap.end()) continue; // Clinet already disconnected
			
			User* user = it->second;
			Debug::LogErrorFormat("User %s disconnected", user->GetUsersName().c_str());
			m_UserToAddressMap.erase(user);
			m_AddressToUserMap.erase(it);
			delete user;
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
			SendGameStartPacket();
		}
		break;
	case NetworkManagerServer::ServerState::SIMULATING:
		SendWorldState();
		break;
	default:
		break;
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
	SendPacketToAllClients(packet);

	m_ServerState = ServerState::SIMULATING;
}

void NetworkManagerServer::SendWorldState()
{
	// Test send every 3 sec
	float time = Time::GetTime();
	if (time < m_NextWorldStatePacket) return;
	m_NextWorldStatePacket = time + k_WorldStatePacketIntervall;

	OutputMemoryBitStream packet;
	packet.Write(PacketType::REPLICATION_DATA, GetRequiredBits<PacketType::MAX_PACKET>::Value);
	packet.Write(m_NetworkIDTOGameObjMap.size(), 32);

	for (auto it : m_NetworkIDTOGameObjMap)
	{
		it.second->Write(packet);
	}

	SendPacketToAllClients(packet);
}

void NetworkManagerServer::SendPacketToAllClients(OutputMemoryBitStream& packet)
{
	for (auto it : m_AddressToUserMap)
	{
		m_ServerSocket->SendTo(packet.GetBufferPtr(), packet.GetByteLength(), it.first);
	}
}
