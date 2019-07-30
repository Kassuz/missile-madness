#pragma once

#include "Networking/RPCManager.h"

class RPCManagerServer : public RPCManager
{
public:
	virtual ~RPCManagerServer();

	virtual void SendRPC(UInt32 name, SerializableObject* rpcParams) override;
	
	void WritePendingRPCs(UInt32 userID, OutputMemoryBitStream& packet);

	void InitUser(UInt32 userID);
	void RemoveUser(UInt32 userID);

	void ACKRPCs(UInt32 user, UInt32 ack);

	void SendDestroyNetworkedGameObjectRPC(UInt32 objToDestroy);
	void SendSetNGOJBActive(UInt32 networkID, bool active);

protected:

	std::unordered_map<UInt32, RPCList*> m_PendingRPCs;

};

