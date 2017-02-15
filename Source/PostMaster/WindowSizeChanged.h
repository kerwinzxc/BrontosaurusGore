#pragma once
#include "Event.h"

class EvWindowSizeChanged : public Event
{
public:
	EvWindowSizeChanged(const unsigned int aNewWindowWidth, const unsigned int aNewWindowHeight);
	~EvWindowSizeChanged();

private:
	const unsigned int myNewWindowWidth;
	const unsigned int myNewWindowHeight;
};
