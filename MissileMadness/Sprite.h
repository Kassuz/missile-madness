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

	Texture2D* GetTexture() const { return m_Texture; }
	Color GetColor() const { return m_SpriteColor; }
	GameObject* GetGameObject() const { return m_GameObject; }
	Int32 GetRenderLayer() const { return m_RenderLayer; }

	void SetColor(Color c) { m_SpriteColor = c; }
	void SetRenderLayer(Int32 layer) { m_RenderLayer = layer; }

private:
	Texture2D* m_Texture;
	Color m_SpriteColor;
	GameObject* m_GameObject;
	Int32 m_RenderLayer;
};

