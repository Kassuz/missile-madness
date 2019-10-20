#pragma once

#include <chrono>
#include <ctime>
#include <string>

#include "Types.h"

class Time
{
public:
	static float deltaTime;
	static float lastFrame;
	static UInt32 frameCount;

	static void Update();
	
	static float GetTime();
	static float GetRealTime();
	
	static void SetFixedTimeStep(unsigned int framerate);

	static UInt64 GetUnixTimeStamp(const std::time_t* t = nullptr);

	static std::string FormatSeconds(float seconds);

	Time() = delete;
	~Time() { }
	Time(const Time& other) = delete;
	Time& operator=(const Time& other) = delete;

private:

	static const std::chrono::time_point<std::chrono::high_resolution_clock> StartTime;
	static float minDelta;
};

