
#pragma once
#include "../ThreadedPostmaster/Message.h"
#include "../CommonUtilities/vector3.h"

class CPlayerPositionMessage: public Postmaster::Message::IMessage
{
public:
	CPlayerPositionMessage(const CU::Vector3f &aPosition, const unsigned anID);
	~CPlayerPositionMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const CU::Vector3f myPosition;
	const unsigned myId;
};
