#pragma once

class LobbyMenu
{
public:
	enum class MenuState { MAIN, COLOR, STATS };

	LobbyMenu() { }
	~LobbyMenu() { }

	void Draw();
	void SetDirty() { m_IsDirty = true; }

	void CheckInput();

private:
	bool m_IsDirty = true;

	MenuState m_MenuState = MenuState::MAIN;

	void DrawMainMenu();
	void DrawColorMenu();
	void DrawStatsMenu();

};

