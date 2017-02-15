#pragma once
#include "Work.h"
#include "Worker.h"
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable> // might be cooleo, otherwise removeio

namespace CU
{
	class TimerManager;
	typedef unsigned int TimerHandle;

	class ThreadPool
	{
	public:
		ThreadPool();
		~ThreadPool();
		void AddWork(Work aTask);

	private:
		friend class Worker; //I always let the workers touch my privates
		std::vector<Worker*> myWorkers;

		std::queue<Work> myLowPrioWork;
		std::queue<Work> myNormalPrioWork;
		std::queue<Work> myHighPrioWork;

		std::mutex myQueueMutex;
		std::condition_variable myCondition;
		std::atomic_bool isStopped;
		TimerManager* myTimerMgr;
	};

}