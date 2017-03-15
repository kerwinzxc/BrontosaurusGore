
#pragma once
#include "../ThreadedPostmaster/Message.h"

class CNetworkDebugInfo: public Postmaster::Message::IMessage
{
public:
	CNetworkDebugInfo(const int aDataSent, const int aRoundTripTime);
	~CNetworkDebugInfo();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const int myDataSent;
	const int myRoundTripTime;
};
