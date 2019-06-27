#pragma once

#include <chrono>

class Time
{
public:
	static float deltaTime;
	static float lastFrame;
	static unsigned int frameCount;

	static void Update();
	
	static float GetTime();
	static void SetFixedTimeStep(unsigned int framerate);

private:
	Time() = delete;
	~Time() = delete;

	Time(const Time& other) = delete;
	Time& operator=(const Time& other) = delete;

	static const std::chrono::time_point<std::chrono::steady_clock> StartTime;
	static float minDelta;
};

