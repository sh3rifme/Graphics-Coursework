#pragma once

#include <chrono>

class TimeManager
{
public:
	TimeManager(void);
	~TimeManager(void);

	double CalcFPS();
	double GetTime();

	double deltaTime;
	double currentTime;
};

