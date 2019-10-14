#pragma once

#include "GameObject.h"

class Sprite;
class User;

class LobbyPlayer : public GameObject
{
public:
	LobbyPlayer(User* user);
	~LobbyPlayer();

	void Update();

	User* GetUser() const { return m_User; }

private:

	User* m_User;
	Sprite* m_Sprite;
};

