#include "User.h"
#include "../Engine.h"

User* User::Me = nullptr;

void User::AddNewMove(Move* move)
{
	UInt32 newID = move->GetID();

	//if (m_IsMe && m_UserName == "Kassu")
	//	Debug::LogFormat("Try to add move %u", newID);
	
	if (newID <= m_LastProcessedMove)
	{
		// Move already processed
		//if (m_UserName == "Kassu")
		//	Debug::LogErrorFormat("Delete move %u, because already processed. Last %u", move->GetID(), m_LastProcessedMove);
		delete move;
		return;
	}

	if (m_UserMoves == nullptr)
	{
		m_UserMoves = move;

		//if (!m_IsMe && m_UserName == "Kassu" && newID != m_LastProcessedMove + 1)
		//Debug::LogErrorFormat("Skipped moves between %u - %u", m_LastProcessedMove, newID);
	}
	else
	{
		Move* last = m_UserMoves;
		while (last->next != nullptr)
			last = last->next;
		UInt32 lastID = last != nullptr ? last->GetID() : 0U;

		if (newID <= lastID)
		{
			//Move already in list
			//if (m_UserName == "Kassu")
			//	Debug::LogErrorFormat("Delete move %u, because already in list. Last %u", move->GetID(), m_LastProcessedMove);
			delete move;
			return;
		}
		//else if (newID != lastID + 1)
		//	Debug::LogErrorFormat("Skipped moves for user: %s", m_UserName.c_str());

		last->next = move;
	}

	m_MoveCount++;
}

Move* User::GetFirstMove()
{
	Move* first = m_UserMoves;
	if (first != nullptr)
	{
		m_UserMoves = first->next;
		m_MoveCount--;
		m_LastProcessedMove = first->GetID();
	}
	return first;
}

void User::AcknowlegeMoves(UInt32 latestAcked)
{
	//if (m_IsMe && m_UserName == "Kassu")
	//	Debug::LogFormat("Acknowlege moves until %u", latestAcked);
	
	Move* m = m_UserMoves;

	while (m != nullptr)
	{
		Move* next = m->next;
		if (m->GetID() <= latestAcked)
		{
			m_MoveCount--;
			delete m;
		}
		else
		{
			break;
		}
		
		m = next;
	}

	m_UserMoves = m;
}

void User::WriteMoves(OutputMemoryBitStream& packet)
{
	// Write only max k_MaxtoMovesToWrite moves
	UInt32 moveCount = m_MoveCount > k_MaxMovesToWrite ? k_MaxMovesToWrite : m_MoveCount;
	UInt32 movesToSkip = m_MoveCount - moveCount;
	UInt32 skipCount = 0U;

	packet.Write(moveCount);

	//std::string movesStr = "";
	Move* m = m_UserMoves;
	while (m != nullptr)
	{
		if (skipCount < movesToSkip)
		{
			skipCount++;
			//if (m_UserName == "Kassu")
			//	Debug::LogErrorFormat("Skip writing move %u, too many moves. Last ACK: %u", m->GetID(), m_LastProcessedMove);
		}
		else
		{
			m->Write(packet);
			//movesStr.append(std::to_string(m->GetID()));
			//movesStr.append(" ");
		}
		m = m->next;
	}

	//if (m_UserName == "Kassu")
	//	Debug::LogWarningFormat("Write moves: %s", movesStr.c_str());
}

void User::ReadMoves(InputMemoryBitStream& packet)
{
	UInt32 moveCount;
	packet.Read(moveCount);

	//Debug::LogFormat("User %s, Recieved %u moves", m_UserName.c_str(), moveCount);

	for (int i = 0; i < moveCount; ++i)
	{
		Move* m = new Move();
		m->Read(packet);
		AddNewMove(m);
	}
}
