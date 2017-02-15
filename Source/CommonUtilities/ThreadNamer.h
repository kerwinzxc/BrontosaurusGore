#pragma once

namespace std
{
	class thread;
}

namespace CU
{
	//if error with uint32_t, include <cstdint> int stdafx in the project you are using this file
	void SetThreadName(uint32_t dwThreadID, const char* threadName);
	void SetThreadName(const char* threadName);

	void SetThreadName(std::thread& thread, const char* threadName);

	const std::string& GetThreadName();
	const std::string& GetThreadName(std::thread& aThread);
	const std::string& GetThreadName(const uint32_t aThreadID);
}