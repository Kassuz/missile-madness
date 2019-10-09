#include "LobbyMenu.h"

#include "Engine.h"

void LobbyMenu::Draw()
{
	if (!m_IsDirty) return;

	TextRenderer::Instance().RenderText("Missile Madness", 10.0f, 10.0f, 3.0f);

	switch (m_MenuState)
	{
	case LobbyMenu::MenuState::MAIN:
		DrawMainMenu();
		break;
	case LobbyMenu::MenuState::COLOR:
		DrawColorMenu();
		break;
	case LobbyMenu::MenuState::STATS:
		DrawStatsMenu();
		break;
	default:
		break;
	}

	RenderingEngine::Render();

	m_IsDirty = false;
}

void LobbyMenu::CheckInput()
{
	switch (m_MenuState)
	{
	case LobbyMenu::MenuState::MAIN:
	{
		if (InputManager::GetKeyDown(GLFW_KEY_1))
		{
			Debug::Log("Player is ready");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_2))
		{
			m_MenuState = MenuState::COLOR;
			SetDirty();
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_3))
		{
			m_MenuState = MenuState::STATS;
			SetDirty();
		}
		break;
	}
	case LobbyMenu::MenuState::COLOR:
	{
		if (InputManager::GetKeyDown(GLFW_KEY_1))
		{
			Debug::Log("Color black");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_2))
		{
			Debug::Log("Color White");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_3))
		{
			Debug::Log("Color red");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_4))
		{
			Debug::Log("Color green");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_5))
		{
			Debug::Log("Color blue");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_6))
		{
			Debug::Log("Color yellow");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_7))
		{
			Debug::Log("Color cyan");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_8))
		{
			Debug::Log("Color magenta");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_9))
		{
			m_MenuState = MenuState::MAIN;
			SetDirty();
		}
		break;
	}
	case LobbyMenu::MenuState::STATS:
	{
		if (InputManager::GetKeyDown(GLFW_KEY_1))
		{
			Debug::Log("All time stats");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_2))
		{
			Debug::Log("Last match stats");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_3))
		{
			Debug::Log("Last 5 matches stats");
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_4))
		{
			m_MenuState = MenuState::MAIN;
			SetDirty();
		}
		break;
	}
	default:
		break;
	}
}

void LobbyMenu::DrawMainMenu()
{
	TextRenderer::Instance().RenderText("Options:", 10.0f, 100.0f, 1.5f);
	TextRenderer::Instance().RenderText("----------------", 10.0f, 125.0f, 1.5f);
	TextRenderer::Instance().RenderText("1. Ready / Unready", 10.0f, 160.0f, 1.0f);
	TextRenderer::Instance().RenderText("2. Change Colors", 10.0f, 195.0f, 1.0f);
	TextRenderer::Instance().RenderText("3. See stats", 10.0f, 230.0f, 1.0f);
	TextRenderer::Instance().RenderText("----------------", 10.0f, 250.0f, 1.5f);
}

void LobbyMenu::DrawColorMenu()
{
	TextRenderer::Instance().RenderText("Change Color:", 10.0f, 100.0f, 1.5f);
	TextRenderer::Instance().RenderText("----------------", 10.0f, 125.0f, 1.5f);
	TextRenderer::Instance().RenderText("1. Black", 10.0f, 160.0f, 1.0f);
	TextRenderer::Instance().RenderText("2. White", 10.0f, 195.0f, 1.0f);
	TextRenderer::Instance().RenderText("3. Red", 10.0f, 230.0f, 1.0f);
	TextRenderer::Instance().RenderText("4. Green", 10.0f, 265.0f, 1.0f);
	TextRenderer::Instance().RenderText("5. Blue", 10.0f, 300.0f, 1.0f);
	TextRenderer::Instance().RenderText("6. Yellow", 10.0f, 335.0f, 1.0f);
	TextRenderer::Instance().RenderText("7. Cyan", 10.0f, 370.0f, 1.0f);
	TextRenderer::Instance().RenderText("8. Magenta", 10.0f, 405.0f, 1.0f);

	TextRenderer::Instance().RenderText("9. Return", 10.0f, 440.0f, 1.0f);

	TextRenderer::Instance().RenderText("----------------", 10.0f, 460.0f, 1.5f);
}

void LobbyMenu::DrawStatsMenu()
{
	TextRenderer::Instance().RenderText("Stats:", 10.0f, 100.0f, 1.5f);
	TextRenderer::Instance().RenderText("----------------", 10.0f, 125.0f, 1.5f);
	TextRenderer::Instance().RenderText("1. All time", 10.0f, 160.0f, 1.0f);
	TextRenderer::Instance().RenderText("2. Last match", 10.0f, 195.0f, 1.0f);
	TextRenderer::Instance().RenderText("3. Last 5 matches", 10.0f, 230.0f, 1.0f);
	TextRenderer::Instance().RenderText("4. Return", 10.0f, 265.0f, 1.0f);
	TextRenderer::Instance().RenderText("----------------", 10.0f, 285.0f, 1.5f);
}


