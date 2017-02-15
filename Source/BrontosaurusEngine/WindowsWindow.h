#pragma once
#include "resource.h"

struct SInitWindowParams;

class CWindowsWindow
{
public:
	CWindowsWindow(const SInitWindowParams& aInitWindowParams);
	~CWindowsWindow();
	void Update();
	void Close();
	inline bool GetIsWindowOpen();
	inline HWND GetHWND();
	inline HINSTANCE GetHinstance();

	const CU::Vector2ui GetWindowSize();

private:
	ATOM MyRegisterClass(HINSTANCE hInstance, const wchar_t* windowName);
	BOOL InitInstance(const SInitWindowParams& aInitWindowParams);
	HINSTANCE myHInst;
	HWND myHWnd;
	MSG myMsg;
	bool isWindowOpen = true;
};

inline bool CWindowsWindow::GetIsWindowOpen()
{
	return isWindowOpen;
}

inline HWND CWindowsWindow::GetHWND()
{
	return myHWnd;
}

inline HINSTANCE CWindowsWindow::GetHinstance()
{
	return myHInst;
}

inline const CU::Vector2ui CWindowsWindow::GetWindowSize()
{
	RECT windowRect;
	GetClientRect(myHWnd, &windowRect);
	unsigned int width, height = width = 0;

	width = windowRect.right - windowRect.left;
	height = windowRect.bottom - windowRect.top;

	return CU::Vector2ui(width, height);
}
