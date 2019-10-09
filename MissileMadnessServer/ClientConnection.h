#pragma once

#include <Networking/Sockets.h>
#include <Networking/User.h>
#include <memory>

enum class ConnectionStatus
{
	NOT_LOGGED_IN,
	LOGGED_IN,
};

class ClientConnection
{
public:

	ClientConnection(SocketAddressPtr addr, TCPSocketPtr socket);

	~ClientConnection();

	ConnectionStatus GetConnectionStatus() const { return m_ConnectionStatus; }
	void SetConnectionStatus(ConnectionStatus status) { m_ConnectionStatus = status; }

	SocketAddressPtr GetTCPAddress() const { return m_TCPAddr; }
	TCPSocketPtr GetTCPSocket() const { return m_TCPSock; }

	User* GetUser() { return m_User; }

	void LogIn(const std::string& username, UInt32 userID);

private:
	
	ConnectionStatus m_ConnectionStatus;

	SocketAddressPtr m_TCPAddr;
	TCPSocketPtr m_TCPSock;

	User* m_User = nullptr;
};

typedef std::shared_ptr<ClientConnection> ClientConnectionPtr;
