#include "CreateExplosionMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

CCreateExplosionMessage::CCreateExplosionMessage(const CU::Vector3f& aPosition, SExplosionData* aExplosionData)
	: IMessage(eMessageType::eCreateExplosionMessage), myPosition(aPosition), myExplosionData(aExplosionData)
{
}


CCreateExplosionMessage::~CCreateExplosionMessage()
{
}

Postmaster::Message::IMessage* CCreateExplosionMessage::Copy()
{
	return new CCreateExplosionMessage(myPosition, myExplosionData);
}

eMessageReturn CCreateExplosionMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}