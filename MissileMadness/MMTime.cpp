#include "MMTime.h"
#include "Debug.h"

float Time::deltaTime = 0.1f;
float Time::lastFrame = 0.0f;
float Time::minDelta = 0.0f;

const std::chrono::time_point<std::chrono::steady_clock> Time::StartTime = std::chrono::high_resolution_clock::now();

unsigned int Time::frameCount = 0;


void Time::Update()
{
	float currentTime = GetTime();
	float delta = currentTime - lastFrame;
	
	// Not proper fixed timestep, but good enough
	while (delta < minDelta)
	{
		currentTime = GetTime();
		delta = currentTime - lastFrame;
	}

	deltaTime = delta;
	lastFrame = currentTime;

	frameCount++;
}

// TODO: Maybe cache this at Time::Update
float Time::GetTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> seconds = currentTime - StartTime;

	return seconds.count();
}

void Time::SetFixedTimeStep(unsigned int framerate)
{
	minDelta = 1.0f / (float)framerate;
}