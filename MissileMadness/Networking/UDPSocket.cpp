//---------------------------------------------------------
//	Adapted from book Multiplayer Game Programming 
//	by Joshua Glazer and Sanjay Madhav
//---------------------------------------------------------

#include "Sockets.h"
#include <iostream>

#include "../Debug.h"

UDPSocket::~UDPSocket()
{
	closesocket(m_Socket);
}

Int32 UDPSocket::Bind(const SocketAddress& inBindAddress)
{
	Int32 err = bind(m_Socket, &inBindAddress.m_SockAddr,
		inBindAddress.GetSize());
	if (err != 0)
	{
		SocketUtil::ReportError("UDPSocket::Bind");
		return SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

Int32 UDPSocket::SendTo(const void* inData, Int32 inLen, const SocketAddress& inTo)
{
	Int32 byteSentCount = sendto(m_Socket, static_cast<const char*>(inData), inLen, 0, &inTo.m_SockAddr, inTo.GetSize());
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

Int32 UDPSocket::ReceiveFrom(void* inBuffer, Int32 inLen, SocketAddress& outFrom)
{
	socklen_t fromLength = outFrom.GetSize();

	Int32 readByteCount = recvfrom(m_Socket, static_cast<char*>(inBuffer), inLen, 0, &outFrom.m_SockAddr, &fromLength);
	if (readByteCount >= 0)
	{
		return readByteCount;
	}
	else
	{
		Int32 error = SocketUtil::GetLastError();

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

Int32 UDPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
{
	u_long arg = inShouldBeNonBlocking ? 1 : 0;
	Int32 result = ioctlsocket(m_Socket, FIONBIO, &arg);

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
