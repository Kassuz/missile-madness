#pragma once

#include <cstdlib>
#include <string>
#include "../Types.h"

#include <glm/glm.hpp>

#include "../Debug.h"

class GameObject;

inline UInt32 ConvertToFixed(float inNumber, float inMin, float inPrecision)
{
	return static_cast<UInt32> ((inNumber - inMin) / inPrecision);
}

inline float ConvertFromFixed(UInt32 inNumber, float inMin, float inPrecision)
{
	return static_cast<float>(inNumber)* inPrecision + inMin;
}

// Some template metaprogramming black magic to get required bits to serialize enum
template<int tValue, int tBits>
struct GetRequiredBitsHelper
{
	enum { Value = GetRequiredBitsHelper<(tValue >> 1), tBits + 1>::Value };
};
template<int tBits>
struct GetRequiredBitsHelper<0, tBits>
{
	enum { Value = tBits };
};
template<int tValue>
struct GetRequiredBits
{
	enum { Value = GetRequiredBitsHelper<tValue, 0>::Value };
};


class OutputMemoryBitStream
{
public:

	OutputMemoryBitStream() : mBitHead(0), mBuffer(nullptr)
	{
		ReallocBuffer(1500 * 8);
	}

	OutputMemoryBitStream(const OutputMemoryBitStream& other)
	{
		mBuffer = nullptr;
		ReallocBuffer(1500 * 8);
		memcpy(mBuffer, other.mBuffer, other.GetByteLength());

		mBitHead = other.mBitHead;
		mBitCapacity = other.mBitCapacity;
	}

	~OutputMemoryBitStream() { std::free(mBuffer); }

	void WriteBits(Byte inData, UInt32 inBitCount);
	void WriteBits(const void* inData, UInt32 inBitCount);

	const Byte* GetBufferPtr() const { return mBuffer; }
	UInt32 GetBitLength() const { return mBitHead; }
	UInt32 GetByteLength() const { return (mBitHead + 7) >> 3; }

	void WriteBytes(const void* inData, UInt32 inByteCount) { WriteBits(inData, inByteCount << 3); }

	template< typename T >
	void Write(T inData, UInt32 inBitCount = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");
		WriteBits(&inData, inBitCount);
	}

	void Write(bool inData) { WriteBits(&inData, 1); }

	void Write(const glm::vec3& inVector);
	void Write(const glm::quat& inQuat);

	void Write(const std::string& inString)
	{
		UInt32 elementCount = static_cast<UInt32>(inString.size());
		Write(elementCount);
		for (const auto& element : inString)
		{
			Write(element);
		}
	}

	void HexDump() const;

private:
	void ReallocBuffer(UInt32 inNewBitCapacity);

	Byte* mBuffer;
	UInt32 mBitHead;
	UInt32 mBitCapacity;
};

class InputMemoryBitStream
{
public:

	InputMemoryBitStream(Byte* inBuffer, UInt32 inBitCount) :
		mBuffer(inBuffer),
		mBitCapacity(inBitCount),
		mBitHead(0),
		mIsBufferOwner(false) {}

	InputMemoryBitStream(const InputMemoryBitStream& inOther) :
		mBitCapacity(inOther.mBitCapacity),
		mBitHead(inOther.mBitHead),
		mIsBufferOwner(true)
	{
		//allocate buffer of right size
		int byteCount = mBitCapacity / 8;
		mBuffer = static_cast<Byte*>(malloc(byteCount));
		//copy
		memcpy(mBuffer, inOther.mBuffer, byteCount);
	}

	~InputMemoryBitStream() { if (mIsBufferOwner) { free(mBuffer); } }

	const Byte* GetBufferPtr() const { return mBuffer; }
	UInt32 GetRemainingBitCount() const { return mBitCapacity - mBitHead; }

	void ReadBits(void* outData, UInt32 inBitCount);

	void ReadBytes(void* outData, UInt32 inByteCount) { ReadBits(outData, inByteCount << 3); }

	template<typename T>
	void Read(T& inData, UInt32 inBitCount = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Read only supports primitive data types");
		ReadBits(&inData, inBitCount);
	}

	// Hacky hack. Value is read to UInt32, because converting bool to void* increases it's size and memset doesn't work properly
	void Read(bool& outData) { UInt32 temp; ReadBits(&temp, 1); outData = static_cast<bool>(temp); }

	void Read(glm::quat& outQuat);
	void Read(glm::vec3& inVector);

	void ResetToCapacity(UInt32 inByteCapacity) { mBitCapacity = inByteCapacity << 3; mBitHead = 0; }

	void Read(std::string& inString)
	{
		UInt32 elementCount;
		Read(elementCount);
		inString.resize(elementCount);
		for (auto& element : inString)
		{
			Read(element);
		}
	}

	void HexDump() const;
	void PrintStats() const 
	{
		printf("Stats for buffer:\n\tReadHead: %u\n\tBitCapasity: %u\n", mBitHead, mBitCapacity);
	}

private:
	Byte* mBuffer;
	UInt32 mBitHead;
	UInt32 mBitCapacity;
	bool mIsBufferOwner;

	void ReadBitsImpl(Byte& outData, UInt32 inBitCount);
};