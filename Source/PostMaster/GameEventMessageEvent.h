#pragma once
#include "Event.h"
#include "DynamicString.h"

class CGameEventMessageEvent :public Event
{
public:
	CGameEventMessageEvent(CU::GrowingArray<CU::DynamicString> aMessage);
	~CGameEventMessageEvent();

	eMessageReturn DoEvent(CGameEventMessenger*) const override;
private:
	CU::GrowingArray<CU::DynamicString> myMessage;
};

