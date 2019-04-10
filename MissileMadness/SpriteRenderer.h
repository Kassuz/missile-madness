#pragma once

#include <vector>
#include <glm/mat4x4.hpp>
#include "Types.h"

class Sprite;
class Shader;

class SpriteRenderer
{
public:
	static SpriteRenderer& Instance();

	void Init(Shader* spriteShader, glm::mat4 projectionMatrix);
	void RenderSprites();
	void RegisterSprite(Sprite* sprite);
	void UnregisterSprite(Sprite* sprite);

private:
	SpriteRenderer();
	~SpriteRenderer();

	SpriteRenderer(const SpriteRenderer&) = delete;
	SpriteRenderer& operator=(const SpriteRenderer&) = delete;


	std::vector<Sprite*> sprites;

	void RenderIndividualSprite(Sprite* sprite);

	Shader* spriteShader;
	glm::mat4 projectionMatrix;
	UInt32 quadVAO;
};

