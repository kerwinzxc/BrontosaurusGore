
#pragma once
#include "../ThreadedPostmaster/Message.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/matrix44.h"

class CPlayerPositionMessage: public Postmaster::Message::IMessage
{
public:
	CPlayerPositionMessage(const CU::Matrix44f &aTransform, const unsigned anID);
	~CPlayerPositionMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	const CU::Matrix44f myTransform;
	const unsigned myId;
private:
	const CU::Vector3f myPosition;
};
