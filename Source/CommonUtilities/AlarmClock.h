#pragma once

#include "CUTime.h"
#include "StopWatch.h"
#include <functional>

namespace CU
{
	class CAlarmClock
	{
	public:
		CAlarmClock();
		CAlarmClock(const Time aTimeTillRinging, const std::function<void(void)>& aCallback);
		~CAlarmClock();

		void Init(const Time aTimeTillRinging, const std::function<void(void)>& aCallback);
		void Update();

	private:
		std::function<void(void)> myCallback;
		CStopWatch myStopWatch;
		Time myDoneTime;
	};
}
