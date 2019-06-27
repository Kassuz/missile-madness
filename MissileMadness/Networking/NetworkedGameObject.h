#pragma once

#include "../GameObject.h"
#include "../Types.h"

#include "MemoryBitStream.h"

// Macro for generating class identification for classes that derive from NetworkedGameObject
// Must be included in every derived class
#define CLASS_IDENTIFICATION(inCode, inClass)\
enum { kClassId = inCode }; \
virtual UInt32 GetClassID() const override { return kClassId; } \
static NetworkedGameObject* CreateInstance(UInt32 networkID) { return static_cast<NetworkedGameObject*>(new inClass(networkID)); } 

// NetworkedGameObjects should be only created thorugh NetworkManager!
class NetworkedGameObject : public GameObject
{
public:

	virtual UInt32 GetClassID() const = 0;

	UInt32 GetNetworkID() { return m_NetworkID; }

	virtual void Write(OutputMemoryBitStream& output) = 0;
	virtual void Read(InputMemoryBitStream& input) = 0;


	virtual ~NetworkedGameObject() {}

protected:

	NetworkedGameObject(UInt32 networkID) : m_NetworkID(networkID)
	{}

private:
	UInt32 m_NetworkID = 0;
};