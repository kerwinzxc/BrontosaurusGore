#include "stdafx.h"
#include "ThreadPool.h"
#include "TimerManager.h"
#include "ThreadNamer.h"
#include <sstream>
#include "StringHelper.h"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

CU::ThreadPool* CU::ThreadPool::ourInstance = nullptr;

#ifndef RETAIL
//#define LogThreads
#endif

namespace CU
{
	ThreadPool::ThreadPool()
	{
		myTimerMgr = new TimerManager();
		isStopped = false;
		myConcurentThreadsSupported = std::thread::hardware_concurrency();
		

#ifdef LogThreads
		TCHAR szExeFileName[MAX_PATH];
		GetModuleFileName(NULL, szExeFileName, MAX_PATH);
		TCHAR* out = PathFindFileName(szExeFileName);
		*PathFindExtension(out) = 0;
		std::wstring appName(out);
		myLogPath = "ThreadLogs/Work/";
		myLogPath += std::string(appName.begin(), appName.end()) + "/";
		//SYSTEMTIME st;
		//GetSystemTime(&st);
		/*myLogPath += std::to_string(st.wYear) + "/" + std::to_string(st.wMonth) + "/" + std::to_string(st.wDay) + "/" +
			std::to_string(st.wHour) + "/" + std::to_string(st.wMinute) + "/" + std::to_string(st.wSecond) + "/";*/
		myLogPath += std::to_string(GetEpochTicks()) + "/";

		CreateDirectoryRecursive(myLogPath);
#endif
	}

	void ThreadPool::Create()
	{
		ourInstance = new ThreadPool;
	}

	void ThreadPool::Destroy()
	{
		SAFE_DELETE(ourInstance);
	}

	ThreadPool* ThreadPool::GetInstance()
	{
		if(ourInstance == nullptr)
		{
			DL_ASSERT("Threadpool has not yet been created.");
		}
		return ourInstance;
	}

	void ThreadPool::Init()
	{
		const static std::string name = "ThreadPool Worker: ";

		for (std::size_t i = 0; i < myConcurentThreadsSupported; ++i)
		{

			TimerHandle handle = myTimerMgr->CreateTimer();
			Worker *worker = new Worker();
			worker->Init(*this, handle);
			myWorkers.push_back(worker);

			//so very windows
			std::string threadName = name + std::to_string(i);
			SetThreadName(*worker->myThread, threadName.c_str());

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
		case ePriority::eStandard:;
		default: break;
		}


		std::unique_lock<std::mutex> lock(myQueueMutex);
		workQueue->push(aWork);
		myCondition.notify_one();
	}

	

	bool ThreadPool::IsRunning() const
	{
		return isStopped == false;
	}
#ifdef LogThreads
	void ThreadPool::LogStart()
	{
		std::ofstream output = GetLogStream();
		output << "#LogStart\n";
		output << GetEpochTicks();
		output << "\n";
		output.close();
	}

	void ThreadPool::LogEnd()
	{

		std::ofstream output = GetLogStream();
		output << "#LogEnd\n";
		output << GetEpochTicks();
		output << "\n";
		output.close();
	}

	void ThreadPool::LogName(const std::string& aThreadName)
	{
		std::ofstream output = GetLogStream();
		output << "#LogName\n";
		output << aThreadName;
		output << "\n";
		output.close();
	}

	void ThreadPool::LogCreateThread()
	{
		std::ofstream output = GetLogStream();
		output << "#LogCreate\n";
		output << GetEpochTicks();
		output << "\n";
		output.close();
	}

	void ThreadPool::LogDestroyThread()
	{
		std::ofstream output = GetLogStream();
		output << "#LogDestroy\n";
		output << GetEpochTicks();
		output << "\n";
		output.close();
	}
#else
	void ThreadPool::LogStart()
	{

	}

	void ThreadPool::LogEnd()
	{

	}

	void ThreadPool::LogName(const std::string& aThreadName)
	{

	}

	void ThreadPool::LogCreateThread()
	{

	}

	void ThreadPool::LogDestroyThread()
	{

	}
#endif

	unsigned long long ThreadPool::GetEpochTicks()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

	void ThreadPool::CreateDirectoryRecursive(const std::string& aPath)
	{
		std::vector<std::string> folders = StringHelper::Split(aPath, "/\\");

		if(folders.empty() == false)
		{
			std::string pathString = folders[0];

			CreateDirectory(std::wstring(pathString.begin(), pathString.end()).c_str(), NULL);

			for (int i = 1; i < folders.size(); ++i)
			{
				pathString += "/" + folders[i];
				CreateDirectory(std::wstring(pathString.begin(), pathString.end()).c_str(), NULL);
			}
		}
	}

	std::ofstream ThreadPool::GetLogStream()
	{

		std::stringstream stream;
		stream << std::this_thread::get_id();
		std::string path = myLogPath + stream.str() + ".log";
		
		return std::ofstream(path, std::ios::app);
	}
}
