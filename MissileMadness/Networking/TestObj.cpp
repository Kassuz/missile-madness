#include "TestObj.h"
#include "../Debug.h"

void TestObj::Write(OutputMemoryBitStream& output)
{
	output.Write(GetNetworkID());
	output.Write(GetClassID());

	output.Write(m_ServerTime);
	output.Write(m_Counter);
}

void TestObj::Read(InputMemoryBitStream& input)
{
	// ClassID and NetworkID have been read elsewhere
	input.Read(m_ServerTime);
	input.Read(m_Counter);

	Debug::LogFormat("Recieved TestObj, ServerTime: %f, Counter: %u", m_ServerTime, m_Counter);
}
