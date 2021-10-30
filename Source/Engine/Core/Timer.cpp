#include "Timer.h"

double Timer::Reset()
{
	m_timeStart = GetTime();
	return m_timeStart;
}

double Timer::TimeElapsed()
{
	return GetTime() - m_timeStart;
}