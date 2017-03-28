#include "stdafx.h"
#include "WindowsHelper.h"

#include <map>
#include <Windows.h>
#include <shellapi.h>

static std::map<std::string, PROCESS_INFORMATION> locStartedProcesses;

namespace WindowsHelper
{
	void GoogleIt()
	{
		std::string googleString = "https://www.google.se/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#q=stack+overflow+please+help&*";
		ShellExecuteA(NULL, "open", "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe", googleString.c_str(), NULL, SW_SHOWDEFAULT);
	}

	void StartProgram(const std::string& aExePath)
	{
		//HINSTANCE newProgram = ShellExecuteA(nullptr, "open", aExePath.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
		//int success = (int)newProgram;
		//if (success <= 32)
		//{
		//	int apa = 0;
		//}
		//return newProgram;

		std::wstring widePath(aExePath.begin(), aExePath.end());

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// start the program up
		if (!CreateProcess(widePath.c_str(),   // the path
			nullptr,        // Command line argv[1]
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			CREATE_NEW_CONSOLE,
			//0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		))
		{
			int br = 0;
		}

		locStartedProcesses[aExePath] = pi;
	}

	void CloseProgram(const std::string& aExePath)
	{
		auto it = locStartedProcesses.find(aExePath);
		if (it != locStartedProcesses.end())
		{
			TerminateProcess(it->second.hProcess, 0);

			CloseHandle(it->second.hProcess);
			CloseHandle(it->second.hThread);
		}
	}
}