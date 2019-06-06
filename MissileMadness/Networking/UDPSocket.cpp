#include "Sockets.h"
#include <iostream>

#include "../Debug.h"

UDPSocket::~UDPSocket()
{
	closesocket(mSocket);
}

int UDPSocket::Bind(const SocketAddress& inBindAddress)
{
	int err = bind(mSocket, &inBindAddress.mSockAddr,
		inBindAddress.GetSize());
	if (err != 0)
	{
		SocketUtil::ReportError("UDPSocket::Bind");
		return SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

int UDPSocket::SendTo(const void* inData, int inLen, const SocketAddress& inTo)
{
	int byteSentCount = sendto(mSocket,	static_cast<const char*>(inData), inLen, 0, &inTo.mSockAddr, inTo.GetSize());
	if (byteSentCount <= 0)
	{
		//we'll return error as negative number to indicate less than requested amount of bytes sent...
		SocketUtil::ReportError("UDPSocket::SendTo");
		return -SocketUtil::GetLastError();
	}
	else
	{
		return byteSentCount;
	}
}

int UDPSocket::ReceiveFrom(void* inBuffer, int inLen, SocketAddress& outFrom)
{
	socklen_t fromLength = outFrom.GetSize();

	int readByteCount = recvfrom(mSocket, static_cast<char*>(inBuffer), inLen, 0, &outFrom.mSockAddr, &fromLength);
	if (readByteCount >= 0)
	{
		return readByteCount;
	}
	else
	{
		int error = SocketUtil::GetLastError();

		if (error == WSAEWOULDBLOCK)
		{
			return 0;
		}
		else if (error == WSAECONNRESET)
		{
			//this can happen if a client closed and we haven't DC'd yet.
			//this is the ICMP message being sent back saying the port on that computer is closed
			Debug::LogError("Connection reset from " + outFrom.ToString());
			return -WSAECONNRESET;
		}
		else
		{
			SocketUtil::ReportError("UDPSocket::ReceiveFrom");
			return -error;
		}
	}
}

int UDPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
{
	u_long arg = inShouldBeNonBlocking ? 1 : 0;
	int result = ioctlsocket(mSocket, FIONBIO, &arg);

	if (result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("UDPSocket::SetNonBlockingMode");
		return SocketUtil::GetLastError();
	}
	else
	{
		return NO_ERROR;
	}
}
