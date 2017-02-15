#pragma once
#include "Event.h"
class ConsoleCalledUpon : public Event
{
public:
	ConsoleCalledUpon(bool aIsConsoleActive);
	~ConsoleCalledUpon();

	eMessageReturn DoEvent(StateStack* aStateStack) const override;
private:
	bool myIsConsoleActive;
};

