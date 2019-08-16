#pragma once

#include "Networking/NetworkManager.h"
#include "Networking/Sockets.h"
#include "Networking/MemoryBitStream.h"
#include "Networking/User.h"
#include "Networking/RPCManager.h"

#include "Move.h"
#include "ExtraMath.h"

constexpr auto SERVER_ADDRESS = "192.168.1.2:45000";
//constexpr auto SERVER_ADDRESS = "127.0.0.1:45000";

class NetworkManagerClient : public NetworkManager
{
public:
	enum class ClientState { NOT_INITIALIZED, NOT_REGISTERED, WAITING, ENGINE_START, REPLICATING, DISCONNECTED };
	
	static NetworkManagerClient& Instance();

	virtual void Initialize() override;
	virtual void ProcessIncomingPackets() override;

	virtual void UpdateSendingPackets() override;

	void InitUser(std::string userName);

	const ClientState GetClientState() const { return m_ClientState; }

	const bool GameShouldStart() const { return m_GameShouldStart; }
	void StartGame() { m_ClientState = ClientState::REPLICATING; }

	User* GetUserWithID(UInt32 userID);

	float GetRTT() const { return m_RTT.GetAverage(); };
	UInt32 GetDroppedPacketCount() const { return m_DroppedPacketCount; }
	float GetServerSendRate() const { return m_AvgDataIntervall.GetAverage(); }

protected:
	NetworkManagerClient();
	virtual ~NetworkManagerClient();

private:	
	ClientState m_ClientState = ClientState::NOT_INITIALIZED;

	UDPSocketPtr m_ClientSocket;
	SocketAddressPtr m_ServerAddress;
	User* m_ClientUser = nullptr;

	std::vector<User*> m_Users;

	RPCManager m_RPCManager;

	bool m_GameShouldStart = false;

	float m_NextHelloTime = 0.0f;
	const float k_HelloIntervall = 1.0f;

	float m_NextInputSendTime = 0.0f;
	const float k_InputSendIntervall = 1.0f / 15.0f;

	UInt32 m_LastProcessedPacketID = 0;

	void SendHelloPacket();
	void SendInputs();

	void ProcessWelcomePacket(InputMemoryBitStream& packet);
	void ProcessGameStartPacket(InputMemoryBitStream& packet);
	void ProcessReplicationData(InputMemoryBitStream& packet);

	// Stuff for calculating RTT
	ExtraMath::AverageValue<float> m_RTT;
	float m_LastReadTimestamp = 0.0f;
	void CalculateRTT(float timestamp);

	UInt32 m_DroppedPacketCount = 0U;

	ExtraMath::AverageValue<float> m_AvgDataIntervall;
	float m_LastRecievedPacket = 0.0f;
};

