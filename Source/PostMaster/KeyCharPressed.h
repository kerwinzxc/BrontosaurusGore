#pragma once
#include "Event.h"
class KeyCharPressed : public Event
{
public:
	KeyCharPressed(const char aKey);
	~KeyCharPressed();

	eMessageReturn DoEvent(CConsole* aConsole) const override;

private:
	const char myKey;
};

