#include "ExtraMath.h"


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

}


