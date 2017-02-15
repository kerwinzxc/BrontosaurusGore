#include "stdafx.h"
#include "StopWatch.h"

namespace CU
{
	CStopWatch::CStopWatch()
		: myLastTimePoint(Clock::now())
		, myLifeTime(0.f)
		, myDeltaTime(0.f)
		, myIsPaused(true)
	{
	}

	CStopWatch::~CStopWatch()
	{
	}

	void CStopWatch::Init()
	{
		Restart();
	}

	void CStopWatch::Start()
	{
		myIsPaused = false;
	}

	void CStopWatch::Restart()
	{
		myLastTimePoint = Clock::now();
		myLifeTime.Reset();
		myDeltaTime.Reset();
		myIsPaused = false;
	}

	void CStopWatch::Pause()
	{
		myIsPaused = true;
	}

	void CStopWatch::Stop()
	{
		Pause();
		myLifeTime.Reset();
		myDeltaTime.Reset();
	}

	void CStopWatch::Update()
	{
		if (!myIsPaused)
		{
			TimePoint newTimePoint = Clock::now();
			FloatDuration rawDeltaTime = newTimePoint - myLastTimePoint;
			MicroSeconds deltaTime = std::chrono::duration_cast<MicroSeconds>(rawDeltaTime);

			myDeltaTime = static_cast<TimeUnit>(deltaTime.count());
			myLifeTime += myDeltaTime;

			myLastTimePoint = newTimePoint;
		}
	}
}
