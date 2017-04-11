#pragma once
#include "ImportantNetworkMessage.h"
class CNetWorkMessage_PickupAmmo : public CImportantNetworkMessage
{
public:
	CNetWorkMessage_PickupAmmo();
	~CNetWorkMessage_PickupAmmo();

	ePackageType GetPackageType()const override;

	const int GetID();
	void SetID(const int aId);

	const void SetReplenishAmount(const short aReplenishAmount);
	const int GetReplenishAmount() const;
private:
	int myNetworkID;
	short myReplenishAmount;


	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

