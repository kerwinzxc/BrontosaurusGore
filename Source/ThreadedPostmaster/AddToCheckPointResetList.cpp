#include "AddToCheckPointResetList.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"


CAddToCheckPointResetList::CAddToCheckPointResetList(CGameObject* aGameObject)
	: IMessage(eMessageType::eAddToCheckPointRespawn), myObjectToReset(aGameObject)
{
}


CAddToCheckPointResetList::~CAddToCheckPointResetList()
{
}

Postmaster::Message::IMessage* CAddToCheckPointResetList::Copy()
{
	return new CAddToCheckPointResetList(myObjectToReset);
}

eMessageReturn CAddToCheckPointResetList::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
