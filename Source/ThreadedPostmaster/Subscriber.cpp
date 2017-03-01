#include "Subscriber.h"
#include "Postmaster.h"


Postmaster::ISubscriber::ISubscriber()
{
}


Postmaster::ISubscriber::~ISubscriber()
{
	Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const ConsoleCalledUpon& aConsoleCalledUpon)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const DrawCallsCount& aConsoleCalledupon)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const DroppedFile& aDroppedFile)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const FocusChange& aFocusChange)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const KeyCharPressed& aCharPressed)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const PopCurrentState& aPopCurrent)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const PushState& aPushState)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CNetworkPosMessageEvent& aPositionEvent)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CSendNetowrkMessageMessage& aSendNetowrkMessageMessage)
{
	return eMessageReturn::eContinue;
}

eMessageReturn Postmaster::ISubscriber::DoEvent(const CServerReadyMessage& aSendNetowrkMessageMessage)
{
	return eMessageReturn::eContinue;
}
