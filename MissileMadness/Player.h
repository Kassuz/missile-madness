#pragma once

#include "GameObject.h"
#include "Sprite.h"

class Texture2D;

class Player : public GameObject
{
public:
	Player(Texture2D*);
	virtual ~Player();

	void Update();

	Sprite sprite;

private:
	float health = 100.0f;

	float maxSpeed = 350.0f;
	float rotateSpeed = 0.05f;

	float acceleration = 1000.0f;
	glm::vec3 velocity = glm::vec3(0.0f);
};

