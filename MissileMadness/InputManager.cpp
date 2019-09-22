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
			m_Keys[key] = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			m_Keys[key] = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			m_Keys[key] = KeyState::KEY_RELEASED;
	}
}

void InputManager::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_MouseX = (float)xpos;
	m_MouseY = (float)ypos;
}

void InputManager::MouseButtonCallback(GLFWwindow* window, Int32 button, Int32 action, Int32 mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			m_MouseRight = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			m_MouseRight = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			m_MouseRight = KeyState::KEY_RELEASED;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			m_MouseLeft = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			m_MouseLeft = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			m_MouseLeft = KeyState::KEY_RELEASED;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if (action == GLFW_PRESS)
			m_MouseMiddle = KeyState::KEY_PRESSED;
		else if (action == GLFW_REPEAT)
			m_MouseMiddle = KeyState::KEY_HELD;
		else if (action == GLFW_RELEASE)
			m_MouseMiddle = KeyState::KEY_RELEASED;
	}
}

void InputManager::Update()
{
	for (Int32 i = 0; i < 1024; ++i)
	{
		if (m_Keys[i] == KeyState::KEY_RELEASED)
			m_Keys[i] = KeyState::KEY_UP;
		else if (m_Keys[i] == KeyState::KEY_PRESSED)
			m_Keys[i] = KeyState::KEY_HELD;
	}

	if (m_MouseRight == KeyState::KEY_RELEASED)
		m_MouseRight = KeyState::KEY_UP;
	else if (m_MouseRight == KeyState::KEY_PRESSED)
		m_MouseRight = KeyState::KEY_HELD;

	if (m_MouseLeft == KeyState::KEY_RELEASED)
		m_MouseLeft = KeyState::KEY_UP;
	else if (m_MouseLeft == KeyState::KEY_PRESSED)
		m_MouseLeft = KeyState::KEY_HELD;

	if (m_MouseMiddle == KeyState::KEY_RELEASED)
		m_MouseMiddle = KeyState::KEY_UP;
	else if (m_MouseMiddle == KeyState::KEY_PRESSED)
		m_MouseMiddle = KeyState::KEY_HELD;
}

bool InputManager::GetKey(Int32 key)
{
	return m_Keys[key] == KeyState::KEY_PRESSED || m_Keys[key] == KeyState::KEY_HELD;
}

bool InputManager::GetKeyDown(Int32 key)
{
	return m_Keys[key] == KeyState::KEY_PRESSED;
}

bool InputManager::GetKeyReleased(Int32 key)
{
	return m_Keys[key] == KeyState::KEY_RELEASED;
}

bool InputManager::GetMouseButton(Int32 button)
{
	switch (button)
	{
		case GLFW_MOUSE_BUTTON_RIGHT:
			return m_MouseRight == KeyState::KEY_PRESSED || m_MouseRight == KeyState::KEY_HELD;
		case GLFW_MOUSE_BUTTON_LEFT:
			return m_MouseLeft == KeyState::KEY_PRESSED || m_MouseLeft == KeyState::KEY_HELD;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return m_MouseMiddle == KeyState::KEY_PRESSED || m_MouseMiddle == KeyState::KEY_HELD;
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
			return m_MouseRight == KeyState::KEY_PRESSED;
		case GLFW_MOUSE_BUTTON_LEFT:
			return m_MouseLeft == KeyState::KEY_PRESSED;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return m_MouseMiddle == KeyState::KEY_PRESSED;
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
			return m_MouseRight == KeyState::KEY_RELEASED;
		case GLFW_MOUSE_BUTTON_LEFT:
			return m_MouseLeft == KeyState::KEY_RELEASED;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return m_MouseMiddle == KeyState::KEY_RELEASED;
		default:
			Debug::LogWarning("No such mouse button!");
			return false;
	}
}

InputManager::KeyState InputManager::GetKeyState(Int32 key)
{
	return m_Keys[key];
}

InputManager::KeyState InputManager::GetMouseState(Int32 button)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_RIGHT:
		return m_MouseRight;
	case GLFW_MOUSE_BUTTON_LEFT:
		return m_MouseLeft;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		return m_MouseMiddle;
	default:
		Debug::LogErrorFormat("No such mouse button, %i!", button);
		return KeyState::KEY_UP;
	}
}

glm::vec2 InputManager::GetMousePos()
{
	return glm::vec2(m_MouseX, m_MouseY);
}

InputManager::InputManager() : m_MouseX(0.0f), m_MouseY(0.0f), m_MouseRight(KeyState::KEY_UP), m_MouseLeft(KeyState::KEY_UP), m_MouseMiddle(KeyState::KEY_UP)
{
	for (Int32 i = 0; i < 1024; ++i)
	{
		m_Keys[i] = KeyState::KEY_UP;
	}
}


InputManager::~InputManager()
{
}