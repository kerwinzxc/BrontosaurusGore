#pragma once
#include <stdlib.h>
#include <functional>
#include <atomic>
#include <thread>

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
		Work() {}
		void SetName(const std::string& aThreadName);
	
		~Work();

		std::string GetName();
		void DoWork();
		inline ePriority GetPriority();
		void AddLoopCondition(const std::function<bool()>& aFunction);
		std::string myLogMessage;
		DL_Debug::eLogTypes myToWhatLog;
	private:
		std::string myThreadName;

		std::function<bool(void)> myLoopCondition;
		ePriority myPrio;
		std::function < void() > myWork;

	};

	ePriority Work::GetPriority()
	{
		return  myPrio;
	}

}