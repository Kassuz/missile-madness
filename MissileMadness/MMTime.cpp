#include "MMTime.h"
#include "Debug.h"

float Time::deltaTime = 0.1f;
float Time::lastFrame = 0.0f;
float Time::minDelta = 0.0f;

const std::chrono::time_point<std::chrono::high_resolution_clock> Time::StartTime = std::chrono::high_resolution_clock::now();

UInt32 Time::frameCount = 0;


void Time::Update()
{
	float currentTime = GetRealTime();
	float delta = currentTime - lastFrame;
	
	// Not proper fixed timestep, but good enough
	while (delta < minDelta)
	{
		currentTime = GetRealTime();
		delta = currentTime - lastFrame;
	}

	deltaTime = delta;
	lastFrame = currentTime;

	frameCount++;
}

// TODO: Maybe cache this at Time::Update
float Time::GetTime()
{
	return lastFrame;
}

float Time::GetRealTime()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> seconds = currentTime - StartTime;

	return seconds.count();
}

void Time::SetFixedTimeStep(unsigned int framerate)
{
	minDelta = 1.0f / (float)framerate;
}

UInt64 Time::GetUnixTimeStamp(const std::time_t* t)
{
	//if specific time is not passed then get current time
	std::time_t st = t == nullptr ? std::time(nullptr) : *t;
	auto secs = static_cast<std::chrono::seconds>(st).count();
	return static_cast<UInt64>(secs);
}

std::string Time::FormatSeconds(float seconds)
{
	Int32 min = static_cast<Int32>(seconds / 60.0f);
	Int32 sec = static_cast<Int32>(seconds - (min * 60.0f));

	return std::to_string(min) + "min " + std::to_string(sec) + "s";
}
