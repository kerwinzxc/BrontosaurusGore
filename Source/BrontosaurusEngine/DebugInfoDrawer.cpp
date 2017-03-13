#include "stdafx.h"
#include "DebugInfoDrawer.h"

#include "EDebugInfoDrawerFlags.h"
#include "TextInstance.h"
#include <Psapi.h>

#include "../CommonUtilities/BitSet.h"
#include "../CommonUtilities/TimerManager.h"
#include "../CommonUtilities/CountDown.h"
#include "../CommonUtilities/EKeyboardKeys.h"
#include "../CommonUtilities/InputMessage.h"
#include "../CommonUtilities/InputMessenger.h"
#include "../CommonUtilities/EInputReturn.h"

#include "../PostMaster/Message.h"
#include "../PostMaster/DrawCallsCount.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/NetworkDebugInfo.h"

CDebugInfoDrawer::CDebugInfoDrawer(unsigned int aDebugFlags)
	: CInputMessenger(eInputMessengerType::eDebugInfoDrawer, 20)
	, myOutputTexts(nullptr)
	, myCountDown(nullptr)
	, myLogicThreadTimers(nullptr)
	, myLogicFPSTimer(0)
	, myUpdateTextTimer(0)
	, myUpdateTextTimer_RenderThread(0)
	, myFPSTimer(0)
	, myDrawCallsCount(0)
	, myDebugFlags(aDebugFlags)
	, myLeftControlIsDown(false)
	, myRTT(0)
	, myDataSent(0)
{
#ifndef _RETAIL_BUILD

	myOutputTexts[eDebugText_FPS] = new CTextInstance();
	myOutputTexts[eDebugText_FPS]->Init();
	myOutputTexts[eDebugText_FPS]->SetText(L"FPS: ");

	myOutputTexts[eDebugText_LogicFPS] = new CTextInstance();
	myOutputTexts[eDebugText_LogicFPS]->Init();
	myOutputTexts[eDebugText_LogicFPS]->SetText(L"LOGIC FPS: ");

	myOutputTexts[eDebugText_DrawCalls] = new CTextInstance();
	myOutputTexts[eDebugText_DrawCalls]->Init();
	myOutputTexts[eDebugText_DrawCalls]->SetText(L"DRAWCALLS: ");

	myOutputTexts[eDebugText_MemoryUsage] = new CTextInstance();
	myOutputTexts[eDebugText_MemoryUsage]->Init();
	myOutputTexts[eDebugText_MemoryUsage]->SetText(L"MEMORY (MB): ");

	myOutputTexts[eDebugText_RoundTripTime] = new CTextInstance();
	myOutputTexts[eDebugText_RoundTripTime]->Init();
	myOutputTexts[eDebugText_RoundTripTime]->SetText(L"RTT  (MS): ");

	myOutputTexts[eDebugText_DataAmmountSent] = new CTextInstance();
	myOutputTexts[eDebugText_DataAmmountSent]->Init();
	myOutputTexts[eDebugText_DataAmmountSent]->SetText(L"DATA SENT (MB): ");

	myCountDown = new CU::CountDown();

	myLogicThreadTimers = new CU::TimerManager();
	myRenderThreadTimers = new CU::TimerManager();

	myLogicFPSTimer = myLogicThreadTimers->CreateTimer();
	myUpdateTextTimer = myLogicThreadTimers->CreateTimer();
	myUpdateTextTimer_RenderThread = myRenderThreadTimers->CreateTimer();
	myFPSTimer = myRenderThreadTimers->CreateTimer();

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eDrawCallsThisFrame);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eDebugInfo);
#endif // !_RETAIL_BUILD
}

CDebugInfoDrawer::~CDebugInfoDrawer()
{
#ifndef _RETAIL_BUILD
	SAFE_DELETE(myLogicThreadTimers);
	SAFE_DELETE(myRenderThreadTimers);
	SAFE_DELETE(myCountDown);

	//PostMaster::GetInstance().UnSubscribe(this, eMessageType::eDrawCallsThisFrame);

	myOutputTexts.DeleteAll();
#endif // !_RETAIL_BUILD
}

void CDebugInfoDrawer::Render(const CU::Vector2ui aTargetWindowSize)
{
#ifndef _RETAIL_BUILD

	const CU::Vector2f targetWindowSize(/*1920.f, 1080.f*/aTargetWindowSize);
	const float xStartPos = 2.f / targetWindowSize.x;
	const float yStartPos = 2.f / targetWindowSize.y;
	const float yOffset = 25.f / targetWindowSize.y;

	int yCount = 1;
	for (int i = 0; i < myOutputTexts.Size(); ++i)
	{
		if (myDebugFlags[i] == true)
		{
			myOutputTexts[i]->SetPosition({ xStartPos, yStartPos + yOffset * yCount++ });
			myOutputTexts[i]->Render();
		}
	}

	myCountDown->Render();

#endif // !_RETAIL_BUILD
}

void CDebugInfoDrawer::Update()
{
#ifndef _RETAIL_BUILD
	myLogicThreadTimers->UpdateTimers();

	UpdateLogicFPSCounter();
	UpdateDrawCallsCounter();
	UpdateMemoryUsage();
	UpdateNetworkDebug();

	myCountDown->Update();

#endif // !_RETAIL_BUILD
}



void CDebugInfoDrawer::PressedKey(const CU::eKeys& aKey)
{
	SUPRESS_UNUSED_WARNING(aKey);

#ifndef _RETAIL_BUILD

	switch (aKey)
	{
	case CU::eKeys::LCONTROL:
		myLeftControlIsDown = true;
		break;
	case CU::eKeys::FOUR:
		if (myLeftControlIsDown == true)
		{
			myDebugFlags.Flip(eDebugText_FPS);
		}
		break;
	case CU::eKeys::FIVE:
		if (myLeftControlIsDown == true)
		{
			myDebugFlags.Flip(eDebugText_LogicFPS);
		}
		break;
	case CU::eKeys::SIX:
		if (myLeftControlIsDown == true)
		{
			myDebugFlags.Flip(eDebugText_DrawCalls);
		}
		break;
	case CU::eKeys::SEVEN:
		if (myLeftControlIsDown == true)
		{
			myDebugFlags.Flip(eDebugText_MemoryUsage);
		}
		break;
	case CU::eKeys::EIGHT:
		if (myLeftControlIsDown == true)
		{
			myDebugFlags.Flip(eDebugText_RoundTripTime);
		}
		break;
	case CU::eKeys::NINE:
		if (myLeftControlIsDown == true)
		{
			myDebugFlags.Flip(eDebugText_DataAmmountSent);
		}
		break;
	default:
		break;
	}

#endif // !_RETAIL_BUILD
}

void CDebugInfoDrawer::ReleasedKey(const CU::eKeys& aKey)
{
	switch (aKey)
	{
	case CU::eKeys::LCONTROL:
		myLeftControlIsDown = false;
		break;
	default:
		break;
	}
}

void CDebugInfoDrawer::SetDrawCalls(const int aDrawCallsCount)
{
	myDrawCallsCount = aDrawCallsCount;
}

void CDebugInfoDrawer::UpdateFPSCounter()
{
#ifndef _RETAIL_BUILD

	myRenderThreadTimers->UpdateTimers();

	const float SecondsBetweenRefreshes = 0.125f;
	
	if (myRenderThreadTimers->GetTimer(myUpdateTextTimer_RenderThread).GetLifeTime().GetSeconds() > SecondsBetweenRefreshes)
	{
		myRenderThreadTimers->ResetTimer(myUpdateTextTimer_RenderThread);

		const int currentFPS = static_cast<int>(myRenderThreadTimers->GetTimer(myFPSTimer).GetFPS());
		myOutputTexts[eDebugText_FPS]->SetText(std::wstring(L"FPS: ") + std::to_wstring(currentFPS));

		if (currentFPS < 30)
		{
			myOutputTexts[eDebugText_FPS]->SetColor(CU::Vector4f(CTextInstance::Red.x * 0.5f, CTextInstance::Red.y * 0.5f, CTextInstance::Red.z * 0.5f, 1.f));
		}
		else if (currentFPS < 60)
		{
			myOutputTexts[eDebugText_FPS]->SetColor(CU::Vector4f(CTextInstance::Yellow.x * 0.5f, CTextInstance::Yellow.y * 0.5f, CTextInstance::Yellow.z * 0.5f, 1.f));
		}
		else
		{
			myOutputTexts[eDebugText_FPS]->SetColor(CU::Vector4f(CTextInstance::White.x * 0.5f, CTextInstance::White.y * 0.5f, CTextInstance::White.z * 0.5f, 1.f));
		}
	}

#endif // !_RETAIL_BUILD
}

eMessageReturn CDebugInfoDrawer::DoEvent(const DrawCallsCount& aConsoleCalledupon)
{
	SetDrawCalls(aConsoleCalledupon.GetDrawCalls());
	return eMessageReturn::eContinue;
}

CU::eInputReturn CDebugInfoDrawer::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	if (aInputMessage.myType == CU::eInputType::eKeyboardPressed)
	{
		PressedKey(aInputMessage.myKey);
	}
	else if (aInputMessage.myType == CU::eInputType::eKeyboardReleased)
	{
		ReleasedKey(aInputMessage.myKey);
	}

	return CU::eInputReturn::ePassOn;
}

void CDebugInfoDrawer::SetNetoworkDebugData(const int aDataSent, const int aRoundTripTime)
{
	myDataSent = aDataSent;
	myRTT = aRoundTripTime;
}

eMessageReturn CDebugInfoDrawer::DoEvent(const CNetworkDebugInfo& someDebugInfo)
{
	SetNetoworkDebugData(someDebugInfo.myDataSent, someDebugInfo.myRoundTripTime);
	return eMessageReturn::eContinue;
}

void CDebugInfoDrawer::UpdateLogicFPSCounter()
{
#ifndef _RETAIL_BUILD
	const float SecondsBetweenRefreshes = 0.125f;

	if (myLogicThreadTimers->GetTimer(myUpdateTextTimer).GetLifeTime().GetSeconds() > SecondsBetweenRefreshes)
	{
		myLogicThreadTimers->ResetTimer(myUpdateTextTimer);

		const int currentFPS = static_cast<int>(myLogicThreadTimers->GetTimer(myLogicFPSTimer).GetFPS());
		myOutputTexts[eDebugText_LogicFPS]->SetText(std::wstring(L"LOGIC FPS: ") + std::to_wstring(currentFPS));

		if (currentFPS < 30)
		{
			myOutputTexts[eDebugText_LogicFPS]->SetColor(CU::Vector4f(CTextInstance::Red.x * 0.5f, CTextInstance::Red.y * 0.5f, CTextInstance::Red.z * 0.5f, 1.f));
		}
		else if (currentFPS < 60)
		{
			myOutputTexts[eDebugText_LogicFPS]->SetColor(CU::Vector4f(CTextInstance::Yellow.x * 0.5f, CTextInstance::Yellow.y * 0.5f, CTextInstance::Yellow.z * 0.5f, 1.f));
		}
		else
		{
			myOutputTexts[eDebugText_LogicFPS]->SetColor(CU::Vector4f(CTextInstance::White.x * 0.5f, CTextInstance::White.y * 0.5f, CTextInstance::White.z * 0.5f, 1.f));
		}
	}
#endif // !_RETAIL_BUILD
}

void CDebugInfoDrawer::UpdateMemoryUsage()
{
	PROCESS_MEMORY_COUNTERS memCounter = {};
	BOOL result = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
	if (result == TRUE)
	{
		int memUsedkB = static_cast<int>(memCounter.WorkingSetSize / 1024);
		int memUsedMB = static_cast<int>(memCounter.WorkingSetSize / 1024 / 1024);
		if (memUsedkB < 1000)
		{
			myOutputTexts[eDebugText_MemoryUsage]->SetText(std::wstring(L"MEMORY (kB): ") + std::to_wstring(memUsedkB));
		}
		else
		{
			myOutputTexts[eDebugText_MemoryUsage]->SetText(std::wstring(L"MEMORY (MB): ") + std::to_wstring(memUsedMB));
		}

		if (memUsedMB > 1500)
		{
			myOutputTexts[eDebugText_MemoryUsage]->SetColor(CU::Vector4f(CTextInstance::Red.x * 0.5f, CTextInstance::Red.y * 0.5f, CTextInstance::Red.z * 0.5f, 1.f));
		}
		else if (memUsedMB > 1000)
		{
			myOutputTexts[eDebugText_MemoryUsage]->SetColor(CU::Vector4f(CTextInstance::Yellow.x * 0.5f, CTextInstance::Yellow.y * 0.5f, CTextInstance::Yellow.z * 0.5f, 1.f));
		}
		else
		{
			myOutputTexts[eDebugText_MemoryUsage]->SetColor(CU::Vector4f(CTextInstance::White.x * 0.5f, CTextInstance::White.y * 0.5f, CTextInstance::White.z * 0.5f, 1.f));
		}
	}
}

void CDebugInfoDrawer::UpdateNetworkDebug()
{
#ifndef _RETAIL_BUILD
	myOutputTexts[eDebugText_DataAmmountSent]->SetText(std::wstring(L"DATA SENT(kB/s) : ") + CU::StringHelper::ToWStringWithPrecision(static_cast<float>(myDataSent) / 1000, 3));
	myOutputTexts[eDebugText_DataAmmountSent]->SetColor(CU::Vector4f(0.5, 0.5, 0.5, 1));

	myOutputTexts[eDebugText_RoundTripTime]->SetText(std::wstring(L"RTT(ms) : ") + std::to_wstring(myRTT));
	myOutputTexts[eDebugText_RoundTripTime]->SetColor(CU::Vector4f(0.5, 0.5, 0.5, 1));
#endif //!_RETAIL_BUILD
}

void CDebugInfoDrawer::UpdateDrawCallsCounter()
{
	myOutputTexts[eDebugText_DrawCalls]->SetText(std::wstring(L"DRAWCALLS: ") + std::to_wstring(myDrawCallsCount));
	if (myDrawCallsCount > 2000)
	{
		myOutputTexts[eDebugText_DrawCalls]->SetColor(CU::Vector4f(CTextInstance::Red.x * 0.5f, CTextInstance::Red.y * 0.5f, CTextInstance::Red.z * 0.5f, 1.f));
	}
	else if (myDrawCallsCount > 1000)
	{
		myOutputTexts[eDebugText_DrawCalls]->SetColor(CU::Vector4f(CTextInstance::Yellow.x * 0.5f, CTextInstance::Yellow.y * 0.5f, CTextInstance::Yellow.z * 0.5f, 1.f));
	}
	else
	{
		myOutputTexts[eDebugText_DrawCalls]->SetColor(CU::Vector4f(CTextInstance::White.x * 0.5f, CTextInstance::White.y * 0.5f, CTextInstance::White.z * 0.5f, 1.f));
	}
}
