#pragma once

#include "pch.h"
#include <limits>

IFNITY_NAMESPACE

class IFNITY_API FpsCounter
{
public:
	explicit FpsCounter(float avgInterval = 0.5f)
		: m_AvgInterval(avgInterval)
	{
		assert(avgInterval > 0.0f);
	}

	bool tick(float deltaSeconds, bool frameRendered = true)
	{
		if(frameRendered)
			m_NumFrames++;

		m_AccumulatedTime += deltaSeconds;

		if(m_AccumulatedTime > m_AvgInterval)
		{
			m_CurrentFPS = static_cast<float>(m_NumFrames / m_AccumulatedTime);
			m_TotalFPS += m_CurrentFPS;
			m_FrameCount++;

			if(m_CurrentFPS > m_MaxFPS)
				m_MaxFPS = m_CurrentFPS;
			if(m_CurrentFPS < m_MinFPS)
				m_MinFPS = m_CurrentFPS;

			if(printFPS_)
				IFNITY_LOG(LogApp, TRACE, "FPS: " + std::to_string(m_CurrentFPS) +
					" MaxFPS: " + std::to_string(m_MaxFPS) +
					" MinFPS: " + std::to_string(m_MinFPS) +
					" AVG: " + std::to_string(getAverageFPS()));

			m_NumFrames = 0;
			m_AccumulatedTime = 0;

			// Verificar posible desbordamiento
			if(m_TotalFPS > std::numeric_limits<double>::max() - m_CurrentFPS)
			{
				m_TotalFPS = 0;
				m_FrameCount = 0;
			}

			return true;
		}

		return false;
	}

	inline float getFPS() const { return m_CurrentFPS; }
	inline float getAverageFPS() const { return m_FrameCount > 0 ? static_cast<float>(m_TotalFPS / m_FrameCount) : 0.0f; }
	inline float getMaxFPS() const { return m_MaxFPS; }
	inline float getMinFPS() const { return m_MinFPS; }

	bool printFPS_ = true;

private:
	const float m_AvgInterval = 0.5f;
	unsigned int m_NumFrames = 0;
	double m_AccumulatedTime = 0;
	float m_CurrentFPS = 0.0f;
	double m_TotalFPS = 0.0;
	unsigned int m_FrameCount = 0;
	float m_MaxFPS = 0.0f;
	float m_MinFPS = std::numeric_limits<float>::max();
};


IFNITY_END_NAMESPACE