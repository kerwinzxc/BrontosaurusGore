#include "stdafx.h"
#include "CUTime.h"

#define self (*this)

namespace CU
{
	Time::Time(const TimeUnit aTime)
	{
		myTime = aTime;
	}

	Time::~Time()
	{
	}

	Time Time::operator+(const Time& aRight) const
	{
		return Time(*this) += aRight;
	}

	Time& Time::operator+=(const Time& aRight)
	{
		myTime += aRight.myTime;
		return *this;
	}
	
	bool Time::operator<(const Time& aRight) const
	{
		return myTime < aRight.myTime;
	}

	bool Time::operator>(const Time& aRight) const
	{
		return !(self < aRight);
	}
}

#undef self