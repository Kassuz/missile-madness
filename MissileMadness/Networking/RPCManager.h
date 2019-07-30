#pragma once

#include "SerializableObject.h"
#include "../Types.h"

#include <memory>
#include <unordered_map>

class User;

using std::shared_ptr;

//----------------------
//-        RPC         -
//----------------------

typedef shared_ptr<SerializableObject> SerializableObjectPtr;

class RPC
{
public:
	
	void Write(OutputMemoryBitStream& output);
	const UInt32 GetID() const { return m_RPCID; }
	const UInt32 GetName() const { return m_RPCName; }

	RPC(UInt32 id, UInt32 name, SerializableObject* rpcParams) 
		: m_RPCID(id), 
		  m_RPCName(name),
		  m_RPCParams(rpcParams)
	{ }

private:
	UInt32 m_RPCID;
	UInt32 m_RPCName;
	
	SerializableObjectPtr m_RPCParams;

	//friend class RPCManager;
};
typedef shared_ptr<RPC> RPCPtr;

//----------------------
//-     RPCManager     -
//----------------------

typedef void (*RPCUnwrapFunction)(InputMemoryBitStream&, bool);

class RPCManager
{
public:

	virtual void SendRPC(UInt32 name, SerializableObject* rpcParams) { };

	void HandleRPCs(InputMemoryBitStream& packet);

	void RegisterRPCUnwrapFunction(UInt32 RPCName, RPCUnwrapFunction func);

	virtual ~RPCManager() {}
	RPCManager() { }

	const UInt32 GetLastProcessedRPC() const { return m_LastRPC; }

protected:

	std::unordered_map<UInt32, RPCUnwrapFunction> m_UnwrapFunctions;

	UInt32 m_LastRPC = 0;

	static UInt32 s_NextRPCID;
private:
};

//----------------------
//-      RPCList       -
//----------------------

class RPCList
{
public:
	void AddRPC(RPCPtr& rpc);
	void ACK(UInt32 ack);

	void Write(OutputMemoryBitStream& packet);

private:
	struct RPCListElement
	{
	public:
		RPCListElement(RPCPtr& rpc) : m_RPC(rpc) { }
		RPCPtr m_RPC;
		RPCListElement* m_Next = nullptr;
	};
	
	RPCListElement* m_Head = nullptr;
	RPCListElement* m_Tail = nullptr;

	UInt32 m_RPCCount = 0U;
};