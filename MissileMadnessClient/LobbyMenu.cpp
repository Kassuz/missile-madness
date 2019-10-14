#include "LobbyMenu.h"

#include "Engine.h"
#include "LobbyPlayer.h"
#include "LobbyManagerClient.h"

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

	UpdatePlayers();

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
			//Debug::Log("Player is ready");
			m_LobbyManager->SetReady();
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
			m_LobbyManager->ChangeUserColor(Color::Black());
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_2))
		{
			Debug::Log("Color White");
			m_LobbyManager->ChangeUserColor(Color::White());
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_3))
		{
			Debug::Log("Color red");
			m_LobbyManager->ChangeUserColor(Color::Red());
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_4))
		{
			Debug::Log("Color green");
			m_LobbyManager->ChangeUserColor(Color::Green());
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_5))
		{
			Debug::Log("Color blue");
			m_LobbyManager->ChangeUserColor(Color::Blue());
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_6))
		{
			Debug::Log("Color yellow");
			m_LobbyManager->ChangeUserColor(Color::Yellow());
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_7))
		{
			Debug::Log("Color cyan");
			m_LobbyManager->ChangeUserColor(Color::Cyan());
		}
		else if (InputManager::GetKeyDown(GLFW_KEY_8))
		{
			Debug::Log("Color magenta");
			m_LobbyManager->ChangeUserColor(Color::Magenta());
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

void LobbyMenu::AddNewPlayer(User* u)
{
	m_PlayerSprites.emplace_back(new LobbyPlayer(u));
	m_IsDirty = true;
}

void LobbyMenu::DeletePlayer(User* u)
{
	for (auto it = m_PlayerSprites.begin(); it != m_PlayerSprites.end(); ++it)
	{
		if ((*it)->GetUser() == u)
		{
			delete (*it);
			m_PlayerSprites.erase(it);
			m_IsDirty = true;
			return;
		}
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

void LobbyMenu::UpdatePlayers()
{
	float startX = 150.0f, xIncrease =  150.0f;
	float startY = 100.0f, yIncrease = -100.0f;

	for (int i = 0; i < m_PlayerSprites.size(); ++i)
	{
		LobbyPlayer* pl = m_PlayerSprites[i];
		pl->GetTransform()->SetPosition(glm::vec3(startX + (i % 2) * xIncrease, startY + i * yIncrease, 0.0f));

		pl->Update();
	}
}


