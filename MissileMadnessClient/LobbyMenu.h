#pragma once

#include <vector>
#include <string>

class LobbyPlayer;
class User;
class LobbyManagerClient;

class LobbyMenu
{
public:
	enum class MenuState { MAIN, COLOR, STATS, WINNER };

	LobbyMenu(LobbyManagerClient* manager) : m_LobbyManager(manager) { }
	~LobbyMenu() { }

	void Draw();
	void SetDirty();

	void CheckInput();

	void AddNewPlayer(User* u);
	void DeletePlayer(User* u);

	void DisplayStats(std::vector<std::string> stats) { m_Stats = stats; SetDirty(); }

	void StartGame();
	void ReturnToLobby(const std::string& winnerName);

private:
	bool m_IsDirty = true;
	LobbyManagerClient* m_LobbyManager;

	std::vector<LobbyPlayer*> m_PlayerSprites;

	std::string m_WinnerName;
	std::vector<std::string> m_Stats;

	MenuState m_MenuState = MenuState::MAIN;

	void DrawMainMenu();
	void DrawColorMenu();
	void DrawStatsMenu();
	void UpdatePlayers();
};

