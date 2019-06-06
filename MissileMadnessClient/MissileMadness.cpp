#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <string>
#include <cmath>

#include "Engine.h"
#include "Player.h"
#include "Game.h"

const UInt32 SCREEN_WIDTH  = 800;
const UInt32 SCREEN_HEIGHT = 600;

void KeyCallback(GLFWwindow* window, Int32 key, Int32 scancode, Int32 action, Int32 mods);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow* window, Int32 button, Int32 action, Int32 mods);

int main(int argc, char* argv[])
{
	// ---------------------------------------
	// -     Init GLFW and GLAD (OpenGL)     -
	// ---------------------------------------

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Missile Madness!", NULL, NULL);

	if (window == NULL)
	{
		Debug::LogError("Failed to create window!");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Hide cursor and set callbacks
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Debug::LogError("Failed to initialize GLAD");
		return -1;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ---------------------------------
	// -          Init Engine          -
	// ---------------------------------

	Shader* spriteShader = ResourceManager::Instance().LoadShader("Sprite");
	glm::mat4 projectionMatrix = glm::ortho(-(float)SCREEN_WIDTH / 2.0f, (float)SCREEN_WIDTH / 2.0f, -(float)SCREEN_HEIGHT / 2.0f, (float)SCREEN_HEIGHT / 2.0f, -1.0f, 1.0f);
	SpriteRenderer::Instance().Init(spriteShader, projectionMatrix);

	TextRenderer::Instance().Init(SCREEN_WIDTH, SCREEN_HEIGHT);
	TextRenderer::Instance().Load("Resources/Fonts/Roboto-Black.ttf", 30);
	/*Transform transform;
	transform.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	transform.SetScale(glm::vec3(100.0f, 100.0f, 0.0f));
	Sprite sprite(tex, Color::White(), &transform, 0);*/

	Game game(SCREEN_WIDTH, SCREEN_HEIGHT, 2);

	// -------------------------------
	// -          Main loop          -
	// -------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// Update time
		Time::Update();

		// Update Game
		game.Update();

		// Rendering
		glClearColor(0.35f, 0.35f, 0.35f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		SpriteRenderer::Instance().RenderSprites();
		// Render all text on top
		TextRenderer::Instance().RenderAllText();

		// Swap buffers
		glfwSwapBuffers(window);

		// Update inputmanager and poll for events
		InputManager::Instance().Update();
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void KeyCallback(GLFWwindow* window, Int32 key, Int32 scancode, Int32 action, Int32 mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	InputManager::Instance().KeyCallback(window, key, scancode, action, mods);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	InputManager::Instance().MouseCallback(window, xpos, ypos);
}

void MouseButtonCallback(GLFWwindow* window, Int32 button, Int32 action, Int32 mods)
{
	InputManager::Instance().MouseButtonCallback(window, button, action, mods);
}