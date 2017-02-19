#include "Subscriber.h"
#include "Postmaster.h"


Postmaster::ISubscriber::ISubscriber()
{
}


Postmaster::ISubscriber::~ISubscriber()
{
	Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

void Postmaster::ISubscriber::DoMessage(const Message::CPrintMessage& aPrintMessage)
{
}

