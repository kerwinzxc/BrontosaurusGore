

#include "PlayerPositionMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

CPlayerPositionMessage::CPlayerPositionMessage(const CU::Vector3f &aPosition, const unsigned anID) : IMessage(eMessageType::eNetworkMessage), myPosition(aPosition), myId(anID)
{
}

CPlayerPositionMessage::~CPlayerPositionMessage()
{
}

Postmaster::Message::IMessage* CPlayerPositionMessage::Copy()
{
	return new CPlayerPositionMessage(myPosition, myId);
}

eMessageReturn CPlayerPositionMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
