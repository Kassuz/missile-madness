#pragma once

#include "MemoryBitStream.h"

class SerializableObject
{
public:

	virtual void Write(OutputMemoryBitStream& output) = 0;
	virtual void Read(InputMemoryBitStream& input) = 0;
};