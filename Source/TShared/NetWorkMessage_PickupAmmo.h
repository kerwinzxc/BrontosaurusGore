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

	void SetWeaponID(const short aID);
	const short GetWeaponID() const;

	const void SetReplenishAmount(const short aReplenishAmount);
	const short GetReplenishAmount() const;
private:
	int myNetworkID;
	short myReplenishAmount;
	short myWeaponID;


	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
};

