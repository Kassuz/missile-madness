//---------------------------------------------------------
//	Adapted from book Multiplayer Game Programming 
//	by Joshua Glazer and Sanjay Madhav
//---------------------------------------------------------

#include "Sockets.h"

Int32 TCPSocket::Connect(const SocketAddress& inAddress)
{
	int err = connect(m_Socket, &inAddress.m_SockAddr, inAddress.GetSize());
	if (err < 0)
	{
		SocketUtil::ReportError("TCPSocket::Connect");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

Int32 TCPSocket::Listen(Int32 inBackLog)
{
	int err = listen(m_Socket, inBackLog);
	if (err < 0)
	{
		SocketUtil::ReportError("TCPSocket::Listen");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

TCPSocketPtr TCPSocket::Accept(SocketAddress& inFromAddress)
{
	socklen_t length = inFromAddress.GetSize();
	SOCKET newSocket = accept(m_Socket, &inFromAddress.m_SockAddr, &length);

	if (newSocket != INVALID_SOCKET)
	{
		return TCPSocketPtr(new TCPSocket(newSocket));
	}
	else
	{
		SocketUtil::ReportError("TCPSocket::Accept");
		return nullptr;
	}
}

Int32 TCPSocket::Send(const void* inData, size_t inLen)
{
	Int32 bytesSentCount = send(m_Socket, static_cast<const char*>(inData), inLen, 0);
	if (bytesSentCount < 0)
	{
		SocketUtil::ReportError("TCPSocket::Send");
		return -SocketUtil::GetLastError();
	}
	return bytesSentCount;
}

Int32 TCPSocket::Receive(void* inData, size_t inLen)
{
	Int32 bytesReceivedCount = recv(m_Socket, static_cast<char*>(inData), inLen, 0);
	if (bytesReceivedCount < 0)
	{
		SocketUtil::ReportError("TCPSocket::Receive");
		return -SocketUtil::GetLastError();
	}
	return bytesReceivedCount;
}

Int32 TCPSocket::Bind(const SocketAddress& inBindAddress)
{
	Int32 error = bind(m_Socket, &inBindAddress.m_SockAddr, inBindAddress.GetSize());
	if (error != 0)
	{
		SocketUtil::ReportError("TCPSocket::Bind");
		return SocketUtil::GetLastError();
	}

	return NO_ERROR;
}

Int32 TCPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
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

TCPSocket::~TCPSocket()
{
#if _WIN32
	closesocket(m_Socket);
#else
	close(mSocket);
#endif
}