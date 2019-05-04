#include "Sprite.h"

#include "Engine.h"


Sprite::Sprite(Texture2D* texture, Color color, GameObject* gameObject, Int32 renderLayer) 
	: texture(texture), spriteColor(color), gameObject(gameObject), renderLayer(renderLayer)
{
	SpriteRenderer::Instance().RegisterSprite(this);
}


Sprite::~Sprite()
{
	SpriteRenderer::Instance().UnregisterSprite(this);
}
