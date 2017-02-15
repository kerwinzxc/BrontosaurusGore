#pragma once
#include "Event.h"
class Pop2States : public Event
{
public:
	Pop2States();
	~Pop2States();

	eMessageReturn DoEvent(StateStack* aStateStack) const override;
};

