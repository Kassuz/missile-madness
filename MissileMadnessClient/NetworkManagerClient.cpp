#include "NetworkManagerClient.h"

#include "Engine.h"

#include "ClientRPCs.h"
#include "ClientGame.h"
#include "CommandLineArgs.h"

NetworkManagerClient& NetworkManagerClient::Instance()
{
	static NetworkManagerClient instance;
	return instance;
}

void NetworkManagerClient::Initialize()
{
	SocketUtil::StaticInit();

	m_ServerAddress = SocketUtil::CreateSocketAress(SERVER_ADDRESS);
	m_ClientSocket = SocketUtil::CreateUDPSocket(INET);
	m_ClientSocket->SetNonBlockingMode(true);

	m_ClientState = ClientState::NOT_REGISTERED;

	// Register RPC unwrap functions
	m_RPCManager.RegisterRPCUnwrapFunction(RPCParams::UserDisconnedtRPC::k_RPCName, RPCUnwrap::UnwrapUserDisconnect);
	m_RPCManager.RegisterRPCUnwrapFunction(RPCParams::DestroyNetworkedGameObject::k_RPCName, RPCUnwrap::UnwrapDestroyNetworkedGameObject);
	m_RPCManager.RegisterRPCUnwrapFunction(RPCParams::SetNetworkedGameObjectActive::k_RPCName, RPCUnwrap::UnwrapSetNGOBJActive);
}


void NetworkManagerClient::ProcessIncomingPackets()
{
	for (int i = 0; i < k_MaxPacketsPerFrame; ++i)
	{
		Int32 recievedBytes = m_ClientSocket->ReceiveFrom(m_RecieveBuffer, 1500, m_RecieveAddress);
		if (recievedBytes > 0)
		{
			InputMemoryBitStream packet(m_RecieveBuffer, recievedBytes * 8);
			PacketType type;
			packet.Read(type, GetRequiredBits<PacketType::MAX_PACKET>::Value);

			switch (type)
			{
			case HELLO:
				Debug::LogError("Client should not recieve HELLO packets");
				break;
			case WELCOME:
				ProcessWelcomePacket(packet);
				break;
			case GAME_START:
				ProcessGameStartPacket(packet);
				break;
			case REPLICATION_DATA:
				ProcessReplicationData(packet);
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
			Debug::LogError("Server not responding!");
			m_ClientState = ClientState::DISCONNECTED;
		}
	}
}

void NetworkManagerClient::UpdateSendingPackets()
{
	switch (m_ClientState)
	{
	case ClientState::NOT_INITIALIZED:
	case ClientState::WAITING:
	case ClientState::ENGINE_START:
		// Do nothing
		break;
	case ClientState::NOT_REGISTERED:
		SendHelloPacket();
		break;
	case ClientState::REPLICATING:
		SendInputs();
		break;
	default:
		break;
	}
}

void NetworkManagerClient::InitUser(std::string userName)
{
	if (m_ClientUser != nullptr) delete m_ClientUser;
	m_ClientUser = new User(userName, 0, true);

	User::Me = m_ClientUser;
}

User* NetworkManagerClient::GetUserWithID(UInt32 userID)
{
	for (auto u : m_Users)
	{
		if (u->GetUserID() == userID)
			return u;
	}
	return nullptr;
}

NetworkManagerClient::NetworkManagerClient() : m_RTT(20), m_AvgDataIntervall(20)
{
}


NetworkManagerClient::~NetworkManagerClient()
{
}

void NetworkManagerClient::SendHelloPacket()
{
	if (Time::GetTime() >= m_NextHelloTime  && m_ClientUser != nullptr)
	{
		Debug::Log("Send hello packet");
		OutputMemoryBitStream helloPacket;
		helloPacket.Write(PacketType::HELLO, GetRequiredBits<PacketType::MAX_PACKET>::Value);
		helloPacket.Write(m_ClientUser->GetUsersName());

		m_ClientSocket->SendTo(helloPacket.GetBufferPtr(), helloPacket.GetByteLength(), *m_ServerAddress);

		m_NextHelloTime = Time::GetTime() + k_HelloIntervall;
	}
}

void NetworkManagerClient::SendInputs()
{
	if (m_ClientUser->GetMoveCount() > 0 && Time::GetTime() >= m_NextInputSendTime)
	{
		//Debug::Log("Send moves to server");
		m_NextInputSendTime = Time::GetTime() + k_InputSendIntervall;

		OutputMemoryBitStream packet;
		packet.Write(PacketType::REPLICATION_DATA, GetRequiredBits<PacketType::MAX_PACKET>::Value);
		packet.Write(m_ClientUser->GetUserID()); // Write user id just to be sure
		m_ClientUser->WriteMoves(packet);

		// Write ACK for recieved rpcs
		packet.Write(m_RPCManager.GetLastProcessedRPC());

		// Send
		m_ClientSocket->SendTo(packet.GetBufferPtr(), packet.GetByteLength(), *m_ServerAddress);
	}
}

void NetworkManagerClient::ProcessWelcomePacket(InputMemoryBitStream& packet)
{
	if (m_ClientState == ClientState::NOT_REGISTERED)
	{
		UInt32 newID;
		packet.Read(newID);

		m_ClientUser->SetUserID(newID);
		m_ClientState = ClientState::WAITING;

		Debug::LogFormat("Client welcomed, new id: %i", newID);
	}
	else
	{
		Debug::LogWarning("Already welcomed, ignore packet");
	}
}

void NetworkManagerClient::ProcessGameStartPacket(InputMemoryBitStream& packet)
{
	if (m_ClientState != ClientState::WAITING)
		return;

	for (auto it : m_Users)
		delete it;
	m_Users.clear();

	UInt32 userCount;
	packet.Read(userCount, 32);

	std::string userName;
	UInt32 userID;
	Debug::Log("Game start! Users:");
	for (int i = 0; i < userCount; ++i)
	{
		packet.Read(userName);
		packet.Read(userID);

		Debug::LogFormat("\t%s, ID: %u", userName.c_str(), userID);

		if (userID == m_ClientUser->GetUserID())
		{
			m_Users.push_back(m_ClientUser);
		}
		else
		{
			m_Users.push_back(new User(userName, userID));
		}
	}

	// Get replication data send intervall for interpolation
	//float intervall;
	//packet.Read(intervall);
	//ClientGame::SetDataIntervall(intervall);

	m_GameShouldStart = true;
	m_ClientState = ClientState::ENGINE_START;
}

void NetworkManagerClient::ProcessReplicationData(InputMemoryBitStream& packet)
{
	if (m_ClientState != ClientState::REPLICATING)
		return;

	// Check if packet sould be dropped
	if (ExtraMath::GetRandomFloat() < CommandLineArgs::GetPacketLoss())
		return;

	// PacketID
	//------------------------------------------------------
	UInt32 packetID;
	packet.Read(packetID, 32);

	if (m_LastProcessedPacketID >= packetID) // Packet is old, just drop it
	{
		Debug::LogWarningFormat("Drop old packet with ID: %U. Last processed %u.", packetID, m_LastProcessedPacketID);
		return;
	}
	else if (packetID > m_LastProcessedPacketID + 1)
	{
		// Packets have been dropped
		m_DroppedPacketCount += packetID - m_LastProcessedPacketID - 1;
	}

	m_LastProcessedPacketID = packetID;

	// Networked Objects
	//------------------------------------------------------
	UInt32 objCount;
	packet.Read(objCount, 32);

	for (int i = 0; i < objCount; ++i)
	{
		UInt32 networkID, classID;
		packet.Read(networkID);
		packet.Read(classID);

		NetworkedGameObject* obj = GetGameObject(networkID);
		if (obj == nullptr)
			obj = CreateNetworkedGameObject(networkID, classID);

		if (obj == nullptr)
		{
			Debug::LogErrorFormat("Failed to create object of type %s. Can't read rest of the packet!", ConvertUIntToString(classID).c_str());
			return;
		}

		obj->Read(packet);
	}

	// Read RPCs
	//------------------------------------------------------
	m_RPCManager.HandleRPCs(packet);

	// Last thing left is ACKs
	//------------------------------------------------------
	float lastProcessedMove;
	packet.Read(lastProcessedMove);
	User::Me->AcknowlegeMoves(lastProcessedMove);

	// Calculate RTT
	//------------------------------------------------------
	CalculateRTT(lastProcessedMove);

	if (m_LastRecievedPacket > 0.0f)
		m_AvgDataIntervall.AddValue(Time::GetTime() - m_LastRecievedPacket);
	else
		m_AvgDataIntervall.AddValue(0.0f);
	m_LastRecievedPacket = Time::GetTime();
}

void NetworkManagerClient::CalculateRTT(float timestamp)
{
	if (timestamp > m_LastReadTimestamp) // This way the same timestamp wont throw off calculations
	{
		float rtt = Time::GetTime() - timestamp;
		//rtt *= 1000; // Get ms

		m_RTT.AddValue(rtt);
		m_LastReadTimestamp = timestamp;
	}
}


