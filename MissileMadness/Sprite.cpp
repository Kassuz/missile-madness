#include "Sprite.h"

#include "Engine.h"


Sprite::Sprite(Texture2D* texture, Color color, Transform* transform, Int32 renderLayer) 
	: texture(texture), spriteColor(color), transform(transform), renderLayer(renderLayer)
{
	SpriteRenderer::Instance().RegisterSprite(this);
}


Sprite::~Sprite()
{
	SpriteRenderer::Instance().UnregisterSprite(this);
}
