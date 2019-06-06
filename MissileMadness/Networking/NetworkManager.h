#pragma once

#include "../Types.h"
#include "NetworkedGameObject.h"

#include <unordered_map>

enum PacketType 
{
	HELLO,
	WELCOME,
	REPLICATION_DATA,
	MAX_PACKET
};

class NetworkManager
{
public:

	const int maxPacketsPerFrame = 10;

	virtual void Initialize() = 0;
	virtual void ProcessIncomingPackets() = 0;

	virtual void UpdateSendingPackets() = 0;

	template <typename T>
	T* CreateNetworkedGameObject()
	{
		UInt32 netID = m_NextNetworkID++;
		T* newObj = T::CreateInstance(netID);
		m_NetworkIDTOGameObjMap[netID] = newObj;
	}

	NetworkedGameObject* GetGameObject(UInt32 netID)
	{
		auto it = m_NetworkIDTOGameObjMap.find(netID);
		if (it != m_NetworkIDTOGameObjMap.end())
			return it->second;
		else
			return nullptr;
	}

	void RemoveGameObject(NetworkedGameObject* objToRemove)
	{
		m_NetworkIDTOGameObjMap.erase(objToRemove->GetNetworkID());
	}

protected:
	NetworkManager() {};
	virtual ~NetworkManager() {};

private:
	UInt32 m_NextNetworkID = 0;

	std::unordered_map<UInt32, NetworkedGameObject*> m_NetworkIDTOGameObjMap;
};

