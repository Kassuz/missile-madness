#include "Transform.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform() : position(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
{
	model = glm::mat4();
	model = glm::scale(model, scale);
	model = glm::toMat4(rotation) * model;
	model = glm::translate(model, position);
	invalidMatrix = false;
}

Transform* Transform::GetParent()
{
	return parent;
}

void Transform::SetParent(Transform* t)
{
	this->parent = t;
}

glm::vec3 Transform::GetPosition() const
{
	return position;
}

void Transform::SetPosition(glm::vec3 pos)
{
	this->position = pos;
	invalidMatrix = true;
}

glm::vec3 Transform::GetScale() const
{
	return scale;
}

void Transform::SetScale(glm::vec3 scale)
{
	this->scale = scale;
	invalidMatrix = true;
}

glm::quat Transform::GetRotation() const
{
	return rotation;
}

void Transform::SetRotation(glm::quat rot)
{
	this->rotation = glm::normalize(rot);
	invalidMatrix = true;
}

glm::mat4 Transform::GetModel()
{
	if (invalidMatrix)
	{
		model = glm::mat4();
		model = glm::translate(model, position) * glm::toMat4(rotation) * glm::scale(model, scale);
		//model = glm::scale(model, scale);
		//model = glm::toMat4(rotation) * model;
		//model = glm::translate(model, position);
		invalidMatrix = false;
	}

	if (parent != nullptr)
		return parent->GetModel() * model;
	else
		return model;
}