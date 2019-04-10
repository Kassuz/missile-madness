#pragma once

class Time
{
public:
	static float deltaTime;
	static float lastFrame;
	static unsigned int frameCount;

	static void Update();
	
	static float GetTime();

private:
	Time() = delete;
	~Time() = delete;

	Time(const Time& other) = delete;
	Time& operator=(const Time& other) = delete;
};

