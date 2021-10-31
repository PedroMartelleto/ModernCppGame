#pragma once

class Timer
{
public:
	/// <summary>
	/// Gets chrono time in seconds.
	/// </summary>
	/// <returns>Returns the time.</returns>
	static double GetTime();

	double Reset();
	double TimeElapsed();
private:
	double m_timeStart;
};

