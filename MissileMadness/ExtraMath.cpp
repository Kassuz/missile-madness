#include "ExtraMath.h"

#include <random>

namespace ExtraMath
{
	glm::vec2 ClosestPointOnLineSegment(glm::vec2 point, glm::vec2 start, glm::vec2 end)
	{
		glm::vec2 ab = end - start;
		float t = glm::dot(point - start, ab) / glm::dot(ab, ab);
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;

		return start + t * ab;
	}

	float GetRandomFloat()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution< float > dis(0.0f, 1.0f);
		return dis(gen);
	}
}


