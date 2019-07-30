#pragma once

#include <string>
#include <glm/vec3.hpp>

#include "Types.h"

struct Color;
class Shader;

class Debug
{
public:
	static void Log(const std::string& text);
	static void LogWarning(const std::string& text);
	static void LogError(const std::string& text);

	static void LogFormat(const char* inFormat, ...);
	static void LogWarningFormat(const char* inFormat, ...);
	static void LogErrorFormat(const char* inFormat, ...);

	//static void DrawLine(glm::vec3 start, glm::vec3 end, Color color);
	//static void DrawCircle(glm::vec3 pos, float radius, Color color);

private:
	Debug() = delete;

	Debug(const Debug& other) = delete;
	Debug& operator=(const Debug& ohter) = delete;

	static void* hConsole;
	//static Shader debugShader;
};

inline std::string ConvertUIntToString(UInt32 value)
{
	char buffer[5];

	buffer[0] = (value >> 24) & 0xFF;
	buffer[1] = (value >> 16) & 0xFF;
	buffer[2] = (value >>  8) & 0xFF;
	buffer[3] = value & 0xFF;
	buffer[4] = '\0';

	return std::string(buffer);
}
