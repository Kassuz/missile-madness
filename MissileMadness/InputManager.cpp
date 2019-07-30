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
			keys[key] = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			keys[key] = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			keys[key] = KeyState::KEY_RELEASED;
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
			mouseRight = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			mouseRight = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			mouseRight = KeyState::KEY_RELEASED;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			mouseLeft = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			mouseLeft = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			mouseLeft = KeyState::KEY_RELEASED;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if (action == GLFW_PRESS)
			mouseMiddle = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			mouseMiddle = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			mouseMiddle = KeyState::KEY_RELEASED;
	}
}

void InputManager::Update()
{
	for (Int32 i = 0; i < 1024; ++i)
	{
		if (keys[i] == KeyState::KEY_RELEASED)
			keys[i] = KeyState::KEY_UP;
		else if (keys[i] == KeyState::KEY_PRESSED)
			keys[i] = KeyState::KEY_HELD;
	}

	if (mouseRight == KeyState::KEY_RELEASED)
		mouseRight = KeyState::KEY_UP;
	else if (mouseRight == KeyState::KEY_PRESSED)
		mouseRight = KeyState::KEY_HELD;

	if (mouseLeft == KeyState::KEY_RELEASED)
		mouseLeft = KeyState::KEY_UP;
	else if (mouseLeft == KeyState::KEY_PRESSED)
		mouseLeft = KeyState::KEY_HELD;

	if (mouseMiddle == KeyState::KEY_RELEASED)
		mouseMiddle = KeyState::KEY_UP;
	else if (mouseMiddle == KeyState::KEY_PRESSED)
		mouseMiddle = KeyState::KEY_HELD;
}

bool InputManager::GetKey(Int32 key)
{
	return keys[key] == KeyState::KEY_PRESSED || keys[key] == KeyState::KEY_HELD;
}

bool InputManager::GetKeyDown(Int32 key)
{
	return keys[key] == KeyState::KEY_PRESSED;
}

bool InputManager::GetKeyReleased(Int32 key)
{
	return keys[key] == KeyState::KEY_RELEASED;
}

bool InputManager::GetMouseButton(Int32 button)
{
	switch (button)
	{
		case GLFW_MOUSE_BUTTON_RIGHT:
			return mouseRight == KeyState::KEY_PRESSED || mouseRight == KeyState::KEY_HELD;
		case GLFW_MOUSE_BUTTON_LEFT:
			return mouseLeft == KeyState::KEY_PRESSED || mouseLeft == KeyState::KEY_HELD;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return mouseMiddle == KeyState::KEY_PRESSED || mouseMiddle == KeyState::KEY_HELD;
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
			return mouseRight == KeyState::KEY_PRESSED;
		case GLFW_MOUSE_BUTTON_LEFT:
			return mouseLeft == KeyState::KEY_PRESSED;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return mouseMiddle == KeyState::KEY_PRESSED;
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
			return mouseRight == KeyState::KEY_RELEASED;
		case GLFW_MOUSE_BUTTON_LEFT:
			return mouseLeft == KeyState::KEY_RELEASED;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return mouseMiddle == KeyState::KEY_RELEASED;
		default:
			Debug::LogWarning("No such mouse button!");
			return false;
	}
}

InputManager::KeyState InputManager::GetKeyState(Int32 key)
{
	return keys[key];
}

InputManager::KeyState InputManager::GetMouseState(Int32 button)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_RIGHT:
		return mouseRight;
	case GLFW_MOUSE_BUTTON_LEFT:
		return mouseLeft;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		return mouseMiddle;
	default:
		Debug::LogErrorFormat("No such mouse button, %i!", button);
		return KeyState::KEY_UP;
	}
}

glm::vec2 InputManager::GetMousePos()
{
	return glm::vec2(mouseX, mouseY);
}

InputManager::InputManager() : mouseX(0.0f), mouseY(0.0f), mouseRight(KeyState::KEY_UP), mouseLeft(KeyState::KEY_UP), mouseMiddle(KeyState::KEY_UP)
{
	for (Int32 i = 0; i < 1024; ++i)
	{
		keys[i] = KeyState::KEY_UP;
	}
}


InputManager::~InputManager()
{
}