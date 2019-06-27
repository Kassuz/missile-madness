#pragma once

#include "NetworkedGameObject.h"

class TestObj : public NetworkedGameObject
{
public:
	CLASS_IDENTIFICATION('TOBJ', TestObj);

	virtual void Write(OutputMemoryBitStream& output) override;
	virtual void Read(InputMemoryBitStream& input) override;

	const float GetServerTime() const { return m_ServerTime; }
	void SetServerTime(float time) { m_ServerTime = time; }

	const UInt32 GetCounter() const { return m_Counter; }
	void IncreaseCounter() { m_Counter++; }

	virtual ~TestObj() { }

protected:
	TestObj(UInt32 networkID) : NetworkedGameObject(networkID) {}

	float m_ServerTime = 0.0f;
	UInt32 m_Counter = 0U;
};