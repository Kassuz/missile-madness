//---------------------------------------------------------
//	Adapted from book Multiplayer Game Programming 
//	by Joshua Glazer and Sanjay Madhav
//---------------------------------------------------------

#pragma once

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <memory>
#include <string>

#include "../Types.h"

// ---------------------------
// -      Socket Adress      -
// ---------------------------
class SocketAddress
{
public:
	SocketAddress(UInt32 inAddress, UInt16 inPort)
	{
		memset(&m_SockAddr, 0, sizeof(m_SockAddr));
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetAsSockAddrIn()->sin_addr.S_un.S_addr = htonl(inAddress);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}
	SocketAddress(const sockaddr& inSockAddr)
	{
		memcpy(&m_SockAddr, &inSockAddr, sizeof(sockaddr));
	}
	SocketAddress()
	{
		memset(&m_SockAddr, 0, sizeof(m_SockAddr));
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetIP4Ref() = INADDR_ANY;
		GetAsSockAddrIn()->sin_port = 0;
	}

	bool operator==(const SocketAddress& inOther) const
	{
		return (m_SockAddr.sa_family == AF_INET && GetAsSockAddrIn()->sin_port == inOther.GetAsSockAddrIn()->sin_port)
			  && (GetIP4Ref() == inOther.GetIP4Ref());
	}

	size_t GetHash() const
	{
		return (GetIP4Ref()) | ((static_cast<UInt32>(GetAsSockAddrIn()->sin_port)) << 13) | m_SockAddr.sa_family;
	}

	size_t GetSize() const { return sizeof(sockaddr); }
	
	std::string ToString()
	{
		const sockaddr_in* s = GetAsSockAddrIn();
		char buffer[128];
		inet_ntop(s->sin_family, const_cast<in_addr*>(&s->sin_addr), buffer, sizeof(buffer));
		std::string returnStr(buffer);
		return returnStr + ":" + std::to_string(ntohs(s->sin_port));
	}

private:
	friend class UDPSocket;
	friend class TCPSocket;

	sockaddr m_SockAddr;

	sockaddr_in* GetAsSockAddrIn() { return reinterpret_cast<sockaddr_in*>(&m_SockAddr); }
	const sockaddr_in* GetAsSockAddrIn() const { return reinterpret_cast<const sockaddr_in*>(&m_SockAddr); }

	UInt32& GetIP4Ref() { return *reinterpret_cast<UInt32*>(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
	const UInt32& GetIP4Ref() const { return *reinterpret_cast<const UInt32*>(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
};
typedef std::shared_ptr<SocketAddress> SocketAddressPtr;

// We can use SocketAddress in unordered_map
namespace std
{
	template<> 
	struct hash<SocketAddress>
	{
		size_t operator()(const SocketAddress& inAddress) const
		{
			return inAddress.GetHash();
		}
	};
}


// --------------------------
// -       UDP SOCKET       -
// --------------------------
class UDPSocket
{
public:
	~UDPSocket();
	Int32 Bind(const SocketAddress& inToAddress);
	Int32 SendTo(const void* inData, Int32 inLen, const SocketAddress& inTo);
	Int32 ReceiveFrom(void* inBuffer, Int32 inLen, SocketAddress& outFrom);

	Int32 SetNonBlockingMode(bool inShouldBeNonBlocking);

private:
	friend class SocketUtil;
	UDPSocket(SOCKET inSocket) : m_Socket(inSocket) {}
	SOCKET m_Socket;
};
typedef std::shared_ptr<UDPSocket> UDPSocketPtr;

// --------------------------
// -       TCP SOCKET       -
// --------------------------
class TCPSocket
{
public:
	~TCPSocket();
	Int32 Connect(const SocketAddress& inAddress);
	Int32 Bind(const SocketAddress& inToAddress);
	Int32 Listen(Int32 inBackLog = 32);
	std::shared_ptr<TCPSocket> Accept(SocketAddress& inFromAddress);
	Int32 Send(const void* inData, size_t inLen);
	Int32 Receive(void* inBuffer, size_t inLen);

	Int32 SetNonBlockingMode(bool inShouldBeNonBlocking);

private:
	friend class SocketUtil;
	TCPSocket(SOCKET inSocket) : m_Socket(inSocket) {}
	SOCKET m_Socket;
};
typedef std::shared_ptr<TCPSocket> TCPSocketPtr;


// -------------------------
// -      SOCKET UTIL      -
// -------------------------
enum SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};

class SocketUtil
{
public:
	static bool	StaticInit();
	static void	CleanUp();

	static void	ReportError(const char* inOperationDesc);
	static int GetLastError() { return WSAGetLastError(); }


	static UDPSocketPtr	CreateUDPSocket(SocketAddressFamily inFamily);
	static TCPSocketPtr	CreateTCPSocket(SocketAddressFamily inFamily);
	static SocketAddressPtr CreateSocketAress(const std::string& address);
};


