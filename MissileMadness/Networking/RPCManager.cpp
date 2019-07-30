#include "RPCManager.h"

//----------------------
//-        RPC         -
//----------------------
void RPC::Write(OutputMemoryBitStream& output)
{
	output.Write(m_RPCID);
	output.Write(m_RPCName);
	m_RPCParams->Write(output);
}


//----------------------
//-     RPCManager     -
//----------------------

UInt32 RPCManager::s_NextRPCID = 1U;

void RPCManager::HandleRPCs(InputMemoryBitStream& packet)
{
	UInt32 rpcCount;
	packet.Read(rpcCount);

	for (UInt32 i = 0U; i < rpcCount; ++i)
	{
		UInt32 id, name;
		packet.Read(id);
		packet.Read(name);

		Debug::LogFormat("Read RPC %s, with ID: %u. Last read: %u", ConvertUIntToString(name).c_str(), id, m_LastRPC);

		auto it = m_UnwrapFunctions.find(name);
		if (it != m_UnwrapFunctions.end())
		{
			// Unwrap rpc always, so rest of the packet can be read. Unwrap func can then ignore alrady handled rpc
			bool rpcNotHandled = id > m_LastRPC;
			it->second(packet, rpcNotHandled);
			if (rpcNotHandled)
				m_LastRPC = id;
		}
		else
		{
			Debug::LogErrorFormat("Couldn't find unwrap function for %s. Ignore rest of packet.", ConvertUIntToString(name).c_str());
			return;
		}
	}
}

void RPCManager::RegisterRPCUnwrapFunction(UInt32 RPCName, RPCUnwrapFunction func)
{
	m_UnwrapFunctions.emplace(RPCName, func);
}


//----------------------
//-      RPCList       -
//----------------------

void RPCList::AddRPC(RPCPtr& rpc)
{
	if (m_Tail == nullptr)
	{
		m_Head = new RPCListElement(rpc);
		m_Tail = m_Head;
	}
	else
	{
		m_Tail->m_Next = new RPCListElement(rpc);
		m_Tail = m_Tail->m_Next;
	}
	m_RPCCount++;
}

void RPCList::ACK(UInt32 ack)
{
	while (m_Head != nullptr)
	{
		if (m_Head->m_RPC->GetID() <= ack)
		{
			RPCListElement* del = m_Head;
			m_Head = m_Head->m_Next;
			delete del;
			m_RPCCount--;
		}
		else
			break;
	}

	if (m_Head == nullptr)
		m_Tail = nullptr;
}

void RPCList::Write(OutputMemoryBitStream& packet)
{
	packet.Write(m_RPCCount);

	RPCListElement* cur = m_Head;
	while (cur != nullptr)
	{
		cur->m_RPC->Write(packet);
		cur = cur->m_Next;
	}
}
