#pragma once
#include "Event.h"
#include "../ThreadedPostmaster/Message.h"

class FocusChange : public Postmaster::Message::IMessage
{
public:
	FocusChange(const bool aHasFocus);
	~FocusChange();
	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	bool GetHasFocus() const;
	IMessage* Copy() override;
	//eMessageReturn DoEvent(CInputManager* aInputManager) const override;
private:
	bool myHasFocus;
};

