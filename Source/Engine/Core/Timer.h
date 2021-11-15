#pragma once

using Timestamp = double;

class Timer
{
public:
	/// <summary>
	/// Gets chrono time in seconds.
	/// </summary>
	/// <returns>Returns the time.</returns>
	static Timestamp GetTime();

	Timestamp Reset();
	Timestamp TimeElapsed();
private:
	Timestamp m_timeStart;
};

