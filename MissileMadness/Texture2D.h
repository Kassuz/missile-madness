#pragma once

#include "Types.h"

class Texture2D
{
public:
	Texture2D();
	~Texture2D();

	UInt32 ID;
	UInt32 width, height;

	void Generate(UInt32 width, UInt32 height, UInt8* data);
	void Use();
};

