#pragma once
#include "Event.h"
#include "../ThreadedPostmaster/Message.h"

class DrawCallsCount : public Postmaster::Message::IMessage
{
public:
	DrawCallsCount(const int aDrawCallsCount);
	~DrawCallsCount();

	//eMessageReturn DoEvent(CDebugInfoDrawer* aDebugInfoDrawer) const override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	int GetDrawCalls() const;
	inline IMessage* Copy() override;
private:
	int myDrawCallsCount;
};
