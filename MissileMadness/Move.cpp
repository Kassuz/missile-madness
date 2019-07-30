#include "Move.h"

#include "Debug.h"

UInt32 Move::s_NextMoveID = 1; // 0 is reserved for uninitialized moves

Move::Move(bool right, bool left, bool up, bool down, bool space, float timestamp, float deltaT)
		   : m_RightState(right), 
			 m_LeftState(left), 
			 m_UpState(up), 
			 m_DownState(down), 
			 m_SpaceState(space), 
			 m_Timestamp(timestamp), 
			 m_DeltaT(deltaT)
{
	m_MoveID = s_NextMoveID++;
}

Move::Move() : m_RightState(false),
			   m_LeftState(false),
	           m_UpState(false),
			   m_DownState(false),
			   m_SpaceState(false),
			   m_Timestamp(0.0f),
			   m_DeltaT(0.0f),
			   m_MoveID(0)
{
}

void Move::Write(OutputMemoryBitStream& output)
{
	output.Write(m_MoveID);
	output.Write(m_RightState);
	output.Write(m_LeftState);
	output.Write(m_UpState);
	output.Write(m_DownState);
	output.Write(m_SpaceState);
	output.Write(m_Timestamp);
	output.Write(m_DeltaT);
}

void Move::Read(InputMemoryBitStream& input)
{
	input.Read(m_MoveID);
	input.Read(m_RightState);
	input.Read(m_LeftState);
	input.Read(m_UpState);
	input.Read(m_DownState);
	input.Read(m_SpaceState);
	input.Read(m_Timestamp);
	input.Read(m_DeltaT);
}

