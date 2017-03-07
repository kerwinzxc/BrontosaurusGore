#pragma once
#include <thread>


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
		std::string myLogString;
		TimerHandle myTimerHandle;
	};
}
