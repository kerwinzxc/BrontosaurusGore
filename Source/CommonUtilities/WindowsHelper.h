#pragma once
#include <Windows.h>

namespace WindowsHelper
{
	void GoogleIt();
	void StartProgram(const std::string& aExePath);
	void CloseProgram(const std::string& aExePath);
	void SetFocus(const HWND& aHwnd);
}
