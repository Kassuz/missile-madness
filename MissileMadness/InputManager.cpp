#include "InputManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Engine.h"

InputManager& InputManager::Instance()
{
	static InputManager instance;
	return instance;
}

void InputManager::KeyCallback(GLFWwindow* window, Int32 key, Int32 scancode, Int32 action, Int32 mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			keys[key] = KEY_HELD;
		else if (action == GLFW_RELEASE)
			keys[key] = KEY_RELEASED;
	}
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = (float)xpos;
	mouseY = (float)ypos;
}

void InputManager::MouseButtonCallback(GLFWwindow* window, Int32 button, Int32 action, Int32 mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			mouseRight = KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			mouseRight = KEY_HELD;
		else if (action == GLFW_RELEASE)
			mouseRight = KEY_RELEASED;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			mouseLeft = KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			mouseLeft = KEY_HELD;
		else if (action == GLFW_RELEASE)
			mouseLeft = KEY_RELEASED;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if (action == GLFW_PRESS)
			mouseMiddle = KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			mouseMiddle = KEY_HELD;
		else if (action == GLFW_RELEASE)
			mouseMiddle = KEY_RELEASED;
	}
}

void InputManager::Update()
{
	for (Int32 i = 0; i < 1024; ++i)
	{
		if (keys[i] == KEY_RELEASED)
			keys[i] = KEY_UP;
		else if (keys[i] == KEY_PRESSED)
			keys[i] = KEY_HELD;
	}

	if (mouseRight == KEY_RELEASED)
		mouseRight = KEY_UP;
	else if (mouseRight == KEY_PRESSED)
		mouseRight = KEY_HELD;

	if (mouseLeft == KEY_RELEASED)
		mouseLeft = KEY_UP;
	else if (mouseLeft == KEY_PRESSED)
		mouseLeft = KEY_HELD;

	if (mouseMiddle == KEY_RELEASED)
		mouseMiddle = KEY_UP;
	else if (mouseMiddle == KEY_PRESSED)
		mouseMiddle = KEY_HELD;
}

bool InputManager::GetKey(Int32 key)
{
	return keys[key] == KEY_PRESSED || keys[key] == KEY_HELD;
}

bool InputManager::GetKeyDown(Int32 key)
{
	return keys[key] == KEY_PRESSED;
}

bool InputManager::GetKeyReleased(Int32 key)
{
	return keys[key] == KEY_RELEASED;
}

bool InputManager::GetMouseButton(Int32 button)
{
	switch (button)
	{
		case GLFW_MOUSE_BUTTON_RIGHT:
			return mouseRight == KEY_PRESSED || mouseRight == KEY_HELD;
		case GLFW_MOUSE_BUTTON_LEFT:
			return mouseLeft == KEY_PRESSED || mouseLeft == KEY_HELD;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return mouseMiddle == KEY_PRESSED || mouseMiddle == KEY_HELD;
		default:
			Debug::LogWarning("No such mouse button!");
			return false;
	}
}

bool InputManager::GetMouseButtonDown(Int32 button)
{
	switch (button)
	{
		case GLFW_MOUSE_BUTTON_RIGHT:
			return mouseRight == KEY_PRESSED;
		case GLFW_MOUSE_BUTTON_LEFT:
			return mouseLeft == KEY_PRESSED;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return mouseMiddle == KEY_PRESSED;
		default:
			Debug::LogWarning("No such mouse button!");
			return false;
	}
}

bool InputManager::GetMouseButtonReleased(Int32 button)
{
	switch (button)
	{
		case GLFW_MOUSE_BUTTON_RIGHT:
			return mouseRight == KEY_RELEASED;
		case GLFW_MOUSE_BUTTON_LEFT:
			return mouseLeft == KEY_RELEASED;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return mouseMiddle == KEY_RELEASED;
		default:
			Debug::LogWarning("No such mouse button!");
			return false;
	}
}

glm::vec2 InputManager::GetMousePos()
{
	return glm::vec2(mouseX, mouseY);
}

InputManager::InputManager() : mouseX(0.0f), mouseY(0.0f), mouseRight(KEY_UP), mouseLeft(KEY_UP), mouseMiddle(KEY_UP)
{
	for (Int32 i = 0; i < 1024; ++i)
	{
		keys[i] = KEY_UP;
	}
}


InputManager::~InputManager()
{
}