#pragma once

#include <glm/vec2.hpp>
#include "Types.h"

struct GLFWwindow;

class InputManager
{
public:

	enum class KeyState { KEY_UP, KEY_PRESSED, KEY_HELD, KEY_RELEASED }; 

	//static InputManager& Instance();
	static void Init();

	static void KeyCallback(GLFWwindow* window, Int32 key, Int32 scancode, Int32 action, Int32 mods);
	static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseButtonCallback(GLFWwindow* window, Int32 button, Int32 action, Int32 mods);

	static void Update();

	static bool GetKey(Int32 key);
	static bool GetKeyDown(Int32 key);
	static bool GetKeyReleased(Int32 key);

	static bool GetMouseButton(Int32 button);
	static bool GetMouseButtonDown(Int32 button);
	static bool GetMouseButtonReleased(Int32 button);

	static KeyState GetKeyState(Int32 key);
	static KeyState GetMouseState(Int32 button);

	static glm::vec2 GetMousePos();

	InputManager() = delete;
	~InputManager();
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

private:

	static KeyState s_Keys[];
	static float s_MouseX, s_MouseY;

	static KeyState s_MouseRight, s_MouseLeft, s_MouseMiddle;
};
