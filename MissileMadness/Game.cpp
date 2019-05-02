#include "Game.h"

#include "Engine.h"
#include "Player.h"

#include <glm/glm.hpp>

Game::Game(UInt32 playerCount)
{
	Texture2D* playerTexture = ResourceManager::Instance().LoadTexture2D("../Character.png");

	for (UInt32 i = 0; i < playerCount; ++i)
	{
		auto p = std::make_unique<Player>(playerTexture);

		float posX = std::cos((i / (float)playerCount) * glm::two_pi<float>());
		float posY = std::sin((i / (float)playerCount) * glm::two_pi<float>());

		p->transform.SetPosition(glm::vec3(posX, posY, 0.0f) * 250.0f);

		players.push_back(std::move(p));
	}
}


Game::~Game()
{
}

void Game::Update()
{
	// Update players

	for (auto it = players.begin(); it != players.end(); ++it)
	{
		(*it)->Update();
	}
}
