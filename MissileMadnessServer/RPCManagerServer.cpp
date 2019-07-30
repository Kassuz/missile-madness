#include "RPCManagerServer.h"
#include "Networking/SharedRPC.h"

void RPCManagerServer::SendRPC(UInt32 name, SerializableObject* rpcParams)
{
	RPCPtr rpc = std::make_shared<RPC>(s_NextRPCID++, name, rpcParams);
	Debug::LogFormat("Create RPC %s with id: %u", ConvertUIntToString(rpc->GetName()).c_str(), rpc->GetID());
	for (auto it : m_PendingRPCs)
	{
		it.second->AddRPC(rpc);
	}
}

void RPCManagerServer::WritePendingRPCs(UInt32 userID, OutputMemoryBitStream& packet)
{
	auto it = m_PendingRPCs.find(userID);
	if (it != m_PendingRPCs.end())
	{
		it->second->Write(packet);
	}
	else
	{
		packet.Write(0U, 32);
	}
}

void RPCManagerServer::InitUser(UInt32 userID)
{
	m_PendingRPCs.emplace(userID, new RPCList);
}

void RPCManagerServer::RemoveUser(UInt32 userID)
{
	auto it = m_PendingRPCs.find(userID);
	if (it != m_PendingRPCs.end())
		m_PendingRPCs.erase(it);
}

void RPCManagerServer::ACKRPCs(UInt32 user, UInt32 ack)
{
	auto it = m_PendingRPCs.find(user);
	if (it != m_PendingRPCs.end())
	{
		it->second->ACK(ack);
	}
}

void RPCManagerServer::SendDestroyNetworkedGameObjectRPC(UInt32 objToDestroy)
{
	SendRPC(RPCParams::DestroyNetworkedGameObject::k_RPCName, new RPCParams::DestroyNetworkedGameObject(objToDestroy));
}

void RPCManagerServer::SendSetNGOJBActive(UInt32 networkID, bool active)
{
	SendRPC(RPCParams::SetNetworkedGameObjectActive::k_RPCName, new RPCParams::SetNetworkedGameObjectActive(networkID, active));
}

RPCManagerServer::~RPCManagerServer()
{
	for (auto it : m_PendingRPCs)
	{
		delete it.second;
	}
	m_PendingRPCs.clear();
}
