#include <iostream>

#include "Engine.h"
#include <Windows.h>

int main(int argc, char* argv[])
{
#ifdef _SERVER

	std::cout << "Server working!" << std::endl;
	Time::GetTime();
	for (int i = 0; i < 10.0f; i++)
	{
		Time::Update();
		Debug::Log("<< That's time!");
		Sleep(200);
	}
	system("pause");
#endif // _SERVER

	return 0;
}