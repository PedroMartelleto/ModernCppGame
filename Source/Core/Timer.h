#pragma once
#include "raylib.h"

class Timer
{
public:
	double Reset();
	double TimeElapsed();
private:
	double m_timeStart;
};

