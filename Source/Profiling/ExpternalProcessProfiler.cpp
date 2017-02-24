#include "ExpternalProcessProfiler.h"
#include <Psapi.h>


Profiling::CExpternalProcessProfiler::CExpternalProcessProfiler()
{
	SYSTEM_INFO systemInfo;

	GetSystemInfo(&systemInfo);
	numProcessor = systemInfo.dwNumberOfProcessors;
}


Profiling::CExpternalProcessProfiler::~CExpternalProcessProfiler()
{
}

double Profiling::CExpternalProcessProfiler::GetCurrentCPUUsage(HANDLE aProcessHandle)
{
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(aProcessHandle, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - lastSysCPU.QuadPart) +
		(user.QuadPart - lastUserCPU.QuadPart);

	const ULONGLONG quadDif = (now.QuadPart - lastCPU.QuadPart);
	if (quadDif != 0)
	{
		percent /= quadDif;
	}

	percent /= numProcessor;
	lastCPU = now;
	lastUserCPU = user;
	lastSysCPU = sys;

	return percent;
}

unsigned Profiling::CExpternalProcessProfiler::GetCurrentMemoryUsage(HANDLE aProcessHandle) const
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(aProcessHandle, &pmc, sizeof(pmc));
	const unsigned memory = pmc.PagefileUsage;

	return memory;
}
