#include "Time.h"

#include <GLFW/glfw3.h>

float Time::deltaTime = 0.1f;
float Time::lastFrame = 0.0f;

unsigned int Time::frameCount = 0;

void Time::Update()
{
	float currentTime = glfwGetTime();
	deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	frameCount++;
}

float Time::GetTime()
{
	return (float) glfwGetTime();
}
