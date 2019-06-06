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
		memset(&mSockAddr, 0, sizeof(mSockAddr));
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetAsSockAddrIn()->sin_addr.S_un.S_addr = htonl(inAddress);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}
	SocketAddress(const sockaddr& inSockAddr)
	{
		memcpy(&mSockAddr, &inSockAddr, sizeof(sockaddr));
	}
	SocketAddress()
	{
		memset(&mSockAddr, 0, sizeof(mSockAddr));
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetIP4Ref() = INADDR_ANY;
		GetAsSockAddrIn()->sin_port = 0;
	}

	bool operator==(const SocketAddress& inOther) const
	{
		return (mSockAddr.sa_family == AF_INET && GetAsSockAddrIn()->sin_port == inOther.GetAsSockAddrIn()->sin_port)
			  && (GetIP4Ref() == inOther.GetIP4Ref());
	}

	size_t GetHash() const
	{
		return (GetIP4Ref()) | ((static_cast<UInt32>(GetAsSockAddrIn()->sin_port)) << 13) | mSockAddr.sa_family;
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

	sockaddr mSockAddr;

	      sockaddr_in* GetAsSockAddrIn()       { return reinterpret_cast<sockaddr_in*>(&mSockAddr); }
	const sockaddr_in* GetAsSockAddrIn() const { return reinterpret_cast<const sockaddr_in*>(&mSockAddr); }

	      UInt32& GetIP4Ref()         { return *reinterpret_cast<UInt32*>(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
	const UInt32& GetIP4Ref()	const { return *reinterpret_cast<const UInt32*>(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
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
	int Bind(const SocketAddress& inToAddress);
	int SendTo(const void* inData, int inLen, const SocketAddress& inTo);
	int ReceiveFrom(void* inBuffer, int inLen, SocketAddress& outFrom);

	int SetNonBlockingMode(bool inShouldBeNonBlocking);

private:
	friend class SocketUtil;
	UDPSocket(SOCKET inSocket) : mSocket(inSocket) {}
	SOCKET mSocket;
};
typedef std::shared_ptr<UDPSocket> UDPSocketPtr;


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
	static SocketAddressPtr CreateSocketAress(const std::string& address);
};


