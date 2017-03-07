#pragma once
#include "Event.h"

class CGameEventMessageEvent :public Event
{
public:
	CGameEventMessageEvent(CU::GrowingArray<std::string> aMessage);
	~CGameEventMessageEvent();

	eMessageReturn DoEvent(CGameEventMessenger*) const override;
private:
	CU::GrowingArray<std::string> myMessage;
};

