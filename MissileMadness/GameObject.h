#pragma once

#include "Transform.h"


class GameObject
{

public:
	GameObject() : m_Transform() { }
	virtual ~GameObject() { }

	Transform* GetTransform() { return &m_Transform; }

	void SetActive(bool active) { m_IsActive = active; }
	bool IsActive() const { return m_IsActive; }

protected:
	Transform m_Transform;
	bool m_IsActive = true;
};

