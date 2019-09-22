#include "Sprite.h"

#include "Engine.h"


Sprite::Sprite(Texture2D* texture, Color color, GameObject* gameObject, Int32 renderLayer) 
	: m_Texture(texture), m_SpriteColor(color), m_GameObject(gameObject), m_RenderLayer(renderLayer)
{
	SpriteRenderer::Instance().RegisterSprite(this);
}


Sprite::~Sprite()
{
	SpriteRenderer::Instance().UnregisterSprite(this);
}
