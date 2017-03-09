
#pragma once
#include "../ThreadedPostmaster/Message.h"
#include "../Components/NetworkPlayerReciverComponent.h"

class COtherPlayerSpawned: public Postmaster::Message::IMessage
{
public:
	COtherPlayerSpawned(CNetworkPlayerReciverComponent* aComponent);
	~COtherPlayerSpawned();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	void SetComponent(CNetworkPlayerReciverComponent* aComponent);
	CNetworkPlayerReciverComponent* GetComponent() const;

private:
	CNetworkPlayerReciverComponent* myComponent;
};

inline CNetworkPlayerReciverComponent* COtherPlayerSpawned::GetComponent() const
{
	return  myComponent;
}
