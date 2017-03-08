
#pragma once
#include "../ThreadedPostmaster/Message.h"

class CSpawnOtherPlayerMessage: public Postmaster::Message::IMessage
{
public:
	CSpawnOtherPlayerMessage(unsigned aPlayerID);
	~CSpawnOtherPlayerMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const unsigned myPlayerID;


};
