#include "LobbyManagerClient.h"

#include "Networking/User.h"
#include "Engine.h"
#include "ClientGame.h"
#include "CommandLineArgs.h"

#include <iostream>
#include <thread>

void LobbyManagerClient::Start()
{
	if (!SocketUtil::StaticInit())
	{
		Debug::LogError("Couldn't start Winsock!");
		return;
	}

	m_ClientSocket = SocketUtil::CreateTCPSocket(INET);
	if (m_ClientSocket == nullptr)
	{
		Debug::LogError("Creating client socket failed!");
		return;
	}

	std::string address;
	if (CommandLineArgs::UseCustomServerAddress())
		address = CommandLineArgs::GetServerAddress();
	else
		address = SERVER_ADDRESS_TCP;

	SocketAddressPtr serverAddress = SocketUtil::CreateSocketAddress(address);
	if (serverAddress == nullptr)
	{
		Debug::LogError("Creating server adress failed!");
		return;
	}

	// Try to connect to server
	float nextConnectTime = 0.0f;
	while (true)
	{
		if (Time::GetRealTime() > nextConnectTime)
		{
			Int32 res = m_ClientSocket->Connect(*serverAddress);
			
			if (res != NO_ERROR)
			{
				Debug::LogErrorFormat("Connecting failed! Trying again in %1.1fs", k_ReconnectIntervall);
				nextConnectTime = Time::GetRealTime() + k_ReconnectIntervall;
			}
			else
			{
				break;
			}
		}
	}

	if (m_ClientSocket->SetNonBlockingMode(true) != NO_ERROR)
	{
		Debug::LogError("Failed to set client scoket to non blocking mode!");
		return;
	}


	// LOGIN MENU
	while (m_LobbyStatus != LobbyStatus::LOGGED_IN)
	{
		ProcessPackets();

		switch (m_LobbyStatus)
		{
		case LobbyStatus::LOGIN_MENU:
			if (m_WaitingForResponse)
				continue;
			else
				LoginMenu();
			break;
		case LobbyStatus::EXIT:
			return; // QUIT
		default:
			break;
		}
	}
	
	// LOBBY MENU
	Debug::Log("You are now Logged In!");
	WindowPtr win = Window::CreateNewWindow(800, 600, "Missile Madness");
	RenderingEngine::Init(win, "Resources/Fonts/Roboto-Black.ttf");

	m_LobbyMenu.AddNewPlayer(User::Me);


	while (!win->WindowShouldClose() && m_LobbyStatus != LobbyStatus::EXIT)
	{
		if (m_GameShouldStart)
		{
			std::vector<User*> userVec;
			for (auto it : m_Users)
				userVec.push_back(it.second);

			m_LobbyMenu.StartGame();

			ClientGame game;
			std::string winner = "";
			if (!game.StartGame(win, userVec, winner))
			{
				// Error in game
				Debug::LogError("Game had error. Shutting down!");
				win->CloseWindow();
				continue;
			}

			m_GameShouldStart = false;
			m_LobbyMenu.ReturnToLobby(winner);
		}


		ProcessPackets();

		m_LobbyMenu.CheckInput();
		m_LobbyMenu.Draw();

		InputManager::Update();

		win->PollEvents();
	}
	
	std::cout << "\n\nGoodbye!" << std::endl;
}

void LobbyManagerClient::SetReady()
{
	OutputMemoryBitStream packet;
	packet.Write(LPT_CLIENT_REQUEST, GetRequiredBits<LPT_MAX_PACKET>::Value);
	packet.Write(CR_SET_READY, GetRequiredBits<CR_MAX_REQ>::Value);

	m_ClientSocket->Send(packet.GetBufferPtr(), packet.GetByteLength());
}

void LobbyManagerClient::ChangeUserColor(Color newColor)
{
	OutputMemoryBitStream packet;
	packet.Write(LPT_CLIENT_REQUEST, GetRequiredBits<LPT_MAX_PACKET>::Value);
	packet.Write(CR_COLOR_CHANGE, GetRequiredBits<CR_MAX_REQ>::Value);
	packet.Write(static_cast<glm::vec3>(newColor));

	m_ClientSocket->Send(packet.GetBufferPtr(), packet.GetByteLength());
}

void LobbyManagerClient::RequestStats(MatchDataFormat type)
{
	OutputMemoryBitStream packet;
	packet.Write(LPT_CLIENT_REQUEST, GetRequiredBits<LPT_MAX_PACKET>::Value);
	packet.Write(CR_MATCH_DATA, GetRequiredBits<CR_MAX_REQ>::Value);
	packet.Write(type, GetRequiredBits<MDF_MAX_FORMAT>::Value);

	m_ClientSocket->Send(packet.GetBufferPtr(), packet.GetByteLength());
}

void LobbyManagerClient::ProcessPackets()
{
	int recievedBytes = m_ClientSocket->Receive(m_RecieveBuffer, sizeof(m_RecieveBuffer));
	if (recievedBytes > 0) // Got data
	{
		InputMemoryBitStream packet(m_RecieveBuffer, recievedBytes * 8);
		LobbyPacketType type;
		packet.Read(type, GetRequiredBits<LPT_MAX_PACKET>::Value);

		switch (type)
		{
		case LPT_LOGIN_DATA:
			Debug::LogError("Client should not recieve LOGIN_DATA packets!");
			break;
		case LPT_CREATE_USER:
			Debug::LogError("Client should not recieve CREATE_USER packets!");
			break;
		case LPT_USER_DATA:
			ProcessUserData(packet);
			break;
		case LPT_USER_DISCONNECT:
			ProcessUserDisconnect(packet);
			break;
		case LPT_MATCH_DATA:
			ProcessMatchData(packet);
			break;
		case LPT_CLIENT_REQUEST:
			Debug::LogError("Client should not recieve CLIENT_REQUEST packets!");
			break;
		case LPT_SERVER_RESPONSE:
			ProcessServerResponse(packet);
			break;
		case LPT_GAME_START:
			m_GameShouldStart = true;
			Debug::Log("Game start packet recieved!");
			break;
		case LPT_MAX_PACKET:
			Debug::LogError("MAX_PACKET should never be sent!");
			break;
		default:
			break;
		}
	}
	else if (recievedBytes == 0 || recievedBytes == -WSAECONNRESET) // FIN packet or connection reset
	{
		Debug::LogError("Connection to server lost");
		m_LobbyStatus = LobbyStatus::EXIT;
	}
	else if (recievedBytes != -WSAEWOULDBLOCK) // Error
	{
		Debug::LogError("Error recieving data from server!");
	}
}

void LobbyManagerClient::ProcessUserData(InputMemoryBitStream& packet)
{
	if (m_LobbyStatus != LobbyStatus::LOGGED_IN) return;

	UInt32 userCount; packet.Read(userCount, 32);

	std::string username;
	UInt32 userID;
	glm::vec3 color;
	bool ready;

	for (UInt32 i = 0U; i < userCount; ++i)
	{
		// Can't use User::Read(), beecause we don't know which user to read
		// TODO Maybe figure out better solution
		packet.Read(username);
		packet.Read(userID);
		packet.Read(color);
		packet.Read(ready);

		auto usIt = m_Users.find(userID);
		if (usIt == m_Users.end() && userID != User::Me->GetUserID()) // Second should always be true
		{
			User* u = new User(username, userID, false, color);
			u->SetReady(ready);

			m_Users.emplace(userID, u);
			m_LobbyMenu.AddNewPlayer(u);
		}
		else
		{
			User* u = usIt->second;
			u->SetUserName(username);
			u->SetUserID(userID);
			u->SetCharacterColor(Color(color));
			u->SetReady(ready);
		}
	}

	m_LobbyMenu.SetDirty();
}

void LobbyManagerClient::ProcessServerResponse(InputMemoryBitStream& packet)
{
	m_WaitingForResponse = false;

	ServerResponse response;
	packet.Read(response, GetRequiredBits<SR_MAX_RESPONSE>::Value);
	switch (response)
	{
	case SR_LOGIN_OK:
	{
		if (m_LobbyStatus == LobbyStatus::LOGIN_MENU)
		{
			m_LobbyStatus = LobbyStatus::LOGGED_IN;

			if (User::Me != nullptr)
				Debug::LogErrorFormat("Somehow User::Me already set as %s with id %u", User::Me->GetUsersName().c_str(), User::Me->GetUserID());

			// Get user stats
			std::string username;	packet.Read(username);
			UInt32 userID;			packet.Read(userID);
			glm::vec3 color;		packet.Read(color);
			bool ready;				packet.Read(ready); // This is false

			User* me = new User(username, userID, true, Color(color));
			User::Me = me;
			m_Users.emplace(userID, me);
			Debug::LogFormat("Login success! User ID: %u", userID);
		}
		break;
	}
	case SR_LOGIN_WRONG:
	{
		if (m_LobbyStatus == LobbyStatus::LOGIN_MENU)
			Debug::LogError("Username or password is wrong!");
		break;
	}
	case SR_USER_CREATE_SUCCESS:
		Debug::Log("User created successfully!");
		break;
	case SR_USER_ALREADY_EXISTS:
		Debug::LogError("User with that name already exists! Please choose different name");
		break;
	case SR_USERNAME_LENGTH_BAD:
		Debug::LogError("Username should be between 3 and 50 characters!");
		break;
	case SR_PASSWORD_LENGTH_BAD:
		Debug::LogError("Password should be between 3 and 50 characters!");
		break;
	case SR_SERVER_ERROR:
		Debug::LogError("Server had an error!");
		break;
	case SR_MAX_RESPONSE:
		Debug::LogError("MAX_RESPONSE should never be sent!");
		break;
	default:
		break;
	}
}

void LobbyManagerClient::ProcessUserDisconnect(InputMemoryBitStream& packet)
{
	UInt32 userID; packet.Read(userID);
	auto usIt = m_Users.find(userID);
	if (usIt != m_Users.end())
	{
		User* u = usIt->second;
		m_LobbyMenu.DeletePlayer(u);
		m_Users.erase(usIt);
		Debug::LogErrorFormat("User %s disconnected!", u->GetUsersName().c_str());
		delete u;
	}
}

void LobbyManagerClient::ProcessMatchData(InputMemoryBitStream& packet)
{
	std::vector<std::string> stats;
	
	UInt32 count; packet.Read(count);
	if (count > 0)
	{
		std::string s;
		for (int i = 0; i < count; ++i)
		{
			packet.Read(s);
			stats.push_back(s);
		}
	}
	else
	{
		stats.push_back("NO STATS :(");
	}

	m_LobbyMenu.DisplayStats(stats);
}

void LobbyManagerClient::LoginMenu()
{
	std::cout << "\nWhat do?" << std::endl;
	std::cout << "1. Create new user" << std::endl;
	std::cout << "2. Login" << std::endl;
	std::cout << "0. Exit" << std::endl;
	std::cout << "> ";
	std::string answer;
	std::getline(std::cin, answer);

	//std::cout << std::endl;

	try
	{
		int ansInt = std::stoi(answer);

		switch (ansInt)
		{
		case 1: // Create user
		{
			std::string name, password;
			if (!GetUsernameAndPassword(name, password, true))
				return;

			OutputMemoryBitStream packet;
			packet.Write(LPT_CREATE_USER, GetRequiredBits<LPT_MAX_PACKET>::Value);
			packet.Write(name);
			packet.Write(password);

			m_ClientSocket->Send(packet.GetBufferPtr(), packet.GetByteLength());

			m_WaitingForResponse = true;

			break;
		}
		case 2: // Login
		{
			std::string name, password;
			GetUsernameAndPassword(name, password);

			OutputMemoryBitStream packet;
			packet.Write(LPT_LOGIN_DATA, GetRequiredBits<LPT_MAX_PACKET>::Value);
			packet.Write(name);
			packet.Write(password);

			m_ClientSocket->Send(packet.GetBufferPtr(), packet.GetByteLength());
			
			m_WaitingForResponse = true;

			break;
		}
		case 0:
			m_LobbyStatus = LobbyStatus::EXIT;
			return;
		}

	}
	catch (std::exception e)
	{
		std::cout << "Gib number plz" << std::endl;
	}
}

namespace
{
	// Shows asteriks when typing password
	std::string Getpass(bool show_asterisk = true)
	{
		const char BACKSPACE = 8;
		const char RETURN = 13;

		std::string password;
		unsigned char ch = 0;

		DWORD con_mode;
		DWORD dwRead;

		HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

		GetConsoleMode(hIn, &con_mode);
		SetConsoleMode(hIn, con_mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

		while (ReadConsoleA(hIn, &ch, 1, &dwRead, NULL) && ch != RETURN)
		{
			if (ch == BACKSPACE)
			{
				if (password.length() != 0)
				{
					if (show_asterisk)
						std::cout << "\b \b";
					password.resize(password.length() - 1);
				}
			}
			else
			{
				password += ch;
				if (show_asterisk)
					std::cout << '*';
			}
		}
		std::cout << std::endl;

		SetConsoleMode(hIn, con_mode);

		return password;
	}
}

bool LobbyManagerClient::GetUsernameAndPassword(std::string& username, std::string& password, bool retypePassword)
{
	std::cout << std::endl;
	std::cout << "Enter username: ";
	std::getline(std::cin, username);
	std::cout << "Enter password: ";
	password = Getpass();

	if (retypePassword)
	{
		std::string password2;
		std::cout << "Retype password: ";
		password2 = Getpass();

		if (password != password2)
		{
			Debug::LogError("Passwords don't match!");
			return false;
		}
	}

	return true;
}
