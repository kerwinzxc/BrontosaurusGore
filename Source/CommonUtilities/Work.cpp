#include "stdafx.h"
#include "Work.h"
#include "DL_Debug.h"
#include "ThreadPool.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../ThreadedPostmaster/PostOffice.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "ThreadNamer.h"


namespace CU
{
	Work::Work(std::function<void()> aFunction, ePriority aPrio)
	{
		myWork = aFunction; // 2 spooky
		myPrio = aPrio;
		myLogMessage = "";
		myToWhatLog = DL_Debug::eLogTypes::eThreadPool;

		myLoopCondition = []()->bool {return false; };
	}

	Work::Work(std::function<void()> aFunction, DL_Debug::eLogTypes aToWhatLog, const char* aLogMessage, ePriority aPrio)
	{
		myWork = aFunction;
		myPrio = aPrio;
		myToWhatLog = aToWhatLog;
		myLogMessage = aLogMessage;
		myLoopCondition = []()->bool {return false; };
	}

	Work::Work(const Work& aWork)
	{
		myWork = aWork.myWork;
		myPrio = aWork.myPrio;
		myLogMessage = aWork.myLogMessage;
		myToWhatLog = aWork.myToWhatLog;
		myLoopCondition = aWork.myLoopCondition;
	}

	void Work::SetName(const std::string& aThreadName)
	{
		myThreadName = aThreadName;
	}

	Work::~Work()
	{
	}

	std::string Work::GetName()
	{
		if(myThreadName == "")
		{
			return "Unnamed";
		}
		return myThreadName;
	}

	void Work::DoWork()
	{
		SetThreadName(GetName().c_str());
		Postmaster::Threaded::CPostmaster::GetInstance().SetOfficeActive(true);
		bool loopCondition = false;
		bool threadpoolRunning = false;
		do
		{
			Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
			ThreadPool::GetInstance()->LogStart();
			myWork();
			ThreadPool::GetInstance()->LogEnd();

			std::this_thread::yield();
			loopCondition = myLoopCondition();
			threadpoolRunning = CEngine::GetInstance()->GetThreadPool()->IsRunning();
		} while (loopCondition && threadpoolRunning == true);
		Postmaster::Threaded::CPostmaster::GetInstance().SetOfficeActive(false);
	}

	void Work::AddLoopCondition(const std::function<bool()>& aFunction)
	{
		myLoopCondition = aFunction;
	}
}
