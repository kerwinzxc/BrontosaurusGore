#pragma once

namespace CU
{
	typedef float TimeUnit; // is this wierd? // no mvh carl :)

	class Time
	{
	public:
		Time(const TimeUnit someTime);
		~Time();

		Time operator+(const Time& aRight) const;
		Time& operator+=(const Time& aRight);
		bool operator<(const Time& aRight) const;
		bool operator>(const Time& aRight) const;

		inline void Reset();

		inline void SetSeconds(const TimeUnit aSeconds);

		inline TimeUnit GetHours() const;
		inline TimeUnit GetMinutes() const;
		inline TimeUnit GetSeconds() const;
		inline TimeUnit GetMilliseconds() const;
		inline TimeUnit GetMicroseconds() const;
		
	private:
		TimeUnit myTime;
	};

	inline void Time::Reset()
	{
		myTime = 0.f;
	}

	inline void Time::SetSeconds(const TimeUnit aSeconds)
	{
		myTime = aSeconds * 1000.f * 1000.f;
	}

	inline TimeUnit Time::GetHours() const
	{
		return 0;//myTime / (60000000 * 60);
	}

	inline TimeUnit Time::GetMinutes() const
	{
		return myTime / 60000000;
	}

	inline TimeUnit Time::GetSeconds() const
	{
		return myTime / 1000000;
	}

	inline TimeUnit Time::GetMilliseconds() const
	{
		return myTime / 1000;
	}

	inline TimeUnit Time::GetMicroseconds() const
	{
		return myTime;
	}
}  