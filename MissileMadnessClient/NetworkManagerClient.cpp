#include "NetworkManagerClient.h"

#include "Engine.h"

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
	}
}

void NetworkManagerClient::UpdateSendingPackets()
{
	switch (m_ClientState)
	{
	case ClientState::NOT_INITIALIZED:
		// Do nothing
		break;
	case ClientState::NOT_REGISTERED:
		SendHelloPacket();
		break;
	case ClientState::WAITING:
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
}

NetworkManagerClient::NetworkManagerClient()
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
	Debug::Log("Game start!\nUsers:");
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

	m_ClientState = ClientState::REPLICATING;
}

void NetworkManagerClient::ProcessReplicationData(InputMemoryBitStream& packet)
{
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
			Debug::LogError("Failed to create object. Can't read rest of the packet!");
			return;
		}

		obj->Read(packet);
	}
}


