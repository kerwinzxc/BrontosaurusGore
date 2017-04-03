#pragma once
#include "../ThreadedPostmaster/Message.h"
#include "../CommonUtilities/vector3.h"

struct SExplosionData;

class CCreateExplosionMessage : public Postmaster::Message::IMessage
{
public:
	CCreateExplosionMessage(const CU::Vector3f& aPosition, SExplosionData* aExplosionData, bool aIsPlayerFriendly);
	~CCreateExplosionMessage();

	IMessage* Copy() override;
	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;

	inline const CU::Vector3f& GetPosition() const;
	inline const SExplosionData& GetExplosionData() const;
	inline const bool GetIsPlayerFriendly() const;
private:
	CU::Vector3f myPosition;
	SExplosionData* myExplosionData;
	bool myIsPlayerFriendly;
};

inline const CU::Vector3f& CCreateExplosionMessage::GetPosition() const
{
	return myPosition;
}
inline const SExplosionData& CCreateExplosionMessage::GetExplosionData() const
{
	return *myExplosionData;
}