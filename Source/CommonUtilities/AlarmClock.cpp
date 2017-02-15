#include "stdafx.h"
#include "AlarmClock.h"

#include <thread>

namespace CU
{
	CAlarmClock::CAlarmClock()
		: myCallback(nullptr)
		, myDoneTime(0.f)
	{
	}

	CAlarmClock::CAlarmClock(const Time aTimeTillRinging, const std::function<void(void)>& aCallback)
		: CAlarmClock()
	{
		Init(aTimeTillRinging, aCallback);
	}

	CAlarmClock::~CAlarmClock()
	{
	}

	void CU::CAlarmClock::Init(const Time aTimeTillRinging, const std::function<void(void)>& aCallback)
	{
		myDoneTime = aTimeTillRinging;
		myCallback = aCallback;

		myStopWatch.Init();
		//std::thread updateThread(std::bind(&CAlarmClock::Update, this));
		//updateThread.detach();
	}

	void CU::CAlarmClock::Update()
	{
		/*while*/if (myCallback != nullptr)
		{
			myStopWatch.Update();
			if (myStopWatch.GetLifeTime() > myDoneTime)
			{
				if (myCallback != nullptr)
				{
					myCallback();
					myCallback = nullptr;
					myStopWatch.Pause();
				}
			}
		}
	}
}
