#pragma once

#include "Types.h"
#include "Color.h"

class Texture2D;
class GameObject;
struct Color;

class Sprite
{
public:
	Sprite(Texture2D* texture, Color color, GameObject* gameObject, Int32 renderLayer);
	~Sprite();

	Texture2D* GetTexture() const { return texture; }
	Color GetColor() const { return spriteColor; }
	GameObject* GetGameObject() const { return gameObject; }
	Int32 GetRenderLayer() const { return renderLayer; }

private:
	Texture2D* texture;
	Color spriteColor;
	GameObject* gameObject;
	Int32 renderLayer;
};

