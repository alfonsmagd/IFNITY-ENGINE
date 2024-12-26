#pragma once

#include "pch.h"
#include <limits>

IFNITY_NAMESPACE

class IFNITY_API FpsCounter
{
public:
	explicit FpsCounter(float avgInterval = 0.5f);

	bool tick(float deltaSeconds, bool frameRendered = true);

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