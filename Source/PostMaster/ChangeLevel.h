#pragma once
#include "Event.h"
#include "../ThreadedPostmaster/Message.h"

class CChangeLevel : public Postmaster::Message::IMessage
{
public:
	CChangeLevel(const eMessageType aType, const int aNewLevelIndex);
	~CChangeLevel();

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	IMessage* Copy() override;
	
	int GetLevelIndex() const;
	State* CreateLoadState(StateStack& aStateStack) const;

private:
	const int myNewLevelIndex;
};
