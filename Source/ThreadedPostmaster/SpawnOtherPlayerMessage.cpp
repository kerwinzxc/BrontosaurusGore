

#include "SpawnOtherPlayerMessage.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

CSpawnOtherPlayerMessage::CSpawnOtherPlayerMessage(unsigned aPlayerID) : IMessage(eMessageType::eNetworkMessage), myPlayerID(aPlayerID)
{
}

CSpawnOtherPlayerMessage::~CSpawnOtherPlayerMessage()
{
}

Postmaster::Message::IMessage* CSpawnOtherPlayerMessage::Copy()
{
	return new CSpawnOtherPlayerMessage(myPlayerID);
}

eMessageReturn CSpawnOtherPlayerMessage::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}
