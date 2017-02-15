#pragma once
#include "CUTime.h"

namespace CU
{
	class Timer
	{
	public:
		Timer();
		~Timer();

		void AddTime(const TimeUnit aTime);
		void Start();
		void Stop();
		void Reset();
		Time GetLifeTime() const;
		Time GetDeltaTime() const;
		TimeUnit GetFPS() const;

	private:
		TimeUnit myLifeTime;
		TimeUnit myDeltaTime;
		bool myOn;
	};

	inline void Timer::Start()
	{
		myOn = true;
	}

	inline void Timer::Stop()
	{
		myOn = false;
	}

	inline void Timer::Reset()
	{
		myLifeTime = 0;
		myDeltaTime = 0;
	}

	inline Time Timer::GetLifeTime() const
	{
		return Time(static_cast<TimeUnit>(myLifeTime));
	}

	inline Time Timer::GetDeltaTime() const
	{
		return Time(static_cast<TimeUnit>(myDeltaTime));
	}

	inline TimeUnit Timer::GetFPS() const
	{
		return 1000000.f / myDeltaTime;
	}
} 