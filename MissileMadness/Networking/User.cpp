#include "User.h"
#include "../Engine.h"

User* User::Me = nullptr;

void User::AddNewMove(Move* move)
{
	//UInt32 newID = move->GetID();
	float newTime = move->GetTimestamp();
	
	//if (newID <= m_LastProcessedMove)
	if (newTime <= m_LastProcessedMove)
	{
		delete move;
		return;
	}

	if (m_UserMoves == nullptr)
	{
		m_UserMoves = move;
	}
	else if (newTime < m_UserMoves->GetTimestamp())
	{
		move->next = m_UserMoves;
		m_UserMoves = move;
	}
	else
	{
		Move* m = m_UserMoves;
		while (m != nullptr)
		{
			Move* next = m->next;
			if (m->GetTimestamp() == newTime)
			{
				// Move already in list
				delete move;
				return;
			}
			else if (newTime > m->GetTimestamp() && (next == nullptr || newTime < next->GetTimestamp()))
			{
				m->next = move;
				move->next = next;
				break;
			}
			m = next;
		}
		//Move* last = m_UserMoves;
		//while (last->next != nullptr)
		//	last = last->next;
		//UInt32 lastID = last != nullptr ? last->GetID() : 0U;

		//if (newID <= lastID)
		//{
		//	delete move;
		//	return;
		//}
		//last->next = move;
	}

	m_MoveCount++;
}

Move* User::GetFirstMove(bool peek)
{
	Move* first = m_UserMoves;
	if (first != nullptr && !peek)
	{
		m_UserMoves = first->next;
		m_MoveCount--;
		//m_LastProcessedMove = first->GetID();
		m_LastProcessedMove = first->GetTimestamp();
	}
	return first;
}

void User::AcknowlegeMoves(float latestAcked)
{
	m_LastProcessedMove = latestAcked;

	Move* m = m_UserMoves;

	while (m != nullptr)
	{
		Move* next = m->next;
		if (m->GetTimestamp() <= latestAcked)
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

	Move* m = m_UserMoves;
	while (m != nullptr)
	{
		if (skipCount < movesToSkip)
		{
			skipCount++;
		}
		else
		{
			m->Write(packet);
		}
		m = m->next;
	}
}

void User::ReadMoves(InputMemoryBitStream& packet)
{
	UInt32 moveCount;
	packet.Read(moveCount);

	for (UInt32 i = 0; i < moveCount; ++i)
	{
		Move* m = new Move();
		m->Read(packet);
		AddNewMove(m);
	}
}
