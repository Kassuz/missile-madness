#include "ClientConnection.h"

ClientConnection::ClientConnection(SocketAddressPtr addr, TCPSocketPtr socket)
	: m_TCPAddr(addr), m_TCPSock(socket), m_ConnectionStatus(ConnectionStatus::NOT_LOGGED_IN)
{
	m_TCPSock->SetNonBlockingMode(true);
}

ClientConnection::~ClientConnection()
{
	if (m_User != nullptr)
		delete m_User;
}

void ClientConnection::LogIn(const std::string& username, UInt32 userID)
{
	m_User = new User(username, userID);
	m_ConnectionStatus = ConnectionStatus::LOGGED_IN;
}
