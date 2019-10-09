#pragma once

#include "Networking/NetworkManager.h"
#include "Networking/Sockets.h"
#include "Networking/MemoryBitStream.h"
#include "Types.h"

#include "RPCManagerServer.h"

#include <unordered_map>

constexpr UInt16 SERVER_PORT_UDP = 45000;

class NetworkManagerServer : public NetworkManager
{
public:
	enum class ServerState { NOT_INITIALIZED, WELCOMING_CLIENTS, STARTING, SIMULATING };

	virtual ~NetworkManagerServer();
	
	static NetworkManagerServer& Instance();

	virtual bool Initialize() override;
	virtual void ProcessIncomingPackets() override;

	virtual void UpdateSendingPackets() override;

	// Creates new NetworkedGameObject
	template <typename T>
	T* CreateNetworkedGameObject()
	{
		UInt32 netID = m_NextNetworkID++;
		NetworkedGameObject* newObj = T::CreateInstance(netID);
		m_NetworkIDTOGameObjMap[netID] = newObj;
		return static_cast<T*>(newObj);
	}

	User* GetUserWithID(UInt32 userID);
	std::vector<User*> GetAllUsers();

	bool IsGameStarting() const { return m_ServerState == ServerState::STARTING; }
	void StartGame() { m_ServerState = ServerState::SIMULATING; }

	UInt32 UserCount() const { return m_AddressToUserMap.size(); }

	void CheckForDisconnectedUsers();

	RPCManagerServer& RPC() { return m_RPCManager; }


protected:
	NetworkManagerServer();

private:
	const UInt32 k_MaxUsers = 2U;

	ServerState m_ServerState = ServerState::NOT_INITIALIZED;

	UDPSocketPtr m_ServerSocket;

	UInt32 m_NextUserID = 1; // 0 is reserved for not initialized users
	UInt32 m_NextPacketID = 1; // Used for world state packets only

	std::unordered_map<SocketAddress, User*> m_AddressToUserMap;
	std::unordered_map<User*, SocketAddress> m_UserToAddressMap;

	const float k_DisconnectTime = 10.0f; // Time of inactivity after which user is considered disconnected
	void DisconnectUser(User* userToDisconnect, const char* disconnectReason);

	void HandleHelloPacket(InputMemoryBitStream& packet, SocketAddress sender);
	void HandleReplication(InputMemoryBitStream& packet, SocketAddress sender);

	const float k_ReplicationSendIntervall = 1.0f / 15.0f;
	float m_NextReplicationTime = 0.0f;

	void SendWelcomePacket(SocketAddress dest);
	void SendGameStartPacket();
	void SendWorldState();

	void SendPacketToAllClients(OutputMemoryBitStream& packet, GameplayPacketType packetType);

	void WriteACKs(OutputMemoryBitStream& packet, User* user);

	RPCManagerServer m_RPCManager;
};