#pragma once
#include "Event.h"

class ChangeLevel : public Event
{
public:
	ChangeLevel(const int aNewLevelIndex);
	~ChangeLevel();

	eMessageReturn DoEvent(StateStack* aStateStack) const override;

private:
	const int myNewLevelIndex;
};
