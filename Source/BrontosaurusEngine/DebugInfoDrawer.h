#pragma once

#include "../CommonUtilities/StaticArray.h"
#include "../PostMaster/Subscriber.h"

#include <atomic>
#include "../CommonUtilities/BitSet.h"
#include "../ThreadedPostmaster/Subscriber.h"

namespace CU
{
	class CountDown;

	template <typename T>
	class Vector2;
	using Vector2ui = Vector2<unsigned int>;

	class TimerManager;
	typedef unsigned int TimerHandle;

	enum class eKeys;
}

class CTextInstance;

class CDebugInfoDrawer : public Postmaster::ISubscriber
{
public:
	CDebugInfoDrawer(unsigned int aDebugFlags = 0);
	~CDebugInfoDrawer();

	void Render(const CU::Vector2ui aTargetWindowSize);
	void Update();

	
	void PressedKey(const CU::eKeys& aKey);
	void ReleasedKey(const CU::eKeys& aKey);

	void SetDrawCalls(const int aDrawCallsCount);
	void UpdateFPSCounter();

	

	eMessageReturn DoEvent(const DrawCallsCount& aConsoleCalledupon) override;
	eMessageReturn DoEvent(const CNetworkDebugInfo& someDebugInfo) override;

private:
	void UpdateLogicFPSCounter();
	void UpdateDrawCallsCounter();
	void UpdateMemoryUsage();
	void UpdateNetworkDebug();


	void SetNetoworkDebugData(const int aDataSent, const int aRoundTripTime);

	enum eDebugText : unsigned int
	{
		eDebugText_FPS,
		eDebugText_LogicFPS,
		eDebugText_DrawCalls,
		eDebugText_MemoryUsage,
		eDebugText_RoundTripTime,
		eDebugText_DataAmmountSent,
		eDebugText_Length,
	};

	CU::StaticArray<CTextInstance*, eDebugText_Length> myOutputTexts;

	CU::CountDown* myCountDown;

	CU::TimerManager* myLogicThreadTimers;
	CU::TimerManager* myRenderThreadTimers;
	CU::TimerHandle myLogicFPSTimer;
	CU::TimerHandle myUpdateTextTimer;
	CU::TimerHandle myUpdateTextTimer_RenderThread;
	CU::TimerHandle myFPSTimer;

	int myDrawCallsCount;
	int myRTT;
	int myDataSent;

	CU::CBitSet<eDebugText_Length> myDebugFlags;

	std::atomic_bool myLeftControlIsDown;
};
