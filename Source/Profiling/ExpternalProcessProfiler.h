#pragma once
#include <Windows.h>
namespace Profiling
{
	class CExpternalProcessProfiler
	{
	public:
		CExpternalProcessProfiler();
		~CExpternalProcessProfiler();


		double GetCurrentCPUUsage(HANDLE aProcessHandle);
		unsigned GetCurrentMemoryUsage(HANDLE aProcessHandle) const;
	protected:
		ULARGE_INTEGER lastSysCPU;
		ULARGE_INTEGER lastUserCPU;
		ULARGE_INTEGER lastCPU;
		unsigned numProcessor;
	};
}
