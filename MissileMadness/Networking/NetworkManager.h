#pragma once

#include "../Types.h"
#include "../Debug.h"
#include "NetworkedGameObject.h"
#include "Sockets.h"

#include <unordered_map>
#include <vector>

class User;

enum PacketType 
{
	HELLO,
	WELCOME,
	GAME_START,
	REPLICATION_DATA,
	MAX_PACKET
};

class NetworkManager
{
public:
	typedef NetworkedGameObject* (*GameObjectCreationFunc)(UInt32);

	const int k_MaxPacketsPerFrame = 30;

	virtual bool Initialize() = 0;
	virtual void ProcessIncomingPackets() = 0;

	virtual void UpdateSendingPackets() = 0;

	NetworkedGameObject* GetGameObject(UInt32 netID)
	{
		auto it = m_NetworkIDTOGameObjMap.find(netID);
		if (it != m_NetworkIDTOGameObjMap.end())
			return it->second;
		else
			return nullptr;
	}

	// Removes the networked gameobject from being replicated, but does not destroy it
	NetworkedGameObject* RemoveGameObject(UInt32 networkID)
	{
		auto it = m_NetworkIDTOGameObjMap.find(networkID);
		if (it != m_NetworkIDTOGameObjMap.end())
		{
			NetworkedGameObject* ngobj = it->second;
			m_NetworkIDTOGameObjMap.erase(it);
			return ngobj;
		}
		else
		{
			return nullptr;
		}
	}

	void RegisterCreationFunc(UInt32 classID, GameObjectCreationFunc func)
	{
		m_CreationFunctions[classID] = func;
	}

protected:
	NetworkManager() {}
	virtual ~NetworkManager() {}

	Byte m_RecieveBuffer[1500];
	SocketAddress m_RecieveAddress;

	std::unordered_map<UInt32, NetworkedGameObject*> m_NetworkIDTOGameObjMap;

	// Creates NetworkedGameObject of type classID with ID: networkID
	// Should be used when recieved data for new NetworkedGameObject
	NetworkedGameObject* CreateNetworkedGameObject(UInt32 networkID, UInt32 classID)
	{
		auto it = m_CreationFunctions.find(classID);
		if (it != m_CreationFunctions.end())
		{
			NetworkedGameObject* obj = m_CreationFunctions[classID](networkID);
			m_NetworkIDTOGameObjMap[networkID] = obj;
			return obj;
		}
		else
		{
			Debug::LogErrorFormat("NO Object Creation function defined for %s", ConvertUIntToString(classID).c_str());
			return nullptr;
		}
	}

	UInt32 m_NextNetworkID = 0;

private:
	std::unordered_map<UInt32, GameObjectCreationFunc> m_CreationFunctions;
};

