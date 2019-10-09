#include "Window.h"

#include "Debug.h"
#include "InputManager.h"


WindowPtr Window::s_WindowInstance = nullptr;

void Window::ClearWindow()
{
	glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

WindowPtr Window::CreateNewWindow(UInt32 width, UInt32 height, std::string name)
{
	if (s_WindowInstance != nullptr)
	{
		Debug::LogError("Window already created");
		return s_WindowInstance;
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

	if (window == NULL)
	{
		Debug::LogError("Failed to create window!");
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, InputManager::MouseCallback);
	glfwSetKeyCallback(window, InputManager::KeyCallback);
	glfwSetMouseButtonCallback(window, InputManager::MouseButtonCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		Debug::LogError("Failed to initialize GLAD/OpenGL");
		glfwTerminate();
		return nullptr;
	}

	glViewport(0, 0, width, height);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	s_WindowInstance = WindowPtr(new Window(width, height, window));
	return s_WindowInstance;
}


Window::~Window()
{
	glfwTerminate();
}
