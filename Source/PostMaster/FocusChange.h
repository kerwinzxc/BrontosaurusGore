#pragma once
#include "Event.h"
class FocusChange : public Event
{
public:
	FocusChange(const bool aHasFocus);
	~FocusChange();
	eMessageReturn DoEvent(CInputManager* aInputManager) const override;
private:
	bool myHasFocus;
};

