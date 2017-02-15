#include "stdafx.h"
#include "ThreadPool.h"
#include "TimerManager.h"
#include "ThreadNamer.h"

namespace CU
{
	ThreadPool::ThreadPool()
	{
		const static std::string name = "ThreadPool Worker: ";
		myTimerMgr = new TimerManager();
		isStopped = false;
		unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
		for (std::size_t i = 0; i < concurentThreadsSupported; ++i)
		{

			TimerHandle handle = myTimerMgr->CreateTimer();
			Worker *worker = new Worker();
			worker->Init(*this, handle);
			myWorkers.push_back(worker);

			//so very windows
			std::string threadName = name + std::to_string(i);
			SetThreadName(*worker->myThread,threadName.c_str());

		}
	}

	ThreadPool::~ThreadPool()
	{
		isStopped = true;
		myCondition.notify_all();

		for (std::size_t i = 0; i < myWorkers.size(); ++i)
		{
			myWorkers[i]->Stop();
			SAFE_DELETE(myWorkers[i]);
		}
		delete (myTimerMgr);
		myTimerMgr = nullptr;
	}

	void ThreadPool::AddWork(Work aWork)
	{
		std::queue<Work>* workQueue = &myNormalPrioWork;
		switch (aWork.GetPriority())
		{
		case ePriority::eLow:
			workQueue = &myLowPrioWork;
			break;
		case ePriority::eHigh:
			workQueue = &myHighPrioWork;
			break;
		}


		std::unique_lock<std::mutex> lock(myQueueMutex);
		workQueue->push(aWork);
		myCondition.notify_one();
	}
}