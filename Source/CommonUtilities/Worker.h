#pragma once
#include <thread>
#include "DynamicString.h"

namespace CU
{
	class Work;
	class ThreadPool;

	typedef unsigned int TimerHandle;

	class Worker
	{
	public:
		Worker();
		~Worker();
		void Init(ThreadPool& aThreadPool, TimerHandle aTimerHandle);
		void Stop();
		inline bool GetIsActive() { return isActive; }

	private:
		void Update();
		friend class ThreadPool;

		Work myWork;
		std::thread *myThread;
		ThreadPool *myPool;
		bool isActive;
		DynamicString myLogString;
		TimerHandle myTimerHandle;
	};
}
