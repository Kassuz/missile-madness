#include "ClientRPCs.h"
#include "ClientGame.h"
#include "NetworkManagerClient.h"

void RPCUnwrap::UnwrapUserDisconnect(InputMemoryBitStream& packet, bool execute)
{
	RPCParams::UserDisconnedtRPC params(0);
	params.Read(packet);

	if (execute)
	{
		Debug::LogFormat("Disconnect user %u", params.GetUser());
		ClientGame::DisconnectPlayer(params.GetUser());
	}
	else
	{
		//Debug::Log("Read rpc, but already executed...");
	}
}

void RPCUnwrap::UnwrapDestroyNetworkedGameObject(InputMemoryBitStream& packet, bool execute)
{
	RPCParams::DestroyNetworkedGameObject params(0);
	params.Read(packet);

	if (execute)
	{
		Debug::LogFormat("Destroy NetworkedGameObject with ID: %u", params.GetDestroyedObject());
		auto ngobj = NetworkManagerClient::Instance().RemoveGameObject(params.GetDestroyedObject());
		if (ngobj != nullptr)
			delete ngobj;
	}
}

void RPCUnwrap::UnwrapSetNGOBJActive(InputMemoryBitStream& packet, bool execute)
{
	RPCParams::SetNetworkedGameObjectActive params;
	params.Read(packet);

	if (execute)
	{
		auto obj = NetworkManagerClient::Instance().GetGameObject(params.GetNetworkedObject());
		if (obj != nullptr)
		{
			obj->SetActive(params.GetActiveStatus());
		}
		else
		{
			Debug::LogErrorFormat("Could not find NetworkedObject with ID: %u", params.GetNetworkedObject());
		}
	}
}
