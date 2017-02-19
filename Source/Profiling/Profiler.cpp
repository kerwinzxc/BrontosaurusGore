#include "Profiler.h"
#include <thread>
#include <iostream>
#include <fstream>


Profiling::CProfiler::CProfiler(bool aProfileCPU, bool aProfileMemory, bool aLiveLog, const std::string& aLiveLogPath):
myCheckThread(nullptr),
myLiveLogPath(aLiveLogPath),
myProfileCPU(aProfileCPU),
myProfileMemory(aProfileMemory),
myIsRunning(true),
myLiveLog(aLiveLog)
{
	if(myProfileCPU)
	{
		InitCPUProfiler();
	}
	if(myProfileMemory)
	{
		InitMemoryProfiler();
	}
}

Profiling::CProfiler::~CProfiler()
{
	if(myCheckThread != nullptr && myCheckThread->joinable() == true)
	{
		myCheckThread->join();
		delete myCheckThread;
	}
	if(myLiveLog == true)
	{
		DeleteFile(myLiveLogPath.c_str());
	}
}

void Profiling::CProfiler::SetStartTime()
{
	
	myStartTime = GetCurrentTime();
}

void Profiling::CProfiler::Start()
{
	myCheckThread = new std::thread(&Profiling::CProfiler::Run, this);
}

void Profiling::CProfiler::Stop()
{
	myIsRunning = false;
	if(myCheckThread != nullptr && myCheckThread->joinable() == true)
	{
		myCheckThread->join();
	}
}

void Profiling::CProfiler::SetEndTime()
{
	myEndTime = GetCurrentTime();
}

void Profiling::CProfiler::Update()
{
	if(myProfileCPU)
	{
		UpdateCPU();
	}
	if(myProfileMemory)
	{
		UpdateMemory();
	}
}

void Profiling::CProfiler::Print() const
{
	if(myProfileCPU == true)
	{
		std::cout << "Average CPU usage: " << myCpuData.average * 100 << "%" << std::endl;
	}
	if(myProfileMemory == true)
	{
		std::cout << "Average memory usage: " << 
			static_cast<double>(myMemoryData.average) / 1000000.f << " MB" << std::endl;
	}
}

void Profiling::CProfiler::Log(const std::string& aLogFilePath)
{
	std::ofstream output(aLogFilePath);

	if(output.good() == true)
	{
		output << "#profiling start time\nstp " << myStartTime << "\n#profiling end time\netp " << myEndTime << "\n\n";

		if(myProfileCPU == true)
		{
			LogCPU(output);
		}
		output << "\n";
		if(myProfileMemory == true)
		{
			LogMemory(output);
		}
		output.close();
	}
}

double Profiling::CProfiler::GetCurrentCPUValue()
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(mySelf, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - myCpuData.lastSysCPU.QuadPart) +
		(user.QuadPart - myCpuData.lastUserCPU.QuadPart);

	const ULONGLONG quadDif = (now.QuadPart - myCpuData.lastCPU.QuadPart);
	if (quadDif != 0)
	{
		percent /= quadDif;
	}

	percent /= myCpuData.numProcessor;
	myCpuData.lastCPU = now;
	myCpuData.lastUserCPU = user;
	myCpuData.lastSysCPU = sys;

	return percent;
}

void Profiling::CProfiler::UpdateCPU()
{
	CPUData data;
	data.percentage = GetCurrentCPUValue();
	if(data.percentage > myCpuData.highest)
	{
		myCpuData.highest = data.percentage;
		myCpuData.highestIndex = myCpuData.samples.size();
	}
	data.time = GetCurrentTime();
	if(myCpuData.average == 0)
	{
		myCpuData.average = data.percentage;
	}
	else
	{
		myCpuData.average += data.percentage;
		myCpuData.average /= 2;
	}
	if(myLiveLog == true)
	{
		LiveLog(data);
	}
	myCpuData.samples.push_back(data);
}

#include <Psapi.h>
void Profiling::CProfiler::UpdateMemory()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(mySelf, &pmc, sizeof(pmc));
	const unsigned memory = pmc.PagefileUsage;
	MemoryData data;
	data.memory = memory;
	data.time = GetCurrentTime();

	if(memory > myMemoryData.highest)
	{
		myMemoryData.highest = memory;
		myMemoryData.highestIndex = myMemoryData.samples.size();
	}
	if(myMemoryData.average == 0)
	{
		myMemoryData.average = memory;
	}
	else
	{
		myMemoryData.average += memory;
		myMemoryData.average /= 2;
	}
	if(myLiveLog == true)
	{
		LiveLog(data);
	}
	myMemoryData.samples.push_back(data);
}

void Profiling::CProfiler::InitCPUProfiler()
{
	SYSTEM_INFO systemInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&systemInfo);
	myCpuData.numProcessor = systemInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&myCpuData.lastCPU, &ftime, sizeof(FILETIME));

	mySelf = GetCurrentProcess();
	GetProcessTimes(mySelf, &ftime, &ftime, &fsys, &fuser);
	memcpy(&myCpuData.lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&myCpuData.lastUserCPU, &fuser, sizeof(FILETIME));
}

void Profiling::CProfiler::InitMemoryProfiler()
{
}

void Profiling::CProfiler::Run()
{
	unsigned sampleInterval = mySampleInterval * 1000000;
	SetStartTime();
	while(myIsRunning == true)
	{
		Update();
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::microseconds(sampleInterval));
	}
	SetEndTime();
}

void Profiling::CProfiler::LogCPU(std::ofstream& aOutput)
{
	aOutput << "#CPU\n" <<
		"##processor count\npcc " << myCpuData.numProcessor << "\n" <<
		"##average usage {percent}\nauc " << myCpuData.average << "\n" <<
		"##highest usage {percent},{index}\nhuc " << myCpuData.highest << "," << myCpuData.highestIndex << "\n" <<
		"##number of samples\nnsc " << myCpuData.samples.size() << "\n" <<
		"##samples\n###{percent},{time},\n";
	for(int i = 0; i < myCpuData.samples.size(); ++i)
	{
		const CPUData& sample = myCpuData.samples[i];
		LogSample(sample, aOutput);
	}
}

void Profiling::CProfiler::LogMemory(std::ofstream& aOutput)
{
	ULONGLONG totalMemory;
	GetPhysicallyInstalledSystemMemory(&totalMemory);
	aOutput << "#Memory\n" <<
		"##total memory {kbytes}\ntm " << totalMemory <<"\n" <<
		"##average usage {kbytes}\naum " << myMemoryData.average / 1024.f << "\n" <<
		"##highest usage {kbytes},{index}\nhum " << myMemoryData.highest / 1024.f << "," << myMemoryData.highestIndex << "\n" <<
		"##number of samples\nnsm " << myMemoryData.samples.size() << "\n" <<
		"##samples\n###{kbytes},{time},\n";
	for (int i = 0; i < myMemoryData.samples.size(); ++i)
	{
		const MemoryData& sample = myMemoryData.samples[i];

		LogSample(sample, aOutput);
		
	}
}

void Profiling::CProfiler::LogSample(const MemoryData& aSample, std::ofstream& aOutput)
{
	aOutput << "sm " << aSample.memory / 1024.f << "," << aSample.time << "\n";
}

void Profiling::CProfiler::LogSample(const CPUData& aSample, std::ofstream& aOutput)
{
	aOutput << "sc " << aSample.percentage << "," << aSample.time << "\n";
}

void Profiling::CProfiler::LiveLog(const CPUData& aData)
{

	std::ofstream output(myLiveLogPath, std::ios::app);
	
	if(output.good() == true)
	{
		LogSample(aData, output);
		output.close();
	}
}

void Profiling::CProfiler::LiveLog(const MemoryData& aData)
{
	std::ofstream output(myLiveLogPath, std::ios::app);

	if (output.good() == true)
	{
		LogSample(aData, output);
		output.close();
	}
}
