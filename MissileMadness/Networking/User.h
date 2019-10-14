#pragma once

#include <string>

#include "../Types.h"
#include "../Move.h"
#include "../Color.h"
#include "MemoryBitStream.h"
#include "SerializableObject.h"

class User : public SerializableObject
{
public:
	User() : m_UserName(""), m_UserID(0), m_IsMe(false), m_CharacterColor(Color::White()) 
	{ }
	User(const std::string& name, UInt32 id, bool isMe = false, Color color = Color::White()) 
		: m_UserName(name), m_UserID(id), m_IsMe(isMe), m_CharacterColor(color)
	{ }
	~User() { }

	static User* Me;

	const std::string GetUsersName() const { return m_UserName; } // Somehow there is macro for GetUserName???
	const UInt32 GetUserID() const { return m_UserID; }
	const bool IsMe() const { return m_IsMe; }
	const Color GetCharacterColor() const { return m_CharacterColor; }
	const bool IsReady() const { return m_IsReady; }

	void SetUserName(std::string name) { m_UserName = name; }
	void SetUserID(UInt32 id) { m_UserID = id; }
	void SetIsMe(bool isMe) { m_IsMe = isMe; }
	void SetCharacterColor(Color c) { m_CharacterColor = c; }
	void SetReady(bool ready) { m_IsReady = ready; }
	void ToggleReady() { m_IsReady = !m_IsReady; }

	void AddNewMove(Move* move);
	Move* GetFirstMove(bool peek = false);
	void AcknowlegeMoves(float latestAcked);

	void WriteMoves(OutputMemoryBitStream& packet);
	void ReadMoves(InputMemoryBitStream& packet);

	const float GetLastProcessedMoveTimestamp() const { return m_LastProcessedMove; }
	const UInt32 GetMoveCount() const { return m_MoveCount; }

	const float GetLastRecievedPacketTime() const { return m_LastRecievedPacket; }
	void SetLastRecievedPacketTime(float time) { m_LastRecievedPacket = time; }

	virtual void Write(OutputMemoryBitStream& output);
	virtual void Read(InputMemoryBitStream& input);

private:
	std::string m_UserName;
	UInt32 m_UserID;
	bool m_IsMe;

	bool m_IsReady = false;

	Color m_CharacterColor;
	
	MoveList m_UserMoves = nullptr; // Could also track tail, so adding would be faster
	UInt32 m_MoveCount = 0U;
	float m_LastProcessedMove = 0.0f;

	const UInt32 k_MaxMovesToWrite = 30U;

	float m_LastRecievedPacket = 0.0f;
};


