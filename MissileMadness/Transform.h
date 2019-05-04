#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
	Transform();

	Transform* GetParent();
	void SetParent(Transform* t);

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);
	void Translate(glm::vec3 movement);

	glm::vec3 GetScale() const;
	void SetScale(glm::vec3 scale);
	void Scale(glm::vec3 scale);

	glm::quat GetRotation() const;
	void SetRotation(glm::quat rot);
	void Rotate(glm::quat rot);

	glm::vec3 GetForward() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetRight() const;

	// Returns the correct matrix
	// If matrix is invalidated, calculates new
	glm::mat4 GetModel();

private:
	Transform* parent;

	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;

	glm::mat4 model;

	bool invalidMatrix;
};

