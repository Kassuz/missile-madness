#include "DatabaseManager.h"

#include <iostream>

//------------------------------------------
//-               Callbacks                -
//------------------------------------------
namespace
{
	void TestCallback(sqlite3_stmt* s, void* outData)
	{
		for (int i = 0; i < sqlite3_column_count(s); ++i)
		{
			std::cout << sqlite3_column_text(s, i) << "\t";
		}
		std::cout << std::endl;
	}

	struct UserData
	{
		int userID;
		std::string username, password;
	};

	void GetUserCallback(sqlite3_stmt* s, void* outData)
	{
		UserData* data = static_cast<UserData*>(outData);

		data->userID = sqlite3_column_int(s, 0);
		data->username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));
		data->password = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 2)));
	}

	void GetColorCallback(sqlite3_stmt* s, void* outData)
	{
		Color* c = static_cast<Color*>(outData);
		c->r = (float)sqlite3_column_double(s, 0);
		c->g = (float)sqlite3_column_double(s, 1);
		c->b = (float)sqlite3_column_double(s, 2);
	}
}

//------------------------------------------
//-            Database Manager            -
//------------------------------------------
//DatabaseManager& DatabaseManager::Instance()
//{
//	static DatabaseManager instance;
//	return instance;
//}

DatabaseManager::~DatabaseManager()
{
}

bool DatabaseManager::Init()
{
	m_Database = DatabaseHelper::OpenDatabase(DATABASE_NAME);
	if (m_Database == nullptr)
		return false;

	m_ListUsers = DatabaseHelper::CreatePreparedStatement("select * from users", m_Database);
	if (m_ListUsers == nullptr)
		return false;

	m_AddUser = DatabaseHelper::CreatePreparedStatement("insert into users(username, password) values(?, ?)", m_Database);
	if (m_AddUser == nullptr)
		return false;

	m_GetUser = DatabaseHelper::CreatePreparedStatement("select userID, username, password from users where username=?", m_Database);
	if (m_GetUser == nullptr)
		return false;

	m_GetColor = DatabaseHelper::CreatePreparedStatement("select colorR, colorG, colorB from characters where userID=?", m_Database);
	if (m_GetColor == nullptr)
		return false;

	m_SetColor = DatabaseHelper::CreatePreparedStatement("update characters set colorR=?, colorG=?, colorB=? where userID=?", m_Database);
	if (m_SetColor == nullptr)
		return false;

	return true;
}



void DatabaseManager::AddNewUser(std::string name, std::string pw)
{
	//m_ListUsers->ExcecuteStatement(TestCallback, nullptr);

	m_AddUser->BindString(name, 1);
	m_AddUser->BindString(pw, 2);
	m_AddUser->ExcecuteStatement();

}

void DatabaseManager::PrintUsers()
{
	m_ListUsers->ExcecuteStatement(TestCallback);
}

bool DatabaseManager::UserExists(std::string username)
{
	m_GetUser->BindString(username, 1);
	UserData userData;

	m_GetUser->ExcecuteStatement(GetUserCallback, &userData);

	return userData.username == username;
}

bool DatabaseManager::Login(std::string username, std::string password, UInt32& outUserID)
{
	m_GetUser->BindString(username, 1);

	UserData userData;

	m_GetUser->ExcecuteStatement(GetUserCallback, &userData);
	
	outUserID = userData.userID;

	if (userData.username == username && userData.password == password)
		return true;
	else
		return false;
}

Color DatabaseManager::GetColorForUser(UInt32 userID)
{
	m_GetColor->BindInt(userID, 1);
	Color c;
	m_GetColor->ExcecuteStatement(GetColorCallback, &c);
	return c;
}

void DatabaseManager::SetColorForUser(UInt32 userID, Color c)
{
	m_SetColor->BindFloat(c.r, 1);
	m_SetColor->BindFloat(c.g, 2);
	m_SetColor->BindFloat(c.b, 3);
	m_SetColor->BindInt(userID, 4);
	m_SetColor->ExcecuteStatement();
}
