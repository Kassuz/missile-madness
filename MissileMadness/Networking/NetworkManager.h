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

	const int k_MaxPacketsPerFrame = 10;

	virtual void Initialize() = 0;
	virtual void ProcessIncomingPackets() = 0;

	virtual void UpdateSendingPackets() = 0;

	template <typename T>
	T* CreateNetworkedGameObject()
	{
		UInt32 netID = m_NextNetworkID++;
		NetworkedGameObject* newObj = T::CreateInstance(netID);
		m_NetworkIDTOGameObjMap[netID] = newObj;
		return static_cast<T*>(newObj);
	}

	NetworkedGameObject* GetGameObject(UInt32 netID)
	{
		auto it = m_NetworkIDTOGameObjMap.find(netID);
		if (it != m_NetworkIDTOGameObjMap.end())
			return it->second;
		else
			return nullptr;
	}

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
			Debug::LogErrorFormat("NO Object Creation function defined for %c", classID);
			return nullptr;
		}
	}

	void RemoveGameObject(NetworkedGameObject* objToRemove)
	{
		m_NetworkIDTOGameObjMap.erase(objToRemove->GetNetworkID());
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

private:
	UInt32 m_NextNetworkID = 0;

	std::unordered_map<UInt32, GameObjectCreationFunc> m_CreationFunctions;
};

