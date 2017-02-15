#include "stdafx.h"
#include "Work.h"
#include "DL_Debug.h"


namespace CU
{
	Work::Work(std::function<void()> aFunction, ePriority aPrio)
	{
		myWork = aFunction; // 2 spooky
		myPrio = aPrio;
		myLogMessage = "";
		myToWhatLog = DL_Debug::eLogTypes::eThreadPool;
	}

	Work::Work(std::function<void()> aFunction, DL_Debug::eLogTypes aToWhatLog, const char* aLogMessage, ePriority aPrio)
	{
		myWork = aFunction;
		myPrio = aPrio;
		myToWhatLog = aToWhatLog;
		myLogMessage = aLogMessage;
	}

	Work::Work(const Work& aWork)
	{
		myWork = aWork.myWork;
		myPrio = aWork.myPrio;
		myLogMessage = aWork.myLogMessage;
		myToWhatLog = aWork.myToWhatLog;
	}

	Work::~Work()
	{
	}

	void Work::DoWork()
	{
		myWork();
	}

}