#include "stdafx.h"
#include "ThreadPool.h"
#include "DL_Debug.h"
#include "TimerManager.h"
#include "ThreadNamer.h"
#include "../ThreadedPostmaster/Postmaster.h"

namespace CU
{
	Worker::Worker()
	{
		myPool = nullptr;
		myThread = nullptr;
	}

	Worker::~Worker()
	{
		if (myThread != nullptr)
		{
			if (myThread->joinable())
			{
				myThread->join();
			}

			SAFE_DELETE(myThread);
		}
	}

	void Worker::Init(ThreadPool& aThreadPool, TimerHandle aTimerHandle)
	{
		myPool = &aThreadPool;
		myThread = new std::thread(&Worker::Update, this);
		myTimerHandle = aTimerHandle;
	}


	void Worker::Stop()
	{
		myThread->join();
	}

	void Worker::Update()
	{
		bool isInited = false;
		while (true)
		{
			{
				std::unique_lock<std::mutex> lock(myPool->myQueueMutex);

				if (isInited == true)
				{
					//stop 
					myPool->myTimerMgr->UpdateTimers();
					myPool->myTimerMgr->StopTimer(myTimerHandle);
					float time = myPool->myTimerMgr->GetTimer(myTimerHandle).GetLifeTime().GetMilliseconds();
					LOG(myWork.myToWhatLog, myLogString.c_str(), time);
				}
				else 
					isInited = true;

				isActive = false;
				while (myPool->isStopped == false && 
					myPool->myLowPrioWork.size() <= 0 && 
					myPool->myNormalPrioWork.size() <= 0 && 
					myPool->myHighPrioWork.size() <= 0)
				{
					Postmaster::Threaded::CPostmaster::GetInstance().SetOfficeActive(false);
					myPool->myCondition.wait(lock); // threads pile up and wait for instructions.
					Postmaster::Threaded::CPostmaster::GetInstance().SetOfficeActive(false);
				}

				if (myPool->isStopped == true)
				{
					return;
				}
				isActive = true;
				std::queue<Work>* workQueue = nullptr;

				if (myPool->myHighPrioWork.size() > 0)
				{
#ifdef _WIN32
					SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#elif defined __linux__
					//http ://stackoverflow.com/questions/10876342/equivalent-of-setthreadpriority-on-linux-pthreads
#endif

					workQueue = &myPool->myHighPrioWork;
				}
				else if (myPool->myNormalPrioWork.size() > 0)
				{
#ifdef _WIN32
					SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
#endif
					workQueue = &myPool->myNormalPrioWork;
				}
				else if (myPool->myLowPrioWork.size() > 0)
				{
#ifdef _WIN32
					SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
#endif
					workQueue = &myPool->myLowPrioWork;
				}


				myLogString = workQueue->front().myLogMessage;
				myWork = workQueue->front();
				workQueue->pop();
				//start
				myPool->myTimerMgr->ResetTimer(myTimerHandle);
				myPool->myTimerMgr->StartTimer(myTimerHandle);
			}
			myWork.DoWork();
			CU::SetThreadName("Worker thread");
			std::this_thread::yield();
		}
	}
}
