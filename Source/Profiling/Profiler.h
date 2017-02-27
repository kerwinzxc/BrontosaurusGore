#pragma once
#include <Windows.h>
#include <vector>
#include <thread>
#include "ExpternalProcessProfiler.h"

namespace Profiling
{
	class CProfiler
	{
	public:
		CProfiler(bool aProfileCPU, bool aProfileMemory, bool aLiveLog, const std::string& aLiveLogPath);
		~CProfiler();

		void SetStartTime();
		void Start();
		void Stop();
		void SetEndTime();

		void Update();

		void SetSampleInterval(float aSampleIntervalInSeconds);

		void Print() const;
		void Log(const std::string& aLogFilePath);
	protected:
		struct CPUData
		{
			double percentage;
			unsigned time;
		};
		struct MemoryData
		{
			unsigned int memory;
			unsigned time;
		};

		double GetCurrentCPUValue();
		void UpdateCPU();
		void UpdateMemory();

		void InitCPUProfiler();
		void InitMemoryProfiler();

		void Run();

		void LogCPU(std::ofstream& aOutput);
		void LogMemory(std::ofstream& aOutput);
		void LogSample(const MemoryData& aSample, std::ofstream& aOutput);
		void LogSample(const CPUData& aSample, std::ofstream& aOutput);

		void LiveLog(const CPUData& aData);
		void LiveLog(const MemoryData& aData);

		unsigned myStartTime = 0;
		unsigned myEndTime = 0;

		Profiling::CExpternalProcessProfiler myExternalProfiler;

		struct
		{
			ULARGE_INTEGER lastCPU;
			ULARGE_INTEGER lastSysCPU;
			ULARGE_INTEGER lastUserCPU;

			unsigned numProcessor;

			double average = 0;
			double highest = 0;
			size_t highestIndex;

			std::vector<CPUData> samples;
		} myCpuData;

		struct
		{
			unsigned average = 0;
			unsigned highest = 0;
			size_t highestIndex;;
			std::vector<MemoryData> samples;
		} myMemoryData;

		HANDLE mySelf;
		ULARGE_INTEGER myUser;

		float mySampleInterval = 0;

		std::thread* myCheckThread;

		std::string myLiveLogPath;

		bool myProfileCPU;
		bool myProfileMemory;
		volatile bool myIsRunning;
		bool myLiveLog;
	};
}

inline void Profiling::CProfiler::SetSampleInterval(float aSampleIntervalInSeconds)
{
	mySampleInterval = aSampleIntervalInSeconds;
}