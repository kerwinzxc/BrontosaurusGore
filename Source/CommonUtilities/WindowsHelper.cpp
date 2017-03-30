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
			BOOL threadResult = TerminateThread(it->second.hThread, 0);
			BOOL result = TerminateProcess(it->second.hProcess, 0);
			DL_PRINT("%s", (result) ? "true" : "false");
			CloseHandle(it->second.hProcess);
			CloseHandle(it->second.hThread);
		}
	}

	void SetFocus(const HWND& aHwnd)
	{
		::SetForegroundWindow(aHwnd);
	}
}
