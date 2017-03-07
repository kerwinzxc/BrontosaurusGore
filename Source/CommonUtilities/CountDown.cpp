#include "stdafx.h"

#include "CountDown.h"
#include "..\BrontosaurusEngine\TextInstance.h"

namespace CU
{
#ifndef _RETAIL_BUILD

	CountDown::CountDown()
		: myOutputText(nullptr)
		, myGoalTime(0)
		, myCurrentTime(0)
	{
		time(&myCurrentTime);

		tm goalTime = {};

		goalTime.tm_sec = 0;
		goalTime.tm_min = 0;
		goalTime.tm_hour = 22;
		goalTime.tm_mday = 8;
		goalTime.tm_mon = 1;
		goalTime.tm_year = 117;
		goalTime.tm_wday = 0;
		goalTime.tm_yday = 0;
		goalTime.tm_isdst = 0;

		myGoalTime = mktime(&goalTime);

		myOutputText = new CTextInstance();
		myOutputText->Init();
		myOutputText->SetPosition(CU::Vector2f(0.1f, 0.9f));
		myOutputText->SetText("Time left until deadline:");
	}

	CountDown::~CountDown()
	{
		SAFE_DELETE(myOutputText);
	}

	void CountDown::Update()
	{
		if (myOutputText != nullptr)
		{
			time(&myCurrentTime);

			int seconds = static_cast<int>(myGoalTime - myCurrentTime);

			int minutes = seconds / 60;
			seconds -= minutes * 60;
			int hours = minutes / 60;
			minutes -= hours * 60;
			int days = hours / 24;
			hours -= days * 24;
			int years = days / 365;
			days -= years * 365;

			if (days < 2)
			{
				myOutputText->SetColor(CTextInstance::Red);
			}
			else if (days < 4)
			{
				myOutputText->SetColor(CTextInstance::Yellow);
			}

			std::string timeString = "";
			if (years > 0)
			{
				timeString += years;
				timeString += " years, ";
			}
			timeString += days;
			if (days > 1)
			{
				timeString += " days, ";
			}
			else
			{
				timeString += " day, ";
			}
			timeString += hours;
			timeString += " hours, ";
			timeString += minutes;
			timeString += " minutes, and ";
			timeString += seconds;
			timeString += " seconds.";

			myOutputText->SetTextLine(1, std::move(timeString));
		}
	}

	void CountDown::Render()
	{
		if (myCurrentTime < myGoalTime && myOutputText != nullptr)
		{
			myOutputText->Render();
		}
	}

#else

	CountDown::CountDown()
		: myOutputText(nullptr)
		, myGoalTime(0)
		, myCurrentTime(0)
	{
	}

	CountDown::~CountDown()
	{
	}

	void CountDown::Update()
	{
	}

	void CountDown::Render()
	{
	}

#endif // !_RETAIL_BUILD
}