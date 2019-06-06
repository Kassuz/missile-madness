#include "MMTime.h"

float Time::deltaTime = 0.1f;
float Time::lastFrame = 0.0f;

const std::chrono::time_point<std::chrono::steady_clock> Time::StartTime = std::chrono::high_resolution_clock::now();

unsigned int Time::frameCount = 0;

void Time::Update()
{
	float currentTime = GetTime();
	deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	frameCount++;
}

float Time::GetTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> seconds = currentTime - StartTime;

	return seconds.count();
}