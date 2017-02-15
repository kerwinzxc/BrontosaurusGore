#include "stdafx.h"
#include "Timer.h"

namespace CU
{
	Timer::Timer()
	{
		myLifeTime = 0;
		myDeltaTime = 0;
		myOn = true;
	}


	Timer::~Timer()
	{
	}

	void Timer::AddTime(const TimeUnit aTime)
	{
		if (myOn == true)
		{
			myLifeTime += aTime;
			myDeltaTime = aTime;
		}
	}
}