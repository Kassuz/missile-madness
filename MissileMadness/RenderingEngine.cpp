#include "RenderingEngine.h"

#include "Engine.h"

#include <glm/gtc/matrix_transform.hpp>

WindowPtr RenderingEngine::s_Window = nullptr;

void RenderingEngine::Init(WindowPtr window, const std::string& font)
{
	s_Window = window;

	UInt32 windowWidth  = window->GetWindowWidth();
	UInt32 windowHeight = window->GetWindowHeight();

	glm::mat4 projectionMatrix = glm::ortho(-(float)windowWidth / 2.0f, (float)windowWidth / 2.0f, -(float)windowHeight / 2.0f, (float)windowHeight / 2.0f, -1.0f, 1.0f);

	SpriteRenderer::Instance().Init(projectionMatrix);
	TextRenderer::Instance().Init(windowWidth, windowHeight);
	TextRenderer::Instance().Load(font, 30);
}

void RenderingEngine::Render()
{
	if (s_Window == nullptr)
	{
		Debug::LogError("Rendering engine has not been initialized!");
		return;
	}

	s_Window->ClearWindow();

	SpriteRenderer::Instance().RenderSprites();
	TextRenderer::Instance().RenderAllText();

	s_Window->SwapBuffers();
}
