#pragma once

#include "sqlite3.h"

#include "Database.h"
#include "Types.h"
#include "Color.h"

#include <unordered_map>
#include <vector>
#include <string>

#define DATABASE_NAME "game.db"

struct GameStats;

class DatabaseManager
{
public:
	//static DatabaseManager& Instance();
	DatabaseManager() { }
	~DatabaseManager();

	bool Init();

	bool AddNewUser(std::string name, std::string pw);
	void PrintUsers();

	bool UserExists(std::string username);
	bool Login(std::string username, std::string password, UInt32& outUserID);

	Color GetColorForUser(UInt32 userID);
	void  SetColorForUser(UInt32 userID, Color c);

	void SaveMatchData(UInt64 matchStart, UInt32 playerCount, float matchDuration, std::unordered_map<UInt32, GameStats*> stats);

	std::vector<std::string> GetAllTimeStatsForUser(UInt32 userID);
	std::vector<std::string> GetLastMatchStatsForUser(UInt32 userID);
	std::vector<std::string> GetLast5MatchesStatsForUser(UInt32 userID);


private:

	DatabasePtr m_Database = nullptr;
	PreparedStatementPtr m_ListUsers = nullptr;
	PreparedStatementPtr m_AddUser = nullptr;
	PreparedStatementPtr m_GetUser = nullptr;
	PreparedStatementPtr m_GetColor = nullptr;
	PreparedStatementPtr m_SetColor = nullptr;

	PreparedStatementPtr m_CreateMatch = nullptr;
	PreparedStatementPtr m_CreateMatchRecord = nullptr;

	PreparedStatementPtr m_GetMostRecentMatchID = nullptr;

	// Gets all match records for specified matchID
	PreparedStatementPtr m_GetMatchRecordsForMatch = nullptr;
	
	// Gets most recent match for specified userID
	PreparedStatementPtr m_GetMostRecentMatchForUser = nullptr;

	// Gets total wins, kills and deaths for userID
	PreparedStatementPtr m_GetTotalStatsForUser = nullptr;

	// Gets X amount of latest match stats for user
	// MatchTimestamp, playerCount, matchDuration, placement, kills, deaths
	PreparedStatementPtr m_GetLatestMatchStatsForUser = nullptr;
};

