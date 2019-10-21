#include "DatabaseManager.h"

#include <iostream>

#include "ServerGame.h"
#include "MMTime.h"
#include "Crypto.h"

//------------------------------------------
//-               Callbacks                -
//------------------------------------------
namespace
{
	void PrintCallback(sqlite3_stmt* s, void* outData)
	{
		for (int i = 0; i < sqlite3_column_count(s); ++i)
		{
			std::cout << sqlite3_column_text(s, i) << "\t";
		}
		std::cout << std::endl;
	}

	struct UserData
	{
		UInt32 userID;
		std::string username, passwordHash;
	};

	void GetUserCallback(sqlite3_stmt* s, void* outData)
	{
		UserData* data = static_cast<UserData*>(outData);

		data->userID = sqlite3_column_int(s, 0);
		data->username = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));
		data->passwordHash = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 2)));
	}

	void GetColorCallback(sqlite3_stmt* s, void* outData)
	{
		Color* c = static_cast<Color*>(outData);
		c->r = (float)sqlite3_column_double(s, 0);
		c->g = (float)sqlite3_column_double(s, 1);
		c->b = (float)sqlite3_column_double(s, 2);
	}

	struct Match
	{
		UInt32 matchID = 0U;
		UInt64 matchTimestamp = 0U;
		std::string timeString = "";
		UInt32 playerCount = 0U;
		float matchDuration = 0.0f;
	};

	// Todo maybe just one sql command
	void GetMostRecentMatchCallback(sqlite3_stmt* s, void* outData)
	{
		UInt32* id = static_cast<UInt32*>(outData);
		
		*id = sqlite3_column_int(s, 0);
	}

	void GetMatchDataCallback(sqlite3_stmt* s, void* outData)
	{
		Match* m = static_cast<Match*>(outData);

		m->matchID = sqlite3_column_int(s, 0);
		m->timeString = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));
		m->playerCount = sqlite3_column_int(s, 2);
		m->matchDuration = static_cast<float>(sqlite3_column_double(s, 3));
	}

	void GetAllTimeStatsCallback(sqlite3_stmt* s, void* outData)
	{
		auto list = static_cast<std::vector<std::string>*>(outData);

		std::string wins   = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 0)));
		std::string kills  = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));
		std::string deaths = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 2)));

		list->push_back("Wins: " + wins);
		list->push_back("Kills: " + kills);
		list->push_back("Deaths: " + deaths);
	}

	void GetMatchRecordsCallback(sqlite3_stmt* s, void* outData)
	{
		auto list = static_cast<std::vector<std::string>*>(outData);

		int placement = sqlite3_column_int(s, 0);
		std::string name   = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));
		std::string kills  = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 2)));
		std::string deaths = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 3)));

		std::string placementStr = placement < 1000 ? std::to_string(placement) + "." : "DC";

		list->push_back(placementStr + " " + name + "    K: " + kills + "    D: " + deaths);
	}

	void GetLatestMatchDataCallback(sqlite3_stmt* s, void* outData)
	{
		std::string time          = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 0)));
		std::string playerCount   = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));
		std::string matchDuration = Time::FormatSeconds(sqlite3_column_double(s, 2));
		int placement			  = sqlite3_column_int(s, 3);
		std::string kills         = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 4)));
		std::string deaths        = std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 5)));

		std::string placementStr = placement < 1000 ? std::to_string(placement) + "." : "DC";

		auto list = static_cast<std::vector<std::string>*>(outData);
		list->push_back(time + "    Players: " + playerCount + "    Duration: " + matchDuration + 
						"    Placement: " + placementStr + "    K: " + kills + "     D: " + deaths);
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

	m_CreateMatch = DatabaseHelper::CreatePreparedStatement("insert into matches(timestamp, playerCount, duration) values(?, ?, ?)", m_Database);
	if (m_CreateMatch == nullptr)
		return false;

	m_CreateMatchRecord = DatabaseHelper::CreatePreparedStatement("insert into match_records(userID, matchID, kills, deaths, placement) \
																   values(?, ?, ?, ?, ?)", m_Database);
	if (m_CreateMatchRecord == nullptr)
		return false;

	m_GetMostRecentMatchID = DatabaseHelper::CreatePreparedStatement("select matchID, max(timestamp) from matches", m_Database);
	if (m_GetMostRecentMatchID == nullptr)
		return false;

	m_GetMatchRecordsForMatch = DatabaseHelper::CreatePreparedStatement("select placement, users.username, kills, deaths \
																		from match_records, users \
																		where users.userID=match_records.userID and matchID = ? \
																		order by placement;", m_Database);
	if (m_GetMatchRecordsForMatch == nullptr)
		return false;

	m_GetMostRecentMatchForUser
		= DatabaseHelper::CreatePreparedStatement("select matches.matchID, datetime(max(matches.timestamp), 'unixepoch', 'localtime'), \
												   matches.playerCount, matches.duration \
												   from matches, match_records \
												   where matches.matchID=match_records.matchID and match_records.userID=?", m_Database);
	if (m_GetMostRecentMatchForUser == nullptr)
		return false;	
		
	m_GetTotalStatsForUser = DatabaseHelper::CreatePreparedStatement("select sum(case when placement=1 then 1 else 0 end), sum(kills), sum(deaths) \
																	  from match_records where userID=?", m_Database);	
	if (m_GetTotalStatsForUser == nullptr)
		return false;

	m_GetLatestMatchStatsForUser = 
		DatabaseHelper::CreatePreparedStatement("select datetime(matches.timestamp, 'unixepoch', 'localtime'), matches.playercount, \
												 matches.duration, match_records.placement, match_records.kills, match_records.deaths \
												 from matches, match_records \
												 where matches.matchID=match_records.matchID and match_records.userID=? \
												 order by matches.timestamp desc limit ?", m_Database);
	if (m_GetLatestMatchStatsForUser == nullptr)
		return false;

	return true;
}


bool DatabaseManager::AddNewUser(std::string name, std::string pw)
{
	std::string passwordHash;
	if (Crypto::HashPassword(pw, passwordHash))
	{
		m_AddUser->BindString(name, 1);
		m_AddUser->BindString(passwordHash, 2);
		m_AddUser->ExcecuteStatement();
		return true;
	}
	else
	{
		return false;
	}
}

void DatabaseManager::PrintUsers()
{
	m_ListUsers->ExcecuteStatement(PrintCallback);
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
	UserData userData;

	m_GetUser->BindString(username, 1);
	m_GetUser->ExcecuteStatement(GetUserCallback, &userData);
	
	outUserID = userData.userID;

	bool passwordMatches = Crypto::VerifyPassword(password, userData.passwordHash);

	if (userData.username == username && passwordMatches)
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

void DatabaseManager::SaveMatchData(UInt64 matchStart, UInt32 playerCount, float matchDuration, std::unordered_map<UInt32, GameStats*> stats)
{
	m_Database->StartTransaction();

	m_CreateMatch->BindInt(matchStart, 1);
	m_CreateMatch->BindInt(playerCount, 2);
	m_CreateMatch->BindFloat(matchDuration, 3);
	m_CreateMatch->ExcecuteStatement();

	// We don't know the match id, so get the most recent matchID, which is the match we just created
	UInt32 matchID = 0U;
	m_GetMostRecentMatchID->ExcecuteStatement(GetMostRecentMatchCallback, &matchID);

	if (matchID == 0U)
	{
		Debug::LogError("Finding most recent match failed. Roll back!");
		m_Database->RollBackTransaction();
		return;
	}

	// Order match stats
	std::vector<std::pair<UInt32, GameStats*>> orderedStats;
	for (auto s : stats) orderedStats.push_back(s);

	std::sort(std::begin(orderedStats), std::end(orderedStats), 
		[](const std::pair<UInt32, GameStats*>& a, const std::pair<UInt32, GameStats*>& b) -> bool
		{
			if (!a.second->m_IsDisconnected && b.second->m_IsDisconnected) return true;
			if (a.second->m_IsDisconnected && !b.second->m_IsDisconnected) return false;

			if (a.second->m_Kills > b.second->m_Kills) return true;
			if (a.second->m_Kills < b.second->m_Kills) return false;

			if (a.second->m_Deaths < b.second->m_Deaths) return true;
			if (a.second->m_Deaths > b.second->m_Deaths) return false;

			return true;
		});

	for (int i = 0; i < orderedStats.size(); ++i)
	{
		auto it = orderedStats[i];
		m_CreateMatchRecord->BindInt(it.first, 1);
		m_CreateMatchRecord->BindInt(matchID, 2);
		m_CreateMatchRecord->BindInt(it.second->m_Kills, 3);
		m_CreateMatchRecord->BindInt(it.second->m_Deaths, 4);

		int placement = it.second->m_IsDisconnected ? i + 1000 : i + 1; // Disconnected placement > 1000
		m_CreateMatchRecord->BindInt(placement, 5);

		m_CreateMatchRecord->ExcecuteStatement();
	}

	m_Database->EndTransaction();

	Debug::Log("Match stats successfully saved to database!");
}

std::vector<std::string> DatabaseManager::GetAllTimeStatsForUser(UInt32 userID)
{
	std::vector<std::string> stats;

	m_GetTotalStatsForUser->BindInt(userID, 1);
	m_GetTotalStatsForUser->ExcecuteStatement(GetAllTimeStatsCallback, &stats);

	return stats;
}

std::vector<std::string> DatabaseManager::GetLastMatchStatsForUser(UInt32 userID)
{
	std::vector<std::string> stats;

	m_Database->StartTransaction();

	Match m;
	m_GetMostRecentMatchForUser->BindInt(userID, 1);
	m_GetMostRecentMatchForUser->ExcecuteStatement(GetMatchDataCallback, &m);
	if (m.matchID != 0U)
	{
		stats.push_back(m.timeString + "  Players: " + std::to_string(m.playerCount) + "  Duration: " + Time::FormatSeconds(m.matchDuration));

		m_GetMatchRecordsForMatch->BindInt(m.matchID, 1);
		m_GetMatchRecordsForMatch->ExcecuteStatement(GetMatchRecordsCallback, &stats);
	}

	m_Database->EndTransaction();

	return stats;
}

std::vector<std::string> DatabaseManager::GetLast5MatchesStatsForUser(UInt32 userID)
{
	std::vector<std::string> stats;

	m_GetLatestMatchStatsForUser->BindInt(userID, 1);
	m_GetLatestMatchStatsForUser->BindInt(5, 2); // Count
	m_GetLatestMatchStatsForUser->ExcecuteStatement(GetLatestMatchDataCallback, &stats);

	return stats;;
}
