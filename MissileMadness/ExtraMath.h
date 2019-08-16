#pragma once

#include <glm/glm.hpp>
#include "Types.h"

namespace ExtraMath
{
	glm::vec2 ClosestPointOnLineSegment(glm::vec2 point, glm::vec2 start, glm::vec2 end);

	// Returns random float between 0.0f and 1.0f
	float GetRandomFloat();

	template <typename T>
	class AverageValue
	{
	public:
		AverageValue(UInt32 count) : m_ValueCount(count)
		{
			m_Values = new T[count];
			m_Counter = 0U;

			m_AverageValue = 0;
		}

		~AverageValue()
		{
			delete[] m_Values;
		}

		void AddValue(T newValue)
		{
			m_Values[m_Counter++ % m_ValueCount] = newValue;

			UInt32 count = m_Counter >= m_ValueCount ? m_ValueCount : m_Counter;
			T sum = 0;
			for (UInt32 i = 0U; i < count; ++i)
			{
				sum += m_Values[i];
			}
			m_AverageValue = sum / static_cast<T>(count);
		}

		T GetAverage() const { return m_AverageValue; }

	private:
		T* m_Values;
		UInt32 m_ValueCount;
		UInt32 m_Counter;

		T m_AverageValue;
	};

}

