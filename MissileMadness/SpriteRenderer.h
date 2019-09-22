//--------------------------------------------------------------------------------------------
// Modified from examples at LearnOpenGL (learnopengl.com) by Joy de Vries https://twitter.com/JoeyDeVriez
// https://learnopengl.com/code_viewer.php?code=in-practice/breakout/sprite_renderer.h
// Licenced under CC BY-NC 4.0 https://creativecommons.org/licenses/by-nc/4.0/
//--------------------------------------------------------------------------------------------

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

	void Init(glm::mat4 projectionMatrix);
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

