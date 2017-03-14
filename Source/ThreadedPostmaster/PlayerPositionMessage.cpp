

#include "PlayerPositionMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

CPlayerPositionMessage::CPlayerPositionMessage(const CU::Matrix44f &aTransform, const unsigned anID) : IMessage(eMessageType::eNetworkMessage), myTransform(aTransform), myId(anID)
{
}

CPlayerPositionMessage::~CPlayerPositionMessage()
{
}

Postmaster::Message::IMessage* CPlayerPositionMessage::Copy()
{
	return new CPlayerPositionMessage(myTransform, myId);
}

eMessageReturn CPlayerPositionMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
