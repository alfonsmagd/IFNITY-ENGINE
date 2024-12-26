

#include "FpsCounter.hpp"

IFNITY_NAMESPACE

FpsCounter::FpsCounter(float avgInterval)
    : m_AvgInterval(avgInterval)
{
    assert(avgInterval > 0.0f);
}

bool FpsCounter::tick(float deltaSeconds, bool frameRendered)
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

IFNITY_END_NAMESPACE
