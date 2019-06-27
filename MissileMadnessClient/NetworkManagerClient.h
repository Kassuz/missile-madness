#pragma once

#include "Networking/NetworkManager.h"
#include "Networking/Sockets.h"
#include "Networking/MemoryBitStream.h"
#include "Networking/User.h"


constexpr auto SERVER_ADDRESS = "127.0.0.1:45000";

class NetworkManagerClient : public NetworkManager
{
public:
	enum class ClientState { NOT_INITIALIZED, NOT_REGISTERED, WAITING, REPLICATING };
	
	static NetworkManagerClient& Instance();

	virtual void Initialize() override;
	virtual void ProcessIncomingPackets() override;

	virtual void UpdateSendingPackets() override;

	void InitUser(std::string userName);

	const ClientState GetClientState() const { return m_ClientState; }

protected:
	NetworkManagerClient();
	virtual ~NetworkManagerClient();

private:	
	ClientState m_ClientState = ClientState::NOT_INITIALIZED;

	UDPSocketPtr m_ClientSocket;
	SocketAddressPtr m_ServerAddress;
	User* m_ClientUser = nullptr;

	std::vector<User*> m_Users;

	float m_NextHelloTime = 0.0f;
	const float k_HelloIntervall = 3.0f;

	void SendHelloPacket();
	void SendInputs();

	void ProcessWelcomePacket(InputMemoryBitStream& packet);
	void ProcessGameStartPacket(InputMemoryBitStream& packet);
	void ProcessReplicationData(InputMemoryBitStream& packet);
};

