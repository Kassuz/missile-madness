#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Types.h"

class Player;
class Texture2D;

class Game
{

public:
	Game(UInt32 screenWidth, UInt32 screenHeight, UInt32 playerCount);
	~Game();

	void Update();

private:
	std::vector<Player*> players;


	struct Wall
	{
	public:
		glm::vec2 GetStart()  const { return start; }
		glm::vec2 GetEnd()    const { return end; }
		glm::vec2 GetNormal() const { return normal; }

		Wall() : start(glm::vec2()), end(glm::vec2()), normal(glm::vec2())
		{}
		Wall(glm::vec2 start, glm::vec2 end, glm::vec2 normal) : start(start), end(end), normal(normal)
		{}
		Wall& operator=(const Wall& ohter)
		{
			this->start = ohter.start;
			this->end = ohter.end;
			this->normal = ohter.normal;
			return *this;
		}
		~Wall() {}
	private:
		glm::vec2 start, end, normal;
	};

	Wall walls[4];
	
	const float playerColliderRadius = 35.0f;
	const float missileColliderRadius = 10.0f;
	const float collisionPushForce = 12.0f;

	void HandleWallCollisions();
	void HandlePlayerToPlayerCollisions();
	void HandleProjectileCollisons();

	const float respawnTime = 3.0f;
	std::unordered_map<Player*, float> respawns;
	void RespawnPlayers();

	float frameTimes[50];
	int currentframe;
};

