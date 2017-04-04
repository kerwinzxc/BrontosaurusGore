#include "stdafx.h"
#include "Work.h"
#include "DL_Debug.h"
#include "ThreadPool.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../ThreadedPostmaster/PostOffice.h"
#include "../ThreadedPostmaster/Postmaster.h"


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
		myLoopCondition = []()->bool {return false; };
	}

	Work::~Work()
	{
	}

	void Work::DoWork()
	{
		Postmaster::Threaded::CPostmaster::GetInstance().SetOfficeActive(true);
		do
		{
			Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
			CEngine::GetInstance()->GetThreadPool()->LogStart();
			myWork();
			CEngine::GetInstance()->GetThreadPool()->LogEnd();
		} while (myLoopCondition() == true && CEngine::GetInstance()->GetThreadPool()->IsRunning() == true);
		Postmaster::Threaded::CPostmaster::GetInstance().SetOfficeActive(false);
	}

}
