#include "SetAsNewCheckPointMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

CSetAsNewCheckPointMessage::CSetAsNewCheckPointMessage(const CU::Vector3f& aPosition)
	: IMessage(eMessageType::eSetNewCheckPoint), myRespawnPosition(aPosition)
{
}


CSetAsNewCheckPointMessage::~CSetAsNewCheckPointMessage()
{
}

Postmaster::Message::IMessage* CSetAsNewCheckPointMessage::Copy()
{
	return new CSetAsNewCheckPointMessage(myRespawnPosition);
}

eMessageReturn CSetAsNewCheckPointMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
