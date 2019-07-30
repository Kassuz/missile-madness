#pragma once

#include "Networking/SharedRPC.h"

// RPC unwrap template
// void name(InputMemoryBitStream& packetToReadFrom, bool shouldBeExecuted)

// Because of the rpc ACK system, same rpc can be recieved multiple times, but should be excecuted only once
// RPC should always be read from the packet, so the read head can be advanced.
// RPC system makes sure, that execute bool is correctly passed

namespace RPCUnwrap
{
	// ID: DCUS
	void UnwrapUserDisconnect(InputMemoryBitStream& packet, bool execute);

	// ID: DNGO
	void UnwrapDestroyNetworkedGameObject(InputMemoryBitStream& packet, bool execute);

	// ID: SNGA
	void UnwrapSetNGOBJActive(InputMemoryBitStream& packet, bool execute);
}