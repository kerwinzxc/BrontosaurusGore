

#include "OtherPlayerSpawned.h"
#include "../PostMaster/MessageType.h"
#include "../ThreadedPostmaster/Subscriber.h"

COtherPlayerSpawned::COtherPlayerSpawned(CNetworkPlayerReciverComponent* aComponent) : IMessage(eMessageType::eNetworkMessage), myComponent(aComponent)
{
}

COtherPlayerSpawned::~COtherPlayerSpawned()
{
}

Postmaster::Message::IMessage* COtherPlayerSpawned::Copy()
{
	return new COtherPlayerSpawned(myComponent);
}

eMessageReturn COtherPlayerSpawned::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

void COtherPlayerSpawned::SetComponent(CNetworkPlayerReciverComponent* aComponent)
{
	myComponent = aComponent;
}
