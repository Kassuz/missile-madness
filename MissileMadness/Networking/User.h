#pragma once

#include <string>

#include "../Types.h"
#include "../Move.h"
#include "MemoryBitStream.h"

class User
{
public:
	User() : m_UserName(""), m_UserID(0), m_IsMe(false) { }
	User(std::string name, UInt32 id, bool isMe = false) : m_UserName(name), m_UserID(id), m_IsMe(isMe) { }
	~User() { }

	static User* Me;

	const std::string GetUsersName() const { return m_UserName; } // Somehow there is macro for GetUserName???
	const UInt32 GetUserID() const { return m_UserID; }
	const bool IsMe() const { return m_IsMe; }

	void SetUserName(std::string name) { m_UserName = name; }
	void SetUserID(UInt32 id) { m_UserID = id; }
	void SetIsMe(bool isMe) { m_IsMe = isMe; }

	void AddNewMove(Move* move);
	Move* GetFirstMove();
	void AcknowlegeMoves(UInt32 latestAcked);

	void WriteMoves(OutputMemoryBitStream& packet);
	void ReadMoves(InputMemoryBitStream& packet);

	const UInt32 GetLastProcessedMoveID() const { return m_LastProcessedMove; }
	const UInt32 GetMoveCount() const { return m_MoveCount; }

	const float GetLastRecievedPacketTime() const { return m_LastRecievedPacket; }
	void SetLastRecievedPacketTime(float time) { m_LastRecievedPacket = time; }

private:
	std::string m_UserName;
	UInt32 m_UserID;
	bool m_IsMe;
	
	MoveList m_UserMoves = nullptr; // Could also track tail, so adding would be faster
	UInt32 m_MoveCount = 0U;
	UInt32 m_LastProcessedMove = 0U;

	const UInt32 k_MaxMovesToWrite = 10U;

	float m_LastRecievedPacket = 0.0f;
};


