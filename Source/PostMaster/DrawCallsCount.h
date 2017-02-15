#pragma once
#include "Event.h"

class DrawCallsCount : public Event
{
public:
	DrawCallsCount(const int aDrawCallsCount);
	~DrawCallsCount();

	eMessageReturn DoEvent(CDebugInfoDrawer* aDebugInfoDrawer) const override;

private:
	int myDrawCallsCount;
};
