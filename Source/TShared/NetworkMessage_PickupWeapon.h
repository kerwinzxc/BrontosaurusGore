#pragma once
#include "ImportantNetworkMessage.h"
class CNetworkMessage_PickupWeapon :
	public CImportantNetworkMessage
{
public:
	CNetworkMessage_PickupWeapon();
	~CNetworkMessage_PickupWeapon();

	void SetWeapon(const unsigned short aWeaponID);
	const unsigned short GetWeaponPickup() const; 

	void SetID(const int aId);
	const int GetID();

	ePackageType GetPackageType()const override;

	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
private:
	unsigned short myWeaponID;
	int myNetworkID;
};

