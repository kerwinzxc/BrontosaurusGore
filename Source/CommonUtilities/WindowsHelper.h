#pragma once

typedef struct HWND__ *HWND;

namespace WindowsHelper
{
	void GoogleIt(const std::string& aGoogleSearch);
	void GoogleIt(const std::wstring& aGoogleSearch);
	void StartProgram(const std::string& aExePath);
	void CloseProgram(const std::string& aExePath);
	void SetFocus(const HWND& aHwnd);
}
