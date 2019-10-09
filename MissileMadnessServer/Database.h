#pragma once

#include "sqlite3.h"
#include <string>
#include <memory>


//-----------------------------
//-		     Database         -
//-----------------------------

class Database
{
public:
	~Database();

	//sqlite3* GetDatabasePointer() { return m_Database; }

	bool StartTransaction();
	bool EndTransaction();
	bool RollBackTransaction();

private:
	Database(sqlite3* db) : m_Database(db) { }

	sqlite3* m_Database;

	int m_RC = 0;
	char* m_ErrMsg = nullptr;

	friend class DatabaseHelper;
};
typedef std::shared_ptr<Database> DatabasePtr;

//-----------------------------
//-		Prepared Statement    -
//-----------------------------

typedef void (*StatementCallback)(sqlite3_stmt*, void*);

class PreparedStatement
{
public:
	~PreparedStatement();
	
	// Excecute prepared statement. 
	// Callback gets excecuted for every row for the result
	// OutData gets passed to callback, where it can be assigned if needed
	void ExcecuteStatement(StatementCallback callback = nullptr, void* outData = nullptr);

	bool BindString(std::string value, int index);
	bool BindInt(int value, int index);
	bool BindDouble(double value, int index);
	bool BindFloat(float value, int index) { BindDouble(value, index); }

private:
	PreparedStatement(sqlite3_stmt* stmt) : m_Statement(stmt) { }
	
	sqlite3_stmt* m_Statement;
	int m_RC = 0;

	friend class DatabaseHelper;
};
typedef std::shared_ptr<PreparedStatement> PreparedStatementPtr;

//-----------------------------
//-		 Database  Helper     -
//-----------------------------

class DatabaseHelper
{
public:
	static DatabasePtr OpenDatabase(std::string dbName);
	static PreparedStatementPtr CreatePreparedStatement(std::string statement, DatabasePtr db);
};
