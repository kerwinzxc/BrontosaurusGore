#pragma once
#include "ImportantNetworkMessage.h"
class CNetworkMessage_PickupWeapon :
	public CImportantNetworkMessage
{
public:
	enum class eWeaponPickupType
	{
		eShotgun,
		ePlasmaRifle,
		eBFG,
	};


	CNetworkMessage_PickupWeapon();
	~CNetworkMessage_PickupWeapon();

	void SetWeapon(const std::string& aWeaponName);
	const eWeaponPickupType GetWeaponPickup() const; 

	void SetID(const int aId);
	const int GetID();

	ePackageType GetPackageType()const override;
private:
	eWeaponPickupType myWeaponPickup;
	int myNetworkID;
};

