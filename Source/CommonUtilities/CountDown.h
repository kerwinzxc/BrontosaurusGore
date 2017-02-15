#pragma once

#include "stdafx.h"
#include <ctime>

class CTextInstance;

namespace CU
{
	class DynamicString;

	class CountDown
	{
	public:
		CountDown();
		~CountDown();

		void Update();
		void Render();

	private:
		CTextInstance* myOutputText;

		time_t myGoalTime;
		time_t myCurrentTime;
	};
}
