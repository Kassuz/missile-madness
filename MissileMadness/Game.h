#pragma once

#include <vector>
#include <memory>
#include "Types.h"

class Player;
class Texture2D;

class Game
{

public:
	Game(UInt32 playerCount);
	~Game();

	void Update();

private:
	std::vector<std::unique_ptr<Player>> players;

};

