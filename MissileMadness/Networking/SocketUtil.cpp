//---------------------------------------------------------
//	Adapted from book Multiplayer Game Programming 
//	by Joshua Glazer and Sanjay Madhav
//---------------------------------------------------------

#include "Sockets.h"

#include "../Debug.h"

bool SocketUtil::s_HasInit = false;

bool SocketUtil::StaticInit()
{
	if (s_HasInit)
	{
		Debug::LogError("WSAStartup already called!");
		return true;
	}

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		ReportError("Starting Up");
		return false;
	}

	s_HasInit = true;

	return true;
}

void SocketUtil::CleanUp()
{
	if (s_HasInit)
		WSACleanup();
	else
		Debug::LogError("Tried to clean up, but Winsock was never started!");
}

void SocketUtil::ReportError(const char* inOperationDesc)
{
	int errorNum = GetLastError();
	if (errorNum != WSAEWOULDBLOCK) // <- Non error, should not report
		Debug::LogErrorFormat("Error %s: %d", inOperationDesc, errorNum);
}

UDPSocketPtr SocketUtil::CreateUDPSocket(SocketAddressFamily inFamily)
{
	SOCKET s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);

	if (s != INVALID_SOCKET)
	{
		return UDPSocketPtr(new UDPSocket(s));
	}
	else
	{
		ReportError("SocketUtil::CreateUDPSocket");
		return nullptr;
	}
}

TCPSocketPtr SocketUtil::CreateTCPSocket(SocketAddressFamily inFamily)
{
	SOCKET s = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);

	if (s != INVALID_SOCKET)
	{
		return TCPSocketPtr(new TCPSocket(s));
	}
	else
	{
		ReportError("SocketUtil::CreateTCPSocket");
		return nullptr;
	}
}

SocketAddressPtr SocketUtil::CreateSocketAddress(const std::string& inString)
{
	auto pos = inString.find_last_of(':');
	std::string host, service;
	if (pos != std::string::npos)
	{
		host = inString.substr(0, pos);
		service = inString.substr(pos + 1);
	}
	else
	{
		host = inString;
		//use default port...
		service = "0";
	}
	addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;

	addrinfo* result;
	int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
	if (error != 0 && result != nullptr)
	{
		SocketUtil::ReportError("SocketAddressFactory::CreateIPv4FromString");
		return nullptr;
	}

	while (!result->ai_addr && result->ai_next)
	{
		result = result->ai_next;
	}

	if (!result->ai_addr)
	{
		return nullptr;
	}

	auto toRet = std::make_shared<SocketAddress>(*result->ai_addr);

	freeaddrinfo(result);

	return toRet;
}
