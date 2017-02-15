#include "stdafx.h"
#include "DrawCallsCount.h"

#include "../BrontosaurusEngine/DebugInfoDrawer.h"

DrawCallsCount::DrawCallsCount(const int aDrawCallsCount)
	: myDrawCallsCount(aDrawCallsCount)
{
}

DrawCallsCount::~DrawCallsCount()
{
}

eMessageReturn DrawCallsCount::DoEvent(CDebugInfoDrawer* aDebugInfoDrawer) const
{
	if (aDebugInfoDrawer != nullptr)
	{
		aDebugInfoDrawer->SetDrawCalls(myDrawCallsCount);
	}

	return eMessageReturn::eContinue;
}
