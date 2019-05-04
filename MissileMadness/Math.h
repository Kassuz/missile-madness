#pragma once

#include <glm/glm.hpp>

class Math
{
public:
	static glm::vec2 ClosestPointOnLineSegment(glm::vec2 point, glm::vec2 start, glm::vec2 end);

private:
	Math() = delete;
	~Math() { };
	Math(const Math& other) = delete;
	Math& operator=(const Math& other) = delete;
};

