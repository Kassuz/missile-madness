#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>

#include "Types.h"
#include "Color.h"

//class Window;

class Window
{
public:
	UInt32 GetWindowWidth()  const { return m_WindowWidth;  }
	UInt32 GetWindowHeight() const { return m_WindowHeight; }

	GLFWwindow* GetWindowPtr() const { return m_WindowPtr; }
	bool WindowShouldClose() const { return glfwWindowShouldClose(m_WindowPtr); }
	void CloseWindow() { glfwSetWindowShouldClose(m_WindowPtr, GL_TRUE); }

	void SetClearColor(Color clearColor) { m_ClearColor = clearColor; }
	void ClearWindow();
	void SwapBuffers() { glfwSwapBuffers(m_WindowPtr); }
	void PollEvents() {	glfwPollEvents(); }

	static std::shared_ptr<Window> CreateNewWindow(UInt32 width, UInt32 height, std::string name);

	~Window();

private:
	Window(UInt32 width, UInt32 height, GLFWwindow* window) : m_WindowWidth(width), m_WindowHeight(height), m_WindowPtr(window)
	{ }

	UInt32 m_WindowWidth;
	UInt32 m_WindowHeight;

	GLFWwindow* m_WindowPtr;

	Color m_ClearColor = Color(0.35, 0.35, 0.35, 1.0f);

	static std::shared_ptr<Window> s_WindowInstance;
};

typedef std::shared_ptr<Window> WindowPtr;
