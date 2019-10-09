#pragma once

#include "sqlite3.h"

#include "Database.h"
#include "Types.h"

#define DATABASE_NAME "game.db"

class DatabaseManager
{
public:
	//static DatabaseManager& Instance();
	DatabaseManager() { }
	~DatabaseManager();

	bool Init();

	void AddNewUser(std::string name, std::string pw);
	void PrintUsers();

	bool UserExists(std::string username);
	bool Login(std::string username, std::string password, UInt32& outUserID);

private:

	DatabasePtr m_Database = nullptr;
	PreparedStatementPtr m_ListUsers = nullptr;
	PreparedStatementPtr m_AddUser = nullptr;
	PreparedStatementPtr m_GetUser = nullptr;
	
};

