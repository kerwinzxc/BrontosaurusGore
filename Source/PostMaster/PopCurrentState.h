#pragma once
#include "Event.h"

class PopCurrentState : public Event
{
public:
	PopCurrentState();
	~PopCurrentState();

	//eMessageReturn DoEvent(State* aCurrentState) const override;
	eMessageReturn DoEvent(StateStack* aStateStack) const override;
};

