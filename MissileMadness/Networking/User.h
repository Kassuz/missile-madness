#pragma once

#include <string>
#include "../Types.h"

class User
{
public:
	User() : m_UserName(""), m_UserID(0), m_IsMe(false) { }
	User(std::string name, UInt32 id, bool isMe = false) : m_UserName(name), m_UserID(id), m_IsMe(isMe) { }
	~User() { }

	const std::string GetUsersName() const { return m_UserName; } // Somehow there is macro for GetUserName???
	const UInt32 GetUserID() const { return m_UserID; }
	const bool IsMe() const { return m_IsMe; }

	void SetUSerName(std::string name) { m_UserName = name; }
	void SetUserID(UInt32 id) { m_UserID = id; }
	void SetIsMe(bool isMe) { m_IsMe = isMe; }

private:
	std::string m_UserName;
	UInt32 m_UserID;
	bool m_IsMe;
};

