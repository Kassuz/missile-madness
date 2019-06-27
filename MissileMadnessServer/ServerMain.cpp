#include <iostream>

#include "Engine.h"
#include "NetworkManagerServer.h"

#include "Networking/TestObj.h"

int main(int argc, char* argv[])
{
	std::cout << "Server started" << std::endl;

	NetworkManagerServer::Instance().Initialize();

	Time::SetFixedTimeStep(30U);

	TestObj* test = NetworkManagerServer::Instance().CreateNetworkedGameObject<TestObj>();
	

	while (true)
	{
		Time::Update();

		test->SetServerTime(Time::GetTime());
		test->IncreaseCounter();

		NetworkManagerServer::Instance().ProcessIncomingPackets();
		NetworkManagerServer::Instance().UpdateSendingPackets();
	}

	return 0;
}