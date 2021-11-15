#include "Timer.h"
#include "Utils.h"
#include <chrono>

static const auto epoch = std::chrono::high_resolution_clock::now();

Timestamp Timer::GetTime()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - epoch).count() / 1000000000.0;
}

Timestamp Timer::Reset()
{
	m_timeStart = Timer::GetTime();
	return m_timeStart;
}

Timestamp Timer::TimeElapsed()
{
	return Timer::GetTime() - m_timeStart;
}