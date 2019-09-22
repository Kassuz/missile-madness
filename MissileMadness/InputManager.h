#pragma once

#include <glm/vec2.hpp>
#include "Types.h"

struct GLFWwindow;

class InputManager
{
public:

	enum class KeyState { KEY_UP, KEY_PRESSED, KEY_HELD, KEY_RELEASED }; //  if more is needed, remember to modify Move-class

	static InputManager& Instance();

	void KeyCallback(GLFWwindow* window, Int32 key, Int32 scancode, Int32 action, Int32 mods);
	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	void MouseButtonCallback(GLFWwindow* window, Int32 button, Int32 action, Int32 mods);

	void Update();

	bool GetKey(Int32 key);
	bool GetKeyDown(Int32 key);
	bool GetKeyReleased(Int32 key);

	bool GetMouseButton(Int32 button);
	bool GetMouseButtonDown(Int32 button);
	bool GetMouseButtonReleased(Int32 button);

	KeyState GetKeyState(Int32 key);
	KeyState GetMouseState(Int32 button);


	glm::vec2 GetMousePos();

private:
	InputManager();
	~InputManager();
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

	KeyState m_Keys[1024];
	float m_MouseX, m_MouseY;

	KeyState m_MouseRight, m_MouseLeft, m_MouseMiddle;
};
