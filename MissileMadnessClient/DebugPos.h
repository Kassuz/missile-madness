#pragma once

#include "GameObject.h"
#include "Sprite.h"

class Texture2D;

class DebugPos : public GameObject
{
public:
	DebugPos(Texture2D* texture)
	{
		m_DebugSprite = new Sprite(texture, Color::White(), this, 1);
		m_Transform.SetScale(glm::vec3(30.0f));
	}
	~DebugPos() { delete m_DebugSprite; }
private:
	Sprite* m_DebugSprite;
};