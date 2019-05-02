#include "Player.h"
#include "Engine.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Player::Player(Texture2D* playerTexture) : sprite(playerTexture, Color::White(), &this->transform, 0)
{
	transform.SetScale(glm::vec3(100.0f));
}


Player::~Player()
{
}

void Player::Update()
{
	// MOVEMENT
	if (InputManager::Instance().GetKey(GLFW_KEY_D) || InputManager::Instance().GetKey(GLFW_KEY_RIGHT))
	{
		glm::quat rot = glm::angleAxis(-rotateSpeed, glm::vec3(0.0f, 0.0f, 1.0f)) * Time::deltaTime;
		transform.Rotate(rot);
	}

	if (InputManager::Instance().GetKey(GLFW_KEY_A) || InputManager::Instance().GetKey(GLFW_KEY_LEFT))
	{
		glm::quat rot = glm::angleAxis(rotateSpeed, glm::vec3(0.0f, 0.0f, 1.0f)) * Time::deltaTime;
		transform.Rotate(rot);
	}

	if (InputManager::Instance().GetKey(GLFW_KEY_W) || InputManager::Instance().GetKey(GLFW_KEY_UP))
		velocity += transform.GetUp() * Time::deltaTime * acceleration;
	
	if (InputManager::Instance().GetKey(GLFW_KEY_S) || InputManager::Instance().GetKey(GLFW_KEY_DOWN))
		velocity -= transform.GetUp() * Time::deltaTime * acceleration;

	velocity = glm::clamp(velocity, -maxSpeed, maxSpeed);
	transform.Move(velocity * Time::deltaTime);

	velocity *= 0.98f;

}
