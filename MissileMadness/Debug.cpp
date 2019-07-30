#include "Debug.h"

#include <Windows.h>
#include <iostream>

#include <glad/glad.h>

#include "Color.h"
#include "Shader.h"
#include "Types.h"
#include "MMTime.h"

HANDLE Debug::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//Shader Debug::debugShader = Shader("../basic.vert", "../basic.frag");

void Debug::Log(const std::string& text)
{
#ifdef _DEBUG
	std::cout << "LOG   >> " << Time::GetTime() << " >> " << text << std::endl;
#endif // _DEBUG
}

void Debug::LogFormat(const char* inFormat, ...)
{
#ifdef _DEBUG
	static char buff[4096];

	va_list args;
	va_start(args, inFormat);

	_vsnprintf_s(buff, 4096, 4096, inFormat, args);

	std::cout  << "LOG   >> " << Time::GetTime() << " >> " << buff << std::endl;
#endif // _DEBUG
}

void Debug::LogError(const std::string& text)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, 12);
	std::cout << "ERROR >> ";
	SetConsoleTextAttribute(hConsole, 7);
	std::cout << Time::GetTime() << " >> " << text << std::endl;
#endif // _DEBUG
}

void Debug::LogErrorFormat(const char* inFormat, ...)
{
#ifdef _DEBUG
	static char buff[4096];

	va_list args;
	va_start(args, inFormat);

	_vsnprintf_s(buff, 4096, 4096, inFormat, args);

	SetConsoleTextAttribute(hConsole, 12);
	std::cout << "ERROR >> ";
	SetConsoleTextAttribute(hConsole, 7);
	std::cout << Time::GetTime() << " >> " << buff << std::endl;
#endif // _DEBUG
}

void Debug::LogWarning(const std::string& text)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(hConsole, 14);
	std::cout << "WARN  >> ";
	SetConsoleTextAttribute(hConsole, 7);
	std::cout << Time::GetTime() << " >> " << text << std::endl;
#endif // _DEBUG
}

void Debug::LogWarningFormat(const char* inFormat, ...)
{
#ifdef _DEBUG
	static char buff[4096];

	va_list args;
	va_start(args, inFormat);

	_vsnprintf_s(buff, 4096, 4096, inFormat, args);

	SetConsoleTextAttribute(hConsole, 14);
	std::cout << "WARN  >> ";
	SetConsoleTextAttribute(hConsole, 7);
	std::cout << Time::GetTime() << " >> " << buff << std::endl;
#endif // _DEBUG
}
