#include "Database.h"

#include <Debug.h>

//-----------------------------
//-		     Database         -
//-----------------------------

Database::~Database()
{
	sqlite3_close_v2(m_Database);
}

bool Database::StartTransaction()
{
	m_RC = sqlite3_exec(m_Database, "begin transaction", 0, 0, &m_ErrMsg);
	if (m_RC != SQLITE_OK)
	{
		Debug::LogErrorFormat("Error begining transaction. ErrMsg: %s", m_ErrMsg);
		sqlite3_free(m_ErrMsg);
		return false;
	}
	else
	{
		return true;
	}
}

bool Database::EndTransaction()
{
	m_RC = sqlite3_exec(m_Database, "end transaction", 0, 0, &m_ErrMsg);
	if (m_RC != SQLITE_OK)
	{
		Debug::LogErrorFormat("Error ending transaction. ErrMsg: %s", m_ErrMsg);
		sqlite3_free(m_ErrMsg);
		return false;
	}
	else
	{
		return true;
	}
}

bool Database::RollBackTransaction()
{
	m_RC = sqlite3_exec(m_Database, "rollback", 0, 0, &m_ErrMsg);
	if (m_RC != SQLITE_OK)
	{
		Debug::LogErrorFormat("Error rolling back transaction. ErrMsg: %s", m_ErrMsg);
		sqlite3_free(m_ErrMsg);
		return false;
	}
	else
	{
		return true;
	}
}

//-----------------------------
//-		Prepared Statement    -
//-----------------------------


PreparedStatement::~PreparedStatement()
{
	// Finalize statement
	sqlite3_finalize(m_Statement);
}

void PreparedStatement::ExcecuteStatement(StatementCallback callback, void* outData)
{
	while (sqlite3_step(m_Statement) == SQLITE_ROW)
	{
		if (callback != nullptr)
			callback(m_Statement, outData);
	}

	sqlite3_clear_bindings(m_Statement);
	sqlite3_reset(m_Statement);
}

bool PreparedStatement::BindString(std::string value, int index)
{
	m_RC = sqlite3_bind_text(m_Statement, index, value.c_str(), -1, SQLITE_TRANSIENT);
	if (m_RC != SQLITE_OK)
	{
		Debug::LogErrorFormat("Error binding value %s to prepared statement. ErrorCode: %i", value.c_str(), m_RC);
		return false;
	}
	else
	{
		return true;
	}
}

bool PreparedStatement::BindInt(int value, int index)
{
	m_RC = sqlite3_bind_int(m_Statement, index, value);
	if (m_RC != SQLITE_OK)
	{
		Debug::LogErrorFormat("Error binding value %i to prepared statement. ErrorCode: %i", value, m_RC);
		return false;
	}
	else
	{
		return true;
	}
}

bool PreparedStatement::BindDouble(double value, int index)
{
	m_RC = sqlite3_bind_double(m_Statement, index, value);
	if (m_RC != SQLITE_OK)
	{
		Debug::LogErrorFormat("Error binding value %d to prepared statement. ErrorCode: %i", value, m_RC);
		return false;
	}
	else
	{
		return true;
	}
}

//-----------------------------
//-		 Database  Helper     -
//-----------------------------


DatabasePtr DatabaseHelper::OpenDatabase(std::string dbName)
{
	sqlite3* db;
	int rc = sqlite3_open(dbName.c_str(), &db);

	if (rc != SQLITE_OK)
	{
		Debug::LogErrorFormat("Opening database %s failed: %s", dbName.c_str(), sqlite3_errmsg(db));
		return nullptr;
	}
	else
	{
		Debug::Log("Database opened");
		return DatabasePtr(new Database(db));
	}

}

PreparedStatementPtr DatabaseHelper::CreatePreparedStatement(std::string statement, DatabasePtr db)
{
	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2(db->m_Database, statement.c_str(), -1, &stmt, 0);
	if (rc != SQLITE_OK)
	{
		Debug::LogErrorFormat("Creating prepared statement: %s, FAILED. Error code: %i", statement.c_str(), rc);
		return nullptr;
	}
	else
	{
		return PreparedStatementPtr(new PreparedStatement(stmt));
	}

}
