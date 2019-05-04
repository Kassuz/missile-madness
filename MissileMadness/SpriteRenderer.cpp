#include "SpriteRenderer.h"

#include <glad/glad.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine.h"

SpriteRenderer& SpriteRenderer::Instance()
{
	static SpriteRenderer instance;
	return instance;
}


SpriteRenderer::SpriteRenderer() : spriteShader(nullptr), quadVAO(0)
{
}


SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::Init(Shader* spriteShader, glm::mat4 projectionMatrix)
{
	this->spriteShader = spriteShader;
	this->projectionMatrix = projectionMatrix;

	GLuint VBO;
	GLfloat vertices[] = {
		// Pos      // Tex
	   -0.5f,  0.5f, 0.0f, 1.0f,
	   -0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 0.0f,

		0.5f,  0.5f, 1.0f, 1.0f,
	   -0.5f,  0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::RenderSprites()
{
	// Sort all sprites based on their render order
	std::sort(sprites.begin(), sprites.end(), 
		[](Sprite* a, Sprite* b) -> bool
		{
			return a->GetRenderLayer() < b->GetRenderLayer();
		}
	);

	// Render all sprites
	for (auto it : sprites)
	{
		RenderIndividualSprite(it);
	}
}

void SpriteRenderer::RegisterSprite(Sprite* sprite)
{
	sprites.push_back(sprite);
}

void SpriteRenderer::UnregisterSprite(Sprite* sprite)
{
	auto it = std::find(sprites.cbegin(), sprites.cend(), sprite);
	if (it != sprites.cend())
		sprites.erase(it);
}

void SpriteRenderer::RenderIndividualSprite(Sprite* sprite)
{
	if (!sprite->GetGameObject()->IsActive()) return;

	this->spriteShader->Use();

	glm::mat4 model = sprite->GetGameObject()->GetTransform()->GetModel();

	this->spriteShader->SetMat4("model", model);
	this->spriteShader->SetMat4("projection", projectionMatrix);
	this->spriteShader->SetVec4("spriteColor", sprite->GetColor());

	glActiveTexture(GL_TEXTURE0);
	sprite->GetTexture()->Use();

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}