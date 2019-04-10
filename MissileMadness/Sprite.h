#pragma once

#include "Types.h"
#include "Color.h"

class Texture2D;
class Transform;
struct Color;

class Sprite
{
public:
	Sprite(Texture2D* texture, Color color, Transform* transform, Int32 renderLayer);
	~Sprite();

	Texture2D* texture;
	Color spriteColor;
	Transform* transform;
	Int32 renderLayer;
};

