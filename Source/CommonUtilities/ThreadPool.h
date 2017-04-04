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
		static void Create();
		static void Destroy();
		static ThreadPool* GetInstance();

		void Init();
		void AddWork(Work aTask);
		void LogStart();
		void LogEnd();
		void LogName(const std::string& aThreadName);
		bool IsRunning() const;
		void LogCreateThread();
		void LogDestroyThread();
	private:
		static ThreadPool* ourInstance;

		ThreadPool();
		~ThreadPool();

		unsigned long long GetEpochTicks();
		void CreateDirectoryRecursive(const std::string& aPath);
		std::ofstream GetLogStream();
		friend class Worker; //I always let the workers touch my privates
		std::vector<Worker*> myWorkers;

		std::queue<Work> myLowPrioWork;
		std::queue<Work> myNormalPrioWork;
		std::queue<Work> myHighPrioWork;

		std::mutex myQueueMutex;
		std::condition_variable myCondition;
		std::atomic_bool isStopped;
		TimerManager* myTimerMgr;
		std::string myLogPath;
		unsigned myConcurentThreadsSupported;
	};

}