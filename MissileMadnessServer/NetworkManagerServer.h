#pragma once

#include "Networking/NetworkManager.h"
#include "Networking/Sockets.h"
#include "Networking/MemoryBitStream.h"
#include "Types.h"

#include <unordered_map>

constexpr UInt16 SERVER_PORT = 45000;

class NetworkManagerServer : public NetworkManager
{
public:
	enum class ServerState { NOT_INITIALIZED, WELCOMING_CLIENTS, SIMULATING };

	static NetworkManagerServer& Instance();

	virtual void Initialize() override;
	virtual void ProcessIncomingPackets() override;

	virtual void UpdateSendingPackets() override;

protected:
	NetworkManagerServer();
	virtual ~NetworkManagerServer();

private:
	const UInt32 k_MaxUsers = 2U;

	ServerState m_ServerState = ServerState::NOT_INITIALIZED;

	UDPSocketPtr m_ServerSocket;

	UInt32 m_NextUserID = 1; // 0 is reserved for not initialized users

	std::unordered_map<SocketAddress, User*> m_AddressToUserMap;
	std::unordered_map<User*, SocketAddress> m_UserToAddressMap;

	void HandleHelloPacket(InputMemoryBitStream& packet, SocketAddress sender);
	void HandleReplication(InputMemoryBitStream& packet, SocketAddress sender);

	void SendWelcomePacket(SocketAddress dest);
	void SendGameStartPacket();
	void SendWorldState();

	void SendPacketToAllClients(OutputMemoryBitStream& packet);

	//temp
	float m_NextWorldStatePacket = 0.0f;
	const float k_WorldStatePacketIntervall = 3.0f;
};