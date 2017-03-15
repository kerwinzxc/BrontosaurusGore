#pragma once
#include "../ThreadedPostmaster/Message.h"
#include "../CommonUtilities/vector3.h"

class CSetAsNewCheckPointMessage : public Postmaster::Message::IMessage
{
public:
	CSetAsNewCheckPointMessage(const CU::Vector3f& aPosition );
	~CSetAsNewCheckPointMessage();

	IMessage* Copy() override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	inline const CU::Vector3f& GetCheckPointPosition() const;
private:
	CU::Vector3f myRespawnPosition;
};

inline const CU::Vector3f& CSetAsNewCheckPointMessage::GetCheckPointPosition() const
{
	return myRespawnPosition;
}