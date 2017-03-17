#include "DeactivateExplosionMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"


CDeactivateExplosionMessage::CDeactivateExplosionMessage(CGameObject* aGameObject)
	: IMessage(eMessageType::eDeactivateExplosion), myGameObject(aGameObject)
{
}


CDeactivateExplosionMessage::~CDeactivateExplosionMessage()
{
}


Postmaster::Message::IMessage* CDeactivateExplosionMessage::Copy()
{
	return new CDeactivateExplosionMessage(myGameObject);
}

eMessageReturn CDeactivateExplosionMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}