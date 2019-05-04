#pragma once

#include "Transform.h"


class GameObject
{

public:
	GameObject() : transform() { }
	virtual ~GameObject() { }

	Transform* GetTransform() { return &transform; }

	void SetActive(bool active) { isActive = active; }
	bool IsActive() const { return isActive; }

protected:
	Transform transform;
	bool isActive = true;
};

