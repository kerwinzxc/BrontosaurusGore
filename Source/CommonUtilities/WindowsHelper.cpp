#include "stdafx.h"
#include "WindowsHelper.h"

#include <map>
#include <Windows.h>
#include <shellapi.h>
#include <tlhelp32.h>

static std::map<std::string, PROCESS_INFORMATION> locStartedProcesses;

namespace WindowsHelper
{
	void GoogleIt(const std::string& aGoogleSearch)
	{
		std::string googleString = "https://www.google.se/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#q=";//stack+overflow+my+pointer+is+null&*";
		for (char ch : aGoogleSearch)
		{
			if (ch == ' ')
			{
				ch = '+';
			}

			googleString += ch;
		}
		googleString += "&*";

		ShellExecuteA(NULL, "open", "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe", googleString.c_str(), NULL, SW_SHOWDEFAULT);
	}

	void GoogleIt(const std::wstring& aGoogleSearch)
	{
		GoogleIt(std::string(aGoogleSearch.begin(), aGoogleSearch.end()));
	}

	bool ProgramIsActive(const std::string& aExePath)
	{
		if (locStartedProcesses.find(aExePath) != locStartedProcesses.end())
		{
			return true;
		}

		return ProgramIsActive(std::wstring(aExePath.begin(), aExePath.end()));
	}

	bool ProgramIsActive(const std::wstring& aExePath)
	{
		bool exists = false;

		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry))
		{
			do
			{
				if (!wcsicmp(entry.szExeFile, aExePath.c_str()))
				{
					exists = true;
					break;
				}
			} while (Process32Next(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return exists;
	}

	bool StartProgram(const std::string& aExePath)
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
			return false;
		}

		locStartedProcesses[aExePath] = pi;
		return true;
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
