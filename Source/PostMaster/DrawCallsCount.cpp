#include "stdafx.h"
#include "DrawCallsCount.h"

#include "../BrontosaurusEngine/DebugInfoDrawer.h"

DrawCallsCount::DrawCallsCount(const int aDrawCallsCount)
	:IMessage(eMessageType::eDrawCallsThisFrame), myDrawCallsCount(aDrawCallsCount)
{
}

DrawCallsCount::~DrawCallsCount()
{
}

eMessageReturn DrawCallsCount::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

int DrawCallsCount::GetDrawCalls() const
{
	return myDrawCallsCount;
}

Postmaster::Message::IMessage* DrawCallsCount::Copy()
{
	return new DrawCallsCount(*this);
}
