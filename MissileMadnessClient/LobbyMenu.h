#pragma once

#include <vector>

class LobbyPlayer;
class User;
class LobbyManagerClient;

class LobbyMenu
{
public:
	enum class MenuState { MAIN, COLOR, STATS };

	LobbyMenu(LobbyManagerClient* manager) : m_LobbyManager(manager) { }
	~LobbyMenu() { }

	void Draw();
	void SetDirty() { m_IsDirty = true; }

	void CheckInput();

	void AddNewPlayer(User* u);
	void DeletePlayer(User* u);

private:
	bool m_IsDirty = true;
	LobbyManagerClient* m_LobbyManager;

	std::vector<LobbyPlayer*> m_PlayerSprites;

	MenuState m_MenuState = MenuState::MAIN;

	void DrawMainMenu();
	void DrawColorMenu();
	void DrawStatsMenu();
	void UpdatePlayers();
};

