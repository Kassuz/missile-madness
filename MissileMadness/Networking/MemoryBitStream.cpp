#include "MemoryBitStream.h"

#include <algorithm>
#include <glm/gtc/quaternion.hpp>

void OutputMemoryBitStream::WriteBits(Byte inData, UInt32 inBitCount)
{
	UInt32 nextBitHead = mBitHead + static_cast<UInt32>(inBitCount);

	if (nextBitHead > mBitCapacity)
	{
		ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
	}
	// Bitwise black magic

	//calculate the byteOffset into our buffer
	//by dividing the head by 8
	//and the bitOffset by taking the last 3 bits
	UInt32 byteOffset = mBitHead >> 3;
	UInt32 bitOffset = mBitHead & 0x7;

	UInt8 currentMask = ~(0xff << bitOffset);
	mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (inData << bitOffset);

	//calculate how many bits were not yet used in
	//our target byte in the buffer
	UInt32 bitsFreeThisByte = 8 - bitOffset;

	//if we needed more than that, carry to the next byte
	if (bitsFreeThisByte < inBitCount)
	{
		//we need another byte
		mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
	}

	mBitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* inData, UInt32 inBitCount)
{
	const Byte* srcByte = static_cast<const Byte*>(inData);
	//write all the bytes
	while (inBitCount > 8)
	{
		WriteBits(*srcByte, 8);
		++srcByte;
		inBitCount -= 8;
	}
	//write anything left
	if (inBitCount > 0)
	{
		WriteBits(*srcByte, inBitCount);
	}
}

void OutputMemoryBitStream::Write(const glm::vec3& inVector)
{
	Write(inVector.x);
	Write(inVector.y);
	Write(inVector.z);
}

void OutputMemoryBitStream::Write(const glm::quat& inQuat)
{
	// 16bit fixed representation is enough for quats. 
	// W can be calcluated from other components, only thing neededf is sign
	float precision = (2.f / 65535.0f);
	Write(ConvertToFixed(inQuat.x, -1.0f, precision), 16);
	Write(ConvertToFixed(inQuat.y, -1.0f, precision), 16);
	Write(ConvertToFixed(inQuat.z, -1.0f, precision), 16);
	Write(inQuat.w < 0.0f);
}

void OutputMemoryBitStream::ReallocBuffer(UInt32 inNewBitLength)
{
	if (mBuffer == nullptr)
	{
		//just need to memset on first allocation
		mBuffer = static_cast<Byte*>(std::malloc(inNewBitLength >> 3));
		memset(mBuffer, 0, inNewBitLength >> 3);
	}
	else
	{
		//need to memset, then copy the buffer
		Byte* tempBuffer = static_cast<Byte*>(std::malloc(inNewBitLength >> 3));
		memset(tempBuffer, 0, inNewBitLength >> 3);
		memcpy(tempBuffer, mBuffer, mBitCapacity >> 3);
		std::free(mBuffer);
		mBuffer = tempBuffer;
	}

	//handle realloc failure
	//...
	mBitCapacity = inNewBitLength;
}

void OutputMemoryBitStream::HexDump() const
{
	UInt32 bufferSize = GetByteLength();
	printf("Buffer len: %i\n", bufferSize);
	for (int i = 0; i < bufferSize; ++i)
	{
		printf("%02hhX ", mBuffer[i]);
	}
	printf("\n");
}



void InputMemoryBitStream::ReadBitsImpl(Byte& outData, UInt32 inBitCount)
{
	// Bitwise black magic

	UInt32 byteOffset = mBitHead >> 3;
	UInt32 bitOffset = mBitHead & 0x7;

	outData = static_cast<UInt8>(mBuffer[byteOffset]) >> bitOffset;

	UInt32 bitsFreeThisByte = 8 - bitOffset;
	if (bitsFreeThisByte < inBitCount)
	{
		//we need another byte
		outData |= static_cast<UInt8>(mBuffer[byteOffset + 1]) << bitsFreeThisByte;
	}

	//don't forget a mask so that we only read the bit we wanted...
	outData &= (~(0x00ff << inBitCount));

	mBitHead += inBitCount;
}

void InputMemoryBitStream::ReadBits(void* outData, UInt32 inBitCount)
{
	Byte* destByte = reinterpret_cast<Byte*>(outData);
	// zero out all bytes
	memset(destByte, 0, sizeof(destByte));

	//write all the bytes
	while (inBitCount > 8)
	{
		ReadBitsImpl(*destByte, 8);
		++destByte;
		inBitCount -= 8;
	}
	//write anything left
	if (inBitCount > 0)
	{
		ReadBitsImpl(*destByte, inBitCount);
	}
}


void InputMemoryBitStream::Read(glm::vec3& outVector)
{
	Read(outVector.x);
	Read(outVector.y);
	Read(outVector.z);
}

void InputMemoryBitStream::Read(glm::quat& outQuat)
{
	float precision = (2.f / 65535.f);

	UInt32 f = 0;

	Read(f, 16);
	outQuat.x = ConvertFromFixed(f, -1.0f, precision);
	Read(f, 16);
	outQuat.y = ConvertFromFixed(f, -1.0f, precision);
	Read(f, 16);
	outQuat.z = ConvertFromFixed(f, -1.0f, precision);

	outQuat.w = sqrtf(1.f -	outQuat.x * outQuat.x - outQuat.y * outQuat.y - outQuat.z * outQuat.z);
	bool isNegative;
	Read(isNegative);

	if (isNegative)
		outQuat.w *= -1.0f;
}

void InputMemoryBitStream::HexDump() const
{
	UInt32 bufferSize = mBitCapacity >> 3;
	printf("Buffer len: %i\n", bufferSize);
	for (int i = 0; i < bufferSize; ++i)
	{
		printf("%02hhX ", mBuffer[i]);
	}
	printf("\n");
}
