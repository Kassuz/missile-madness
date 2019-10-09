#include "InputManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Engine.h"

//InputManager& InputManager::Instance()
//{
//	static InputManager instance;
//	return instance;
//}

InputManager::KeyState InputManager::s_Keys[1024];

float InputManager::s_MouseX = 0.0f;
float InputManager::s_MouseY = 0.0f;
InputManager::KeyState InputManager::s_MouseRight  = InputManager::KeyState::KEY_UP;
InputManager::KeyState InputManager::s_MouseLeft   = InputManager::KeyState::KEY_UP;
InputManager::KeyState InputManager::s_MouseMiddle = InputManager::KeyState::KEY_UP;

void InputManager::Init()
{
	for (Int32 i = 0; i < 1024; ++i)
	{
		s_Keys[i] = KeyState::KEY_UP;
	}
}

void InputManager::KeyCallback(GLFWwindow* window, Int32 key, Int32 scancode, Int32 action, Int32 mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			s_Keys[key] = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			s_Keys[key] = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			s_Keys[key] = KeyState::KEY_RELEASED;
	}
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	s_MouseX = (float)xpos;
	s_MouseY = (float)ypos;
}

void InputManager::MouseButtonCallback(GLFWwindow* window, Int32 button, Int32 action, Int32 mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			s_MouseRight = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			s_MouseRight = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			s_MouseRight = KeyState::KEY_RELEASED;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			s_MouseLeft = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			s_MouseLeft = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			s_MouseLeft = KeyState::KEY_RELEASED;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if (action == GLFW_PRESS)
			s_MouseMiddle = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			s_MouseMiddle = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			s_MouseMiddle = KeyState::KEY_RELEASED;
	}
}

void InputManager::Update()
{
	for (Int32 i = 0; i < 1024; ++i)
	{
		if (s_Keys[i] == KeyState::KEY_RELEASED)
			s_Keys[i] = KeyState::KEY_UP;
		else if (s_Keys[i] == KeyState::KEY_PRESSED)
			s_Keys[i] = KeyState::KEY_HELD;
	}

	if (s_MouseRight == KeyState::KEY_RELEASED)
		s_MouseRight = KeyState::KEY_UP;
	else if (s_MouseRight == KeyState::KEY_PRESSED)
		s_MouseRight = KeyState::KEY_HELD;

	if (s_MouseLeft == KeyState::KEY_RELEASED)
		s_MouseLeft = KeyState::KEY_UP;
	else if (s_MouseLeft == KeyState::KEY_PRESSED)
		s_MouseLeft = KeyState::KEY_HELD;

	if (s_MouseMiddle == KeyState::KEY_RELEASED)
		s_MouseMiddle = KeyState::KEY_UP;
	else if (s_MouseMiddle == KeyState::KEY_PRESSED)
		s_MouseMiddle = KeyState::KEY_HELD;
}

bool InputManager::GetKey(Int32 key)
{
	return s_Keys[key] == KeyState::KEY_PRESSED || s_Keys[key] == KeyState::KEY_HELD;
}

bool InputManager::GetKeyDown(Int32 key)
{
	return s_Keys[key] == KeyState::KEY_PRESSED;
}

bool InputManager::GetKeyReleased(Int32 key)
{
	return s_Keys[key] == KeyState::KEY_RELEASED;
}

bool InputManager::GetMouseButton(Int32 button)
{
	switch (button)
	{
		case GLFW_MOUSE_BUTTON_RIGHT:
			return s_MouseRight == KeyState::KEY_PRESSED || s_MouseRight == KeyState::KEY_HELD;
		case GLFW_MOUSE_BUTTON_LEFT:
			return s_MouseLeft == KeyState::KEY_PRESSED || s_MouseLeft == KeyState::KEY_HELD;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return s_MouseMiddle == KeyState::KEY_PRESSED || s_MouseMiddle == KeyState::KEY_HELD;
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
			return s_MouseRight == KeyState::KEY_PRESSED;
		case GLFW_MOUSE_BUTTON_LEFT:
			return s_MouseLeft == KeyState::KEY_PRESSED;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return s_MouseMiddle == KeyState::KEY_PRESSED;
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
			return s_MouseRight == KeyState::KEY_RELEASED;
		case GLFW_MOUSE_BUTTON_LEFT:
			return s_MouseLeft == KeyState::KEY_RELEASED;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return s_MouseMiddle == KeyState::KEY_RELEASED;
		default:
			Debug::LogWarning("No such mouse button!");
			return false;
	}
}

InputManager::KeyState InputManager::GetKeyState(Int32 key)
{
	return s_Keys[key];
}

InputManager::KeyState InputManager::GetMouseState(Int32 button)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_RIGHT:
		return s_MouseRight;
	case GLFW_MOUSE_BUTTON_LEFT:
		return s_MouseLeft;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		return s_MouseMiddle;
	default:
		Debug::LogErrorFormat("No such mouse button, %i!", button);
		return KeyState::KEY_UP;
	}
}

glm::vec2 InputManager::GetMousePos()
{
	return glm::vec2(s_MouseX, s_MouseY);
}

//InputManager::InputManager() : s_MouseX(0.0f), s_MouseY(0.0f), s_MouseRight(KeyState::KEY_UP), s_MouseLeft(KeyState::KEY_UP), s_MouseMiddle(KeyState::KEY_UP)
//{
//	for (Int32 i = 0; i < 1024; ++i)
//	{
//		s_Keys[i] = KeyState::KEY_UP;
//	}
//}


InputManager::~InputManager()
{
}