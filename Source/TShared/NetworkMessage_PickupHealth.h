#pragma once
#include "ImportantNetworkMessage.h"
class CNetworkMessage_PickupHealth : public CImportantNetworkMessage
{
public:
	CNetworkMessage_PickupHealth();
	~CNetworkMessage_PickupHealth();

	ePackageType GetPackageType()const override;

	const int GetID();
	void SetID(const int aId);

	void SetReplenishAmount(const short aValue);
	const short GetReplenishAmount() const;
private:
	int myNetworkID;
	short myReplenishAmount;

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

