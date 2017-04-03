#pragma once
#include <stdlib.h>
#include <functional>
#include <atomic>

namespace DL_Debug
{
	enum class eLogTypes;
}

namespace CU
{
	enum class ePriority //TODO: Impl priority functionality.
	{
		eHigh,
		eStandard,
		eLow
	};

	class Work
	{
	public:
		Work(std::function<void()> aFunction, ePriority aPrio = ePriority::eStandard);
		Work(std::function<void()> aFunction, DL_Debug::eLogTypes aToWhatLog, const char* aLogMessage, ePriority aPrio = ePriority::eStandard);
		Work(const Work& aWork);
		Work() {};
		~Work();

		void DoWork();
		inline ePriority GetPriority();

		std::string myLogMessage;
		DL_Debug::eLogTypes myToWhatLog;
	private:

		std::function<bool(void)> myLoopCondition;
		ePriority myPrio;
		std::function < void() > myWork;

	};

	ePriority Work::GetPriority()
	{
		return  myPrio;
	}

}