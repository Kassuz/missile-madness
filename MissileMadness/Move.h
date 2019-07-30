#pragma once

#include "Types.h"
#include "InputManager.h"

#include "Networking/SerializableObject.h"

class Move : public SerializableObject
{
public:
	Move(bool right, bool left, bool up, bool down, bool space, float timestamp, float deltaT);
	Move();


	virtual void Write(OutputMemoryBitStream& output) override;
	virtual void Read(InputMemoryBitStream& input) override;

	const UInt32 GetID() const { return m_MoveID; }

	const bool GetRightState() const { return m_RightState; }
	const bool GetLeftState()  const { return m_LeftState;  }
	const bool GetUpState()    const { return m_UpState;    }
	const bool GetDownState()  const { return m_DownState;  }
	const bool GetSpaceState() const { return m_SpaceState; }
	
	const float GetTimestamp() const { return m_Timestamp; }
	const float GetDeltaT()    const { return m_DeltaT; }

	Move* next = nullptr;

private:
	static UInt32 s_NextMoveID;
	
	UInt32 m_MoveID;
	bool m_RightState, m_LeftState, m_UpState, m_DownState, m_SpaceState;
	float m_Timestamp, m_DeltaT;
};

typedef Move* MoveList;
